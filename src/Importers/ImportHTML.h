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

#pragma once
#ifndef IMPORTHTML_H
#define IMPORTHTML_H

#include "Misc/GumboInterface.h"
#include "BookManipulation/XhtmlDoc.h"
#include "Importers/Importer.h"

class HTMLResource;
class CSSResource;
class QDomDocument;

class ImportHTML : public Importer
{

public:

    // Constructor;
    // The parameter is the file to be imported
    ImportHTML(const QString &fullfilepath);

    // Needed so that we can use an existing Book
    // in which to load HTML files (and their dependencies).
    void SetBook(QSharedPointer<Book> book, bool ignore_duplicates);

    virtual XhtmlDoc::WellFormedError CheckValidToLoad();

    // Reads and parses the file
    // and returns the created Book.
    virtual QSharedPointer<Book> GetBook();

private:

    // Loads the source code into the Book
    QString LoadSource();

    // Searches for meta information in the HTML file
    // and tries to convert it to Dublin Core
    void LoadMetadata(const QString &source);

    HTMLResource *CreateHTMLResource();

    void UpdateFiles(HTMLResource *html_resource,
                     QString &source,
                     const QHash<QString, QString> &updates);

    // Loads the referenced files into the main folder of the book;
    // as the files get a new name, the references are updated
    QHash<QString, QString> LoadFolderStructure(const QString & source);

    // Returns a hash with keys being old references (URLs) to resources,
    // and values being the new references to those resources.
    QHash<QString, QString> LoadMediaFiles(const QStringList & file_paths);

    QHash<QString, QString> LoadStyleFiles(const QStringList & file_paths);


    ///////////////////////////////
    // PRIVATE MEMBER VARIABLES
    ///////////////////////////////

    bool m_IgnoreDuplicates;

    QString m_CachedSource;
   
    QString m_EpubVersion;
};

#endif // IMPORTHTML_H

