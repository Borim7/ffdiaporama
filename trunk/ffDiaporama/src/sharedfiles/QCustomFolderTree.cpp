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

#include <QtCore>
#include <QtDebug>
#include <QtGui/QApplication>
#include <QFileInfoList>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QProcess>

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <QSettings>
    #include <QPixmapCache>
#endif

#include "cBaseApplicationConfig.h"
#include "QCustomFolderTree.h"

//#define DEBUGMODE

#define TAG "<to expand>"

//====================================================================================================================

QCustomFolderTree::QCustomFolderTree(QWidget *parent):QTreeWidget(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::QCustomFolderTree";
    #endif
    ShowHidden  =false;
    ShowMntDrive=false;
    connect(this,SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(s_itemExpanded(QTreeWidgetItem *)));
}

//====================================================================================================================

void QCustomFolderTree::InitDrives(cDriveList *TheDriveList) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::InitDrives";
    #endif
    DriveList=TheDriveList;
    foreach(cDriveDesc HDD,DriveList->List)
        if ((!HDD.Path.startsWith("/mnt/"))||(ShowMntDrive))
            addTopLevelItem(CreateItem(HDD.Label,HDD.Path,HDD.IconDrive));
}

//====================================================================================================================
// Private utility function to be use to know if a folder have child (depends on ShowHidden property)
//      FilePath : Path to check

bool QCustomFolderTree::IsFolderHaveChild(QString FilePath) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QTreeWidgetItem::IsFolderHaveChild";
    #endif
    QFileInfoList List=QDir(FilePath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
    return List.count()>0;
}

//====================================================================================================================
// Private utility function to be use to know if drive's folder is readonly (CD/DVD ROM)
//      FilePath : Path to check

bool QCustomFolderTree::IsReadOnlyDrive(QString Folder) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QTreeWidgetItem::IsReadOnlyDrive";
    #endif
    bool IsReadOnly=false;
    for (int i=0;i<DriveList->List.count();i++) if (Folder.startsWith(DriveList->List[i].Path)) {
        IsReadOnly=DriveList->List[i].IsReadOnly;
        break;
    }
    return IsReadOnly;
}

//====================================================================================================================
// Private utility function to create a new QTreeWidgetItem
//      Text     : Text to be shown
//      Icon     : Icon for the Item
//      FilePath : Path to get Icon

QTreeWidgetItem *QCustomFolderTree::CreateItem(QString Text,QString FilePath,QIcon Icon) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::CreateItem";
    #endif

    QTreeWidgetItem *Current=new QTreeWidgetItem();
    Current->setText(0,Text);
    Current->setIcon(0,Icon);
    if (IsFolderHaveChild(FilePath)) {
        QTreeWidgetItem *SubItem=new QTreeWidgetItem();
        SubItem->setText(0,TAG);                            // add a tag to sub item
        Current->addChild(SubItem);
    }
    return Current;
}

//====================================================================================================================
// Public utility function to get Path from an Item
//      Item     : Item to get Path from
//      TreeMode : if true, don't make alias interpretation

QString QCustomFolderTree::GetFolderPath(const QTreeWidgetItem *Item,bool TreeMode) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetFolderPath";
    #endif

    if (!Item) return "";

    QString FilePath;

    // Parse Item to Compose path
    while (Item!=NULL) {
        // if root item
        if (!Item->parent()) {
            QString RootStr=Item->text(0);
            // For windows we display drive like D:\[Label], then cut after [ if exist
            if (Item->text(0).indexOf("[")!=-1) RootStr=Item->text(0).left(Item->text(0).indexOf("["));
            if (!TreeMode) {
                // Search if text is a registered alias, then replace text with path
                for (int i=0;i<DriveList->List.count();i++) if (DriveList->List[i].Label==RootStr) {
                    if (RootStr!=QApplication::translate("QCustomFolderTree","Personal folder")) RootStr=DriveList->List[i].Path; else
                        #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                            RootStr="~";
                        #else
                            RootStr="%HOMEDRIVE%%HOMEPATH%";
                        #endif
                }
            }
            if (!RootStr.endsWith(QDir::separator())) RootStr=RootStr+QDir::separator();
            FilePath=RootStr+FilePath;

            Item=NULL;
        } else {
            FilePath=Item->text(0)+((Item->text(0)!=QDir().separator())&&(FilePath!=QString(""))?QString(QDir().separator()):QString(""))+FilePath;
            Item=Item->parent();
        }
    }
    return FilePath;
}

