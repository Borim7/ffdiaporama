/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2012 Dominique Levray <levray.dominique@bbox.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   ====================================================================== */

#ifndef QCUSTOMFOLDERTREE_H
#define QCUSTOMFOLDERTREE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QIcon>

// Include some common various class
#include "cDriveList.h"

class QCustomFolderTree : public QTreeWidget {
Q_OBJECT
public:
    cBaseApplicationConfig  *ApplicationConfig;
    cDriveList              *DriveList;
    explicit                QCustomFolderTree(QWidget *parent=0);

    // Public utility functions
    virtual void        InitDrives(cDriveList *TheDriveList);
    virtual QString     GetFolderPath(const QTreeWidgetItem *current,bool TreeMode);
    virtual QString     GetCurrentFolderPath();
    virtual void        SetSelectItemByPath(QString Path);
    virtual void        RefreshItemByPath(QString Path,bool RefreshAll,int Level=0);
    virtual QString     RealPathToTreePath(QString Path);
    virtual cDriveDesc  *SearchRealDrive(QString Path);
    virtual void        RefreshDriveList();

private:
    QTreeWidgetItem     *CreateItem(QString Text,QString FilePath,QIcon Icon);
    bool                IsFolderHaveChild(QString Folder);
    bool                IsReadOnlyDrive(QString Folder);
    void                DeleteChildItem(QTreeWidgetItem *Item);

private slots:
    void                s_itemExpanded(QTreeWidgetItem *item);

signals:

};

#endif // QCUSTOMFOLDERTREE_H