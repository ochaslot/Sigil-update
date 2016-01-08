/************************************************************************
**
**  Copyright (C) 2012 John Schember <john@nachtimwald.com>
**  Copyright (C) 2012 Dave Heiland
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

#include <QtWidgets/QLineEdit>

#include "Dialogs/OpenWithName.h"

OpenWithName::OpenWithName(QString filename, QWidget *parent)
    :
    QDialog(parent),
    m_Filename(filename),
    m_MenuName(filename)
{
    ui.setupUi(this);
    connectSignalsSlots();
    ui.Filename->setText(m_Filename);
    ui.MenuName->setText(m_Filename);
}

void OpenWithName::SetName()
{
    m_MenuName = ui.MenuName->text();
}

QString OpenWithName::GetName()
{
    return m_MenuName;
}

void OpenWithName::connectSignalsSlots()
{
    connect(this,         SIGNAL(accepted()),
            this,         SLOT(SetName()));
}