//====================================================================================================================
// Public utility function to get Path from from the current selected Item

QString QCustomFolderTree::GetCurrentFolderPath() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::GetCurrentFolderPath";
    #endif

    return GetFolderPath(currentItem(),false);
}

//====================================================================================================================
// we use this signal function instead of overloaded itemExpanded function because we need to modify item and
// overloaded function use const !

void QCustomFolderTree::s_itemExpanded(QTreeWidgetItem *item) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::expandItem";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Expand node if necessary
    if ((item->childCount()==1)&&(item->child(0)->text(0)==TAG)) {

        setUpdatesEnabled(false);

        QTreeWidgetItem     *CurItem=item;
        QString             Folder=GetFolderPath(CurItem,false);
        QFileInfoList       Directorys;
        int                 i,k;

        // remove tag to sub item
        QTreeWidgetItem *SubItem=CurItem->child(0);
        CurItem->removeChild(SubItem);
        delete SubItem;

        Directorys=QDir(Folder).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
        for (k=0;k<Directorys.count();k++)
            if (Directorys[k].isDir())
                CurItem->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),DriveList->GetFolderIcon(Directorys[k].absoluteFilePath())));

        // Unselect previous selected item and then select new one
        QList<QTreeWidgetItem *>    List  =selectedItems();
        bool                        IsFind=false;

        for (i=0;i<List.count();i++)
            if (List[i]==CurItem) IsFind=true; else List[i]->setSelected(false);

        if (!IsFind) setCurrentItem(CurItem);

        setUpdatesEnabled(true);

    }

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

