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

#include "_QCustomDialog.h"
#include <QHeaderView>

//#define DEBUGMODE

QCustomDialog::QCustomDialog(QString HelpURL,cBaseApplicationConfig *BaseApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):QDialog(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::QCustomDialog";
    #endif

    this->HelpURL               =HelpURL;
    this->BaseApplicationConfig =BaseApplicationConfig;
    this->DlgWSP                =DlgWSP;
    Undo                        =NULL;
    OkBt                        =NULL;
    CancelBt                    =NULL;
    HelpBt                      =NULL;

    #if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
        setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
    #else
        setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
    #endif

}

//====================================================================================================================

QCustomDialog::~QCustomDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::~QCustomDialog";
    #endif
    if (Undo) {
        delete Undo;
        Undo=NULL;
    }
}

//====================================================================================================================
// Initialise dialog

void QCustomDialog::InitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::InitDialog";
    #endif

    // Define handler for standard buttons
    if (OkBt)       connect(OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    if (CancelBt)   connect(CancelBt,SIGNAL(clicked()),this,SLOT(reject()));

    if (HelpBt) {
        if (HelpURL!="") connect(HelpBt,SIGNAL(clicked()),this,SLOT(doHelp()));
            else HelpBt->setVisible(false);
    }

    // Restore window size and position
    if (DlgWSP) DlgWSP->ApplyToWindow(this);

    // Prepare undo
    PrepareGlobalUndo();

    // Initialise dialog
    DoInitDialog();
}

//====================================================================================================================

void QCustomDialog::doHelp() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::Help";
    #endif
    if (HelpURL!="") QDesktopServices::openUrl(QUrl(HelpURL.replace("<local>",BaseApplicationConfig->GetValideWEBLanguage(BaseApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void QCustomDialog::accept() {
    #ifdef QCustomDialog
    qDebug() << "IN:QCustomDialog::accept";
    #endif
    // Save Window size and position
    if (DlgWSP) DlgWSP->SaveWindowState(this);

    // call overloaded function
    DoAccept();

    // Close dialog
    done(0);
}

//====================================================================================================================

void QCustomDialog::reject() {
    #ifdef QCustomDialog
    qDebug() << "IN:QCustomDialog::accept";
    #endif
    // Save Window size and position
    if (DlgWSP) DlgWSP->SaveWindowState(this);

    // Undo change
    DoGlobalUndo();

    // call overloaded function
    DoRejet();

    // Close dialog
    done(1);
}

//====================================================================================================================
// utility function to init a table widget

void QCustomDialog::DoInitTableWidget(QTableWidget *Table,QString TableColumns) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::DoInitTableWidget";
    #endif
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    Table->horizontalHeader()->show();
    Table->horizontalHeader()->setStretchLastSection(false);
    Table->horizontalHeader()->setSortIndicatorShown(false);
    Table->horizontalHeader()->setCascadingSectionResizes(false);
    Table->horizontalHeader()->setClickable(false);
    Table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    Table->horizontalHeader()->setMovable(false);
    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    Table->verticalHeader()->hide();
    Table->verticalHeader()->setStretchLastSection(false);
    Table->verticalHeader()->setSortIndicatorShown(false);
    Table->verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    Table->setShowGrid(true);                  // Ensure grid display
    Table->setWordWrap(false);                 // Ensure no word wrap
    Table->setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    Table->setColumnCount(TableColumns.split(";").count());
    Table->setHorizontalHeaderLabels(TableColumns.split(";"));
}

//====================================================================================================================
// utility function to create a QTableWidgetItem

QTableWidgetItem *QCustomDialog::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::CreateItem";
    #endif
    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================
// utility function to resize columns in a table widget

void QCustomDialog::DoResizeColumnsTableWidget(QTableWidget *Table) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::DoResizeColumnsTableWidget";
    #endif
    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    Table->setVisible(false);                      // To ensure all items of all columns are used to compute size
    Table->resizeColumnsToContents();              // Resize column widht
    Table->resizeRowsToContents();                 // Resize row height
    Table->setVisible(true);                       // To allow display
    Table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
}