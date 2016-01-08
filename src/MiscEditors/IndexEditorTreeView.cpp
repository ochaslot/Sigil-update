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

#include <QHeaderView>

#include "MiscEditors/IndexEditorTreeView.h"

IndexEditorTreeView::IndexEditorTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setAcceptDrops(false);
    setDropIndicatorShown(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSortingEnabled(true);
    setTabKeyNavigation(true);
}

IndexEditorTreeView::~IndexEditorTreeView()
{
}

QModelIndex IndexEditorTreeView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    if (cursorAction == QAbstractItemView::MoveNext) {
        QModelIndex index = currentIndex();

        // Only the first 2 columns are editable
        if (index.column() < header()->count() - 1) {
            return model()->index(index.row(), index.column() + 1, index.parent());
        }

        if (indexBelow(index).isValid()) {
            setCurrentIndex(model()->index(index.row(), 0, index.parent()));
        }
    } else if (cursorAction == QAbstractItemView::MovePrevious) {
        QModelIndex index = currentIndex();

        if (index.column() > 0) {
            return model()->index(index.row(), index.column() - 1, index.parent());
        }

        if (indexAbove(index).isValid()) {
            setCurrentIndex(model()->index(index.row(), header()->count() - 1, index.parent()));
        }
    }

    return QTreeView::moveCursor(cursorAction, modifiers);
}