cDriveDesc *QCustomFolderTree::SearchRealDrive(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SearchRealDrive";
    #endif

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(1);
    #elif defined(Q_OS_WIN)
        Path.replace("%HOMEDRIVE%%HOMEPATH%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path.replace("%USERPROFILE%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path=AdjustDirForOS(Path);
    #endif

    if (Path.endsWith(QDir::separator()))  Path=Path=Path.left(Path.length()-1);    // Remove endest separator
    if (QDir(Path).canonicalPath()!="")    Path=QDir(Path).canonicalPath();         // Convert path to physical path
    Path=AdjustDirForOS(Path);
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();             // Add separtor to find drive in our list
    for (int i=0;i<DriveList->List.count();i++)
        if ((DriveList->List[i].Path!=QString("/"))&&(Path.startsWith(DriveList->List[i].Path)))
            return &DriveList->List[i];
    for (int i=0;i<DriveList->List.count();i++) if (DriveList->List[i].Path==QString("/"))
        return &DriveList->List[i];
    return NULL;
}

//====================================================================================================================

QString QCustomFolderTree::RealPathToTreePath(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::RealPathToTreePath";
    #endif

    for (int i=0;i<DriveList->List.count();i++) if ((DriveList->List[i].Path!=QString("/"))&&(Path.startsWith(DriveList->List[i].Path))) {
        Path=DriveList->List[i].Label+Path.mid(DriveList->List[i].Path.length()-1);
        break;
    }
    return Path;
}

//====================================================================================================================
QString RemoveLabel(QString Path) {
    #if defined(Q_OS_WIN)
        if (Path.indexOf("[")>0) Path=Path.left(Path.indexOf("["));
    #endif
    return Path;
}

void QCustomFolderTree::SetSelectItemByPath(QString Path) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SetSelectedItemByPath";
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setUpdatesEnabled(false);

    QFileInfoList       Directorys;
    int                 i,j,k,NbrChild;
    QStringList         Folders;
    QString             CurrentFolder;
    QTreeWidgetItem     *Current=NULL;

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (Path.startsWith("~")) Path=QApplication::translate("QCustomFolderTree","Personal folder")+Path.mid(1);
    #elif defined(Q_OS_WIN)
        Path.replace("%HOMEDRIVE%%HOMEPATH%",QApplication::translate("QCustomFolderTree","Personal folder"),Qt::CaseInsensitive);
        Path.replace("%USERPROFILE%",QApplication::translate("QCustomFolderTree","Personal folder"),Qt::CaseInsensitive);
    #endif

    // Create a list with each part of the wanted Path
    while (Path.contains(QDir::separator())) {
        Folders.append(Path.mid(0,Path.indexOf(QDir::separator())));
        Path=Path.mid(Path.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    Folders.append(Path);

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if ((Folders.count()>=1)&&(Folders[0]=="")) Folders[0]=QApplication::translate("QCustomFolderTree","System files");
    #endif

    // find node in the tree and expand it if it was not previously expanded
    i=0;
    Current=topLevelItem(0);

    while ((Current!=NULL)&&(i<Folders.count())) {
        if (i==0) {
            // Search in topitemlist : Note : Search in reverse order to give preference to drive instead of /mnt/drive or /media/drive
            j=topLevelItemCount()-1;
            while ((j>=0)&&(RemoveLabel(topLevelItem(j)->text(0))!=Folders[i])&&(RemoveLabel(topLevelItem(j)->text(0))!=Folders[i]+QDir::separator())) j--;
            if ((j>=0)&&((RemoveLabel(topLevelItem(j)->text(0))==Folders[i])||(RemoveLabel(topLevelItem(j)->text(0))==Folders[i]+QDir::separator())))
                Current=topLevelItem(j); else Current=NULL;
        } else {
            j=0;
            // Search in current item child list
            while ((j<Current->childCount())&&(Current->child(j)->text(0)!=Folders[i])) j++;
            if ((j<Current->childCount())&&(Current->child(j)->text(0)==Folders[i])) Current=Current->child(j);
                else Current=NULL;
        }
        if (Current) {
            CurrentFolder   =GetFolderPath(Current,false);
            NbrChild        =Current->childCount();
            if ((NbrChild==1)&&(Current->child(0)->text(0)==TAG)) { // If not initialize
                // remove tag to sub item
                QTreeWidgetItem *SubItem=Current->child(0);
                Current->removeChild(SubItem);
                delete SubItem;
                QString RealPath=CurrentFolder;
                #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                if (RealPath.startsWith("~")) RealPath=QDir::homePath()+RealPath.mid(1);
                #endif
                Directorys=QDir(RealPath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
                for (k=0;k<Directorys.count();k++) Current->addChild(CreateItem(Directorys[k].fileName(),Directorys[k].absoluteFilePath(),DriveList->GetFolderIcon(Directorys[k].absoluteFilePath())));
            }
            Current->setExpanded(true);
        } else Current=NULL; // Error ????
        i++;
    }

    // Unselect previous selected item and then select new one
    if (Current) {
        QList<QTreeWidgetItem *>    List=selectedItems();
        bool                        IsFind=false;
        for (i=0;i<List.count();i++) if (List[i]==Current) IsFind=true; else List[i]->setSelected(false);
        if (!IsFind) setCurrentItem(Current);
    }
    resizeColumnToContents(0);
    setUpdatesEnabled(true);

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

class cTreeItemDescriptor {
public:
    QTreeWidgetItem *Item;
    bool            IsFound;

    cTreeItemDescriptor(QTreeWidgetItem *TheItem) {
        Item=TheItem;
        IsFound=false;
    }
};

void QCustomFolderTree::RefreshItemByPath(QString Path,bool RefreshAll,int Level) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::SetSelectedItemByPath";
    #endif

    QString RealPath=Path;
    int     i,j;

    // Construct RealPath
    for (i=0;i<DriveList->List.count();i++) if (RealPath.startsWith(DriveList->List[i].Label)) {
        if (RealPath.startsWith(QDir::separator())) RealPath=DriveList->List[i].Path+RealPath.mid(DriveList->List[i].Label.length()+1); else {
            RealPath=DriveList->List[i].Path+RealPath.mid(DriveList->List[i].Label.length());
            if (RealPath.endsWith("//")) RealPath=RealPath.left(RealPath.length()-1);
            #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
            if (RealPath.startsWith("//")) RealPath=RealPath.mid(1);
            #endif
        }
        break;
    }
    if ((Level==0)&&(IsReadOnlyDrive(RealPath))) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString             PartPath=Path;
    QStringList         Folders;
    QTreeWidgetItem     *Current=NULL;
    QTreeWidgetItem     *SubItem=NULL;

    // Adjust Path
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Path.replace("~",QApplication::translate("QCustomFolderTree","Personal folder"));
    #endif

    // Create a list with each part of the wanted Path
    while (PartPath.contains(QDir::separator())) {
        Folders.append(PartPath.mid(0,PartPath.indexOf(QDir::separator())));
        PartPath=PartPath.mid(PartPath.indexOf(QDir::separator())+QString(QDir::separator()).length());
    }
    if (PartPath!="") Folders.append(PartPath);

    // Now we can search corresponding item in the tree
    i=0;
    while ((i<this->topLevelItemCount())&&(topLevelItem(i)->text(0)!=Folders[0])) i++;
    if ((i<this->topLevelItemCount())&&(topLevelItem(i)->text(0)==Folders[0])) {
        // We have found the toplevel, now down the tree
        Current=topLevelItem(i);
        j=1;
        while ((Current)&&(j<Folders.count())) {
            i=0;
            while ((i<Current->childCount())&&(Current->child(i)->text(0)!=Folders[j])) i++;
            if ((i<Current->childCount())&&(Current->child(i)->text(0)==Folders[j])) {
                Current=Current->child(i);
                j++;
            } else Current=NULL;
        }
    }

    // if item found
    if (Current) {
        if (Level==0) setUpdatesEnabled(false);

        // Set icon for this current item
        Current->setIcon(0,DriveList->GetFolderIcon(RealPath));

        // Check if havechild status have change
        if (IsFolderHaveChild(RealPath)) {
            // if folder now have child
            if (Current->childCount()==0) {
                QTreeWidgetItem *SubItem=new QTreeWidgetItem();
                SubItem->setText(0,TAG);                            // add a tag to sub item
                Current->addChild(SubItem);
            }
        } else {
            // if folder no longer have child
            while (Current->childCount()>0) {
                SubItem=Current->child(0);
                DeleteChildItem(SubItem);
                Current->removeChild(SubItem);
                delete SubItem;
            }
        }

        // Construct a list with actual known folders
        QList<cTreeItemDescriptor>  CurrentList;
        for (i=0;i<Current->childCount();i++) CurrentList.append(cTreeItemDescriptor(Current->child(i)));

        // Construct a second list for real folders
        QFileInfoList   Directorys=QDir(RealPath).entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|(ShowHidden?QDir::Hidden:QDir::Dirs));
        QStringList     FolderToAdd;
        bool            isFound;
        for (i=0;i<Directorys.count();i++) {
            isFound=false;
            // for each item search if it is present in current list
            for (j=0;j<CurrentList.count();j++) if (Directorys[i].fileName()==CurrentList[j].Item->text(0)) {
                CurrentList[j].IsFound=true;
                isFound=true;
                break;
            }
            // if folder not found add it to to FolderToAdd List
            if (!isFound) FolderToAdd.append(Directorys[i].fileName());
        }

        // Now we have 2 lists to work with
        for (i=0;i<CurrentList.count();i++) {
            // update all previously existing folder
            if (CurrentList[i].IsFound) {
                QString ChildPath=Path;
                if (!ChildPath.endsWith(QDir::separator())) ChildPath=ChildPath+QDir::separator();
                ChildPath=ChildPath+CurrentList[i].Item->text(0);
                if (RefreshAll) RefreshItemByPath(ChildPath,Level+1);
            } else {
                // remove existing folder which no longer exist
                SubItem=CurrentList[i].Item;
                DeleteChildItem(SubItem);
                Current->removeChild(SubItem);
                delete SubItem;
            }
        }
        // add new folder
        QString ChildPath=QDir().absoluteFilePath(RealPath);
        if (!ChildPath.endsWith(QDir::separator())) ChildPath=ChildPath+QDir::separator();
        for (i=0;i<FolderToAdd.count();i++) {
            int ItemBefore=0;
            while ((ItemBefore<Current->childCount())&&(FolderToAdd[i]>=Current->child(ItemBefore)->text(0))) ItemBefore++;
            Current->insertChild(ItemBefore,CreateItem(FolderToAdd[i],ChildPath+FolderToAdd[i],DriveList->GetFolderIcon(ChildPath+FolderToAdd[i])));
        }

        if (Level==0) setUpdatesEnabled(true);
    }

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void QCustomFolderTree::DeleteChildItem(QTreeWidgetItem *Item) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::DeleteChildItem";
    #endif
    while (Item->childCount()!=0) {
        QTreeWidgetItem *SubItem=Item->child(0);
        DeleteChildItem(SubItem);
        Item->removeChild(SubItem);
        delete SubItem;
    }
}

//====================================================================================================================

void QCustomFolderTree::RefreshDriveList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomFolderTree::RefreshDriveList";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    DriveList->UpdateDriveList();
    int i=0;
    while (i<DriveList->List.count()) {
        if (DriveList->List[i].Flag==0) {
            // Drive no longer exist
            int j=0;
            while ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)!=DriveList->List[i].Label)) j++;
            if ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)==DriveList->List[i].Label)) {
                // We have found the item in the tree
                QTreeWidgetItem *Item=takeTopLevelItem(j);
                DeleteChildItem(Item);                      // Delete item in tree
                delete Item;
            }
            DriveList->List.removeAt(i);                    // Delete from drive list
        } else if (DriveList->List[i].Flag==1) {
            // Drive previously exist
            int j=0;
            while ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)!=DriveList->List[i].Label)) j++;
            if ((j<topLevelItemCount())&&(topLevelItem(j)->text(0)==DriveList->List[i].Label)) {
                // if drive is not a /mnt/ drive or if we continu to display them, then update it
                if ((!DriveList->List[i].Path.startsWith("/mnt/"))||(ShowMntDrive)) RefreshItemByPath(DriveList->List[i].Label,true);
                else { // if ShowMntDrive have changed, we have to delete it now
                    QTreeWidgetItem *Item=takeTopLevelItem(j);
                    DeleteChildItem(Item);                      // Delete item in tree
                    delete Item;
                }
            } else {
                // ShowMntDrive have changed, we have to create it
                if ((!DriveList->List[i].Path.startsWith("/mnt/"))||(ShowMntDrive))
                    addTopLevelItem(CreateItem(DriveList->List[i].Label,DriveList->List[i].Path,DriveList->List[i].IconDrive));
            }
            i++;
        } else {
            // It's a new drive
            if ((!DriveList->List[i].Path.startsWith("/mnt/"))||(ShowMntDrive))
                addTopLevelItem(CreateItem(DriveList->List[i].Label,DriveList->List[i].Path,DriveList->List[i].IconDrive));
            i++;
        }
    }
    QApplication::restoreOverrideCursor();
}