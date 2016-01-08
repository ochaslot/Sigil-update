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

#pragma once
#ifndef INDEXEDITOR_H
#define INDEXEDITOR_H

#include <QtWidgets/QDialog>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtCore/QSharedPointer>

#include "Misc/SettingsStore.h"
#include "MiscEditors/IndexEditorModel.h"
#include "BookManipulation/Book.h"

#include "ui_IndexEditor.h"

class QPoint;

/**
 * The editor used to create and modify index entries
 */
class IndexEditor : public QDialog
{
    Q_OBJECT

public:
    IndexEditor(QWidget *parent);

    void SetBook(QSharedPointer <Book> book);
    void ForceClose();

public slots:
    QStandardItem *AddEntry(bool is_group = false, IndexEditorModel::indexEntry *index_entry = NULL, bool insert_after = true);

signals:
    void ShowStatusMessageRequest(const QString &message);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void reject();
    void showEvent(QShowEvent *event);

private slots:
    void Edit();
    void Cut();
    bool Copy();
    void Paste();
    void Delete();
    void AutoFill();
    void Open();
    void Reload();
    void SaveAs();
    void SelectAll();

    bool Save();

    void FilterEditTextChangedSlot(const QString &text);

    void OpenContextMenu(const QPoint &point);

    void SettingsFileModelUpdated();

private:
    bool MaybeSaveDialogSaysProceed(bool is_forced);
    void SetupIndexEditorTree();

    int SelectedRowsCount();

    QList<IndexEditorModel::indexEntry *> GetSelectedEntries();

    QList<QStandardItem *> GetSelectedItems();

    bool SaveData(QList<IndexEditorModel::indexEntry *> entries = QList<IndexEditorModel::indexEntry *>() , QString filename = QString());

    void ReadSettings();
    void WriteSettings();

    void CreateContextMenuActions();
    void SetupContextMenu(const QPoint &point);

    void ConnectSignalsSlots();

    QAction *m_AddEntry;
    QAction *m_Edit;
    QAction *m_Cut;
    QAction *m_Copy;
    QAction *m_Paste;
    QAction *m_Delete;
    QAction *m_AutoFill;
    QAction *m_Open;
    QAction *m_Reload;
    QAction *m_SaveAs;
    QAction *m_SelectAll;

    IndexEditorModel *m_IndexEditorModel;

    QSharedPointer<Book> m_Book;

    QString m_LastFolderOpen;

    QMenu *m_ContextMenu;

    QList<IndexEditorModel::indexEntry *> m_SavedIndexEntries;

    Ui::IndexEditor ui;
};

#endif // INDEXEDITOR_H
