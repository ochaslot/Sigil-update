/************************************************************************
**
**  Copyright (C) 2009, 2010, 2011  Strahinja Markovic  <strahinja.markovic@gmail.com>
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include <memory>
#include <functional>

#include <QtCore/QtCore>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFutureSynchronizer>
#include <QtConcurrent/QtConcurrent>

#include "BookManipulation/CleanSource.h"
#include "BookManipulation/FolderKeeper.h"
#include "BookManipulation/Metadata.h"
#include "BookManipulation/XhtmlDoc.h"
#include "Importers/ImportHTML.h"
#include "Misc/GumboInterface.h"
#include "Misc/HTMLEncodingResolver.h"
#include "Misc/SettingsStore.h"
#include "Misc/TempFolder.h"
#include "Misc/Utility.h"
#include "ResourceObjects/CSSResource.h"
#include "ResourceObjects/HTMLResource.h"
#include "ResourceObjects/NCXResource.h"
#include "SourceUpdates/PerformHTMLUpdates.h"
#include "SourceUpdates/UniversalUpdates.h"
#include "sigil_constants.h"
#include "sigil_exception.h"

// Constructor;
// The parameter is the file to be imported
ImportHTML::ImportHTML(const QString &fullfilepath)
    :
    Importer(fullfilepath),
    m_IgnoreDuplicates(false),
    m_CachedSource(QString())
{
    SettingsStore ss;
    m_EpubVersion = ss.defaultVersion();
}


void ImportHTML::SetBook(QSharedPointer<Book> book, bool ignore_duplicates)
{
    m_Book = book;
    m_IgnoreDuplicates = ignore_duplicates;
    // update epub version to match the book that was just set
    m_EpubVersion = m_Book->GetConstOPF()->GetEpubVersion();
}


XhtmlDoc::WellFormedError ImportHTML::CheckValidToLoad()
{
    // For HTML & XML documents we will perform a well-formed check
    return XhtmlDoc::WellFormedErrorForSource(LoadSource());
}


// Reads and parses the file
// and returns the created Book
QSharedPointer<Book> ImportHTML::GetBook()
{
    QString source = LoadSource();
    LoadMetadata(source);
    UpdateFiles(CreateHTMLResource(), source, LoadFolderStructure(source));
    return m_Book;
}


// Loads the source code into the Book
QString ImportHTML::LoadSource()
{
    SettingsStore ss;

    if (m_CachedSource.isNull()) {
        if (!Utility::IsFileReadable(m_FullFilePath)) {
            throw (CannotReadFile(m_FullFilePath.toStdString()));
        }

        m_CachedSource = HTMLEncodingResolver::ReadHTMLFile(m_FullFilePath);
        m_CachedSource = CleanSource::CharToEntity(m_CachedSource);

        if (ss.cleanOn() & CLEANON_OPEN) {
          m_CachedSource = CleanSource::Mend(XhtmlDoc::ResolveCustomEntities(m_CachedSource), m_EpubVersion);
        }
    }

    return m_CachedSource;
}


// Searches for meta information in the HTML file
// and tries to convert it to Dublin Core
void ImportHTML::LoadMetadata(const QString & source)
{
    GumboInterface gi(source, m_EpubVersion);
    gi.parse();
    QList<Metadata::MetaElement> metadata;
    QList<GumboNode*> nodes = gi.get_all_nodes_with_tag(GUMBO_TAG_META); 
    for (int i = 0; i < nodes.count(); ++i) {
        GumboNode* node = nodes.at(i);
        Metadata::MetaElement book_meta = Metadata::Instance()->MapToBookMetadata(node, gi);
        if (!book_meta.name.isEmpty() && !book_meta.value.toString().isEmpty()) {
            metadata.append(book_meta);
        }
    }

    m_Book->SetMetadata(metadata);
}


HTMLResource *ImportHTML::CreateHTMLResource()
{
    TempFolder tempfolder;
    QString fullfilepath = tempfolder.GetPath() + "/" + QFileInfo(m_FullFilePath).fileName();
    Utility::WriteUnicodeTextFile("TEMP_SOURCE", fullfilepath);
    HTMLResource *resource = qobject_cast<HTMLResource *>(m_Book->GetFolderKeeper()->AddContentFileToFolder(fullfilepath));
    resource->SetCurrentBookRelPath(m_FullFilePath);
    return resource;
}


void ImportHTML::UpdateFiles(HTMLResource *html_resource,
                             QString &source, 
                             const QHash<QString, QString> &updates)
{
    Q_ASSERT(html_resource != NULL);
    QHash<QString, QString> html_updates;
    QHash<QString, QString> css_updates;
    QString newsource = source;
    QString currentpath = html_resource->GetCurrentBookRelPath();
    QString version = html_resource->GetEpubVersion();
    std::tie(html_updates, css_updates, std::ignore) =
        UniversalUpdates::SeparateHtmlCssXmlUpdates(updates);
    QList<Resource *> all_files = m_Book->GetFolderKeeper()->GetResourceList();
    int num_files = all_files.count();
    QList<CSSResource *> css_resources;

    for (int i = 0; i < num_files; ++i) {
        Resource *resource = all_files.at(i);

        if (resource->Type() == Resource::CSSResourceType) {
            css_resources.append(qobject_cast<CSSResource *>(resource));
        }
    }
    
    QFutureSynchronizer<void> sync;
    sync.addFuture(QtConcurrent::map(css_resources,
                                     std::bind(UniversalUpdates::LoadAndUpdateOneCSSFile, std::placeholders::_1, css_updates)));
    html_resource->SetText(PerformHTMLUpdates(newsource, html_updates, css_updates, currentpath, version)());
    html_resource->SetCurrentBookRelPath("");
    sync.waitForFinished();
}


// Loads the referenced files into the main folder of the book;
// as the files get a new name, the references are updated
QHash<QString, QString> ImportHTML::LoadFolderStructure(const QString &source)
{
    QStringList mediapaths = XhtmlDoc::GetPathsToMediaFiles(source);
    QStringList stylepaths = XhtmlDoc::GetPathsToStyleFiles(source);
    QFutureSynchronizer<QHash<QString, QString>> sync;
    sync.addFuture(QtConcurrent::run(this, &ImportHTML::LoadMediaFiles,  mediapaths));
    sync.addFuture(QtConcurrent::run(this, &ImportHTML::LoadStyleFiles,  stylepaths));
    sync.waitForFinished();
    QList<QFuture<QHash<QString, QString>>> futures = sync.futures();
    int num_futures = futures.count();
    QHash<QString, QString> updates;

    for (int i = 0; i < num_futures; ++i) {
        updates.unite(futures.at(i).result());
    }

    return updates;
}


QHash<QString, QString> ImportHTML::LoadMediaFiles(const QStringList & file_paths)
{
    QHash<QString, QString> updates;
    QDir folder(QFileInfo(m_FullFilePath).absoluteDir());
    QStringList current_filenames = m_Book->GetFolderKeeper()->GetAllFilenames();
    // Load the media files (images, video, audio) into the book and
    // update all references with new urls
    foreach(QString file_path, file_paths) {
        try {
            QString filename = QFileInfo(file_path).fileName();
            QString fullfilepath  = QFileInfo(folder, file_path).absoluteFilePath();
            QString newpath;

            if (m_IgnoreDuplicates && current_filenames.contains(filename)) {
                newpath = "../" + m_Book->GetFolderKeeper()->GetResourceByFilename(filename)->GetRelativePathToOEBPS();
            } else {
                Resource * resource = m_Book->GetFolderKeeper()->AddContentFileToFolder(fullfilepath);
                newpath = "../" + resource->GetRelativePathToOEBPS();
            }

            updates[ fullfilepath ] = newpath;
        } catch (FileDoesNotExist) {
            // Do nothing. If the referenced file does not exist,
            // well then we don't load it.
            // TODO: log this.
        }
    }
    return updates;
}


QHash<QString, QString> ImportHTML::LoadStyleFiles(const QStringList & file_paths )
{
    QHash<QString, QString> updates;
    QDir folder(QFileInfo(m_FullFilePath).absoluteDir());
    QStringList current_filenames = m_Book->GetFolderKeeper()->GetAllFilenames();
    foreach(QString file_path, file_paths) {
        try {
            QString filename = QFileInfo(file_path).fileName();
            QString fullfilepath  = QFileInfo(folder, file_path).absoluteFilePath();
            QString newpath;

            if (m_IgnoreDuplicates && current_filenames.contains(filename)) {
                newpath = "../" + m_Book->GetFolderKeeper()->GetResourceByFilename(filename)->GetRelativePathToOEBPS();
            } else {
                Resource * resource = m_Book->GetFolderKeeper()->AddContentFileToFolder(fullfilepath);
                newpath = "../" + resource->GetRelativePathToOEBPS();
            }

            updates[ fullfilepath ] = newpath;
        } catch (FileDoesNotExist) {
            // Do nothing. If the referenced file does not exist,
            // well then we don't load it.
            // TODO: log this.
        }
    }

    return updates;
}
