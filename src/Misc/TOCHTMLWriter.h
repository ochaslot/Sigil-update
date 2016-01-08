/************************************************************************
**
**  Copyright (C) 2012 Dave Heiland
**  Copyright (C) 2012 John Schember <john@nachtimwald.com>
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

#ifndef TOCHTMLWRITER_H
#define TOCHTMLWRITER_H

#include "MainUI/NCXModel.h"

class QXmlStreamWriter;

/**
 * Writes the TOC into an HTML file of the EPUB publication.
 */
class TOCHTMLWriter
{
public:
    TOCHTMLWriter(NCXModel::NCXEntry ncx_root_entry);
    ~TOCHTMLWriter();

    QString WriteXML(const QString &version);

private:
    void WriteHead();
    void WriteBody();
    void WriteEntries(NCXModel::NCXEntry entry, int level = 1);

    QXmlStreamWriter *m_Writer;

    NCXModel::NCXEntry m_NCXRootEntry;
};

#endif // TOCHTMLWRITER_H
