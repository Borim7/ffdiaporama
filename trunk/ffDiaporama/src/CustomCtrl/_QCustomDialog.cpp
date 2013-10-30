/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include "../ffDiaporama/HelpPopup/HelpPopup.h"

//====================================================================================================================

QString CustomInputDialog(QWidget *parent,const QString &title,const QString &label,int mode,const QString &text,bool *ok,Qt::InputMethodHints inputMethodHints) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:CustomInputDialog");

    Qt::WindowFlags Flags=(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint);

    QInputDialog dialog(parent,Flags);
    int x=QCursor::pos().x()-dialog.width();   if (x<0) x=0;
    int y=QCursor::pos().y()-dialog.height();  if (y<0) y=0;
    dialog.move(x,y);
    dialog.setWindowTitle(title);
    dialog.setLabelText(label);
    dialog.setTextValue(text);
    dialog.setTextEchoMode((QLineEdit::EchoMode)mode);
    dialog.setInputMethodHints(inputMethodHints);
    dialog.setWindowModality(Qt::ApplicationModal);

    int ret = dialog.exec();
    if (ok) *ok = !!ret;
    if (ret) return dialog.textValue();
        else return QString();
}

//====================================================================================================================

int CustomMessageBox(QWidget *parent,QMessageBox::Icon icon,const QString& title,const QString& text,QMessageBox::StandardButtons buttons,QMessageBox::StandardButton defaultButton) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:CustomMessageBox");

    Qt::WindowFlags Flags=(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint);

    QMessageBox         msgBox(icon,title,text,QMessageBox::NoButton,parent,Flags);
    QDialogButtonBox    *buttonBox = msgBox.findChild<QDialogButtonBox*>();

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb) continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton()) continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    msgBox.setWindowModality(Qt::ApplicationModal);
    if (msgBox.exec() == -1) return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

//====================================================================================================================

QCustomDialog::QCustomDialog(cBaseApplicationConfig *BaseApplicationConfig,QWidget *parent):QDialog(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::QCustomDialog");

    this->BaseApplicationConfig =BaseApplicationConfig;
    TypeWindowState             =TypeWindowState_simple;
    Splitter                    =NULL;
    Undo                        =NULL;
    OkBt                        =NULL;
    CancelBt                    =NULL;
    UndoBt                      =NULL;
    HelpBt                      =NULL;

    #ifndef Q_OS_WIN
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
    #endif
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setWindowModality(Qt::WindowModal);
}

//====================================================================================================================

QCustomDialog::~QCustomDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::~QCustomDialog");

    if (!HelpFile.isEmpty() && BaseApplicationConfig->WikiFollowInterface) BaseApplicationConfig->PopupHelp->RestorePreviousHelpFile();

    if (Undo) {
        delete Undo;
        Undo=NULL;
    }
}

//====================================================================================================================
// Initialise dialog

void QCustomDialog::InitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::InitDialog");

    // Define handler for standard buttons
    if (OkBt) {
        OkBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
        connect(OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    }
    if (CancelBt) {
        CancelBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
        connect(CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    }
    if (UndoBt) {
        UndoBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogResetButton));
        connect(UndoBt,SIGNAL(clicked()),this,SLOT(DoPartialUndo()));
    }

    if (HelpBt) {
        HelpBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton));
        connect(HelpBt,SIGNAL(clicked()),this,SLOT(help()));
    }

    // Restore window size and position
    RestoreWindowState();

    // Prepare undo
    PrepareGlobalUndo();

    // Initialise dialog
    DoInitDialog();

    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);

    toolTipTowhatsThis(this);
    if (!HelpFile.isEmpty() && BaseApplicationConfig->WikiFollowInterface) {
        BaseApplicationConfig->PopupHelp->SaveLatestHelpFile();
        BaseApplicationConfig->PopupHelp->OpenHelp(HelpFile,false);
    }
}

//====================================================================================================================
// Function use to duplicate toolTip properties of all child object to whatsThis properties

void QCustomDialog::toolTipTowhatsThis(QObject *StartObj) {
    if (StartObj->property("toolTip").toString()!="") {
        StartObj->setProperty("whatsThis",StartObj->property("toolTip").toString());
        if (BaseApplicationConfig->DisableTooltips) StartObj->setProperty("toolTip","");
    }
    for (int i=0;i<StartObj->children().count();i++) toolTipTowhatsThis(StartObj->children().at(i));
}

//====================================================================================================================

void QCustomDialog::accept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::accept");

    SaveWindowState();  // Save Window size and position
    if (DoAccept())     // call overloaded function
        done(0);        // Close dialog
}

//====================================================================================================================

void QCustomDialog::reject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::accept");

    SaveWindowState();  // Save Window size and position
    DoGlobalUndo();     // Undo change
    DoRejet();          // call overloaded function
    done(1);            // Close dialog
}

//====================================================================================================================

void QCustomDialog::help() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::help");
    BaseApplicationConfig->PopupHelp->OpenHelp(HelpFile,true);
}

//====================================================================================================================
// Save Window size and position

void QCustomDialog::SaveWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::SaveWindowState");

    if (BaseApplicationConfig->RestoreWindow) {
        QDomDocument    domDocument;
        QDomElement     root=domDocument.createElement("WindowState");
        domDocument.appendChild(root);

        if (TypeWindowState==TypeWindowState_simple) {
            cSaveWindowPosition DlgWSP(objectName(),BaseApplicationConfig->RestoreWindow,false);
            DlgWSP.SaveWindowState(this);
            DlgWSP.SaveToXML(root);
            BaseApplicationConfig->SettingsTable->SetIntAndTextValue(objectName(),TypeWindowState,domDocument.toString());
        } else if (TypeWindowState==TypeWindowState_withsplitterpos) {
            cSaveWinWithSplitterPos DlgWSP(objectName(),BaseApplicationConfig->RestoreWindow,false);
            DlgWSP.SaveWindowState(this,Splitter);
            DlgWSP.SaveToXML(root);
            BaseApplicationConfig->SettingsTable->SetIntAndTextValue(objectName(),TypeWindowState,domDocument.toString());
        }
    }
}

//====================================================================================================================
// Restore window size and position

void QCustomDialog::RestoreWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::RestoreWindowState");

    QString   WindowStateString;
    if (BaseApplicationConfig->SettingsTable->GetIntAndTextValue(objectName(),&TypeWindowState,&WindowStateString)) {
        QDomDocument    domDocument;
        QString         errorStr;
        int             errorLine,errorColumn;
        if (!domDocument.setContent(WindowStateString,true,&errorStr,&errorLine,&errorColumn)) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error reading window state of %1 from home user database","Error message").arg(objectName()));
        } else {
            if (TypeWindowState==TypeWindowState_simple) {
                cSaveWindowPosition DlgWSP(objectName(),BaseApplicationConfig->RestoreWindow,false);
                DlgWSP.LoadFromXML(domDocument.documentElement());
                if (DlgWSP.IsInit) DlgWSP.ApplyToWindow(this);
            } else if (TypeWindowState==TypeWindowState_withsplitterpos) {
                cSaveWinWithSplitterPos DlgWSP(objectName(),BaseApplicationConfig->RestoreWindow,false);
                DlgWSP.LoadFromXML(domDocument.documentElement());
                if (DlgWSP.IsInit) DlgWSP.ApplyToWindow(this,Splitter);
            }
        }
    }
}

//====================================================================================================================
// utility function to init a table widget

void QCustomDialog::DoInitTableWidget(QTableWidget *Table,QString TableColumns) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoInitTableWidget");

    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    Table->horizontalHeader()->show();
    Table->horizontalHeader()->setStretchLastSection(false);
    Table->horizontalHeader()->setSortIndicatorShown(false);
    Table->horizontalHeader()->setCascadingSectionResizes(false);
    Table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    Table->verticalHeader()->hide();
    Table->verticalHeader()->setStretchLastSection(false);
    Table->verticalHeader()->setSortIndicatorShown(false);
    #if QT_VERSION >= 0x050000
    Table->horizontalHeader()->setSectionsClickable(false);
    Table->horizontalHeader()->setSectionsMovable(false);
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    Table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    #else
    Table->horizontalHeader()->setClickable(false);
    Table->horizontalHeader()->setMovable(false);
    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);          //Fixed because ResizeToContents will be done after table filling
    Table->verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling
    #endif
    Table->setShowGrid(true);                  // Ensure grid display
    Table->setWordWrap(false);                 // Ensure no word wrap
    Table->setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    Table->setColumnCount(TableColumns.split(";").count());
    Table->setHorizontalHeaderLabels(TableColumns.split(";"));
}

//====================================================================================================================
// utility function to create a QTableWidgetItem

QTableWidgetItem *QCustomDialog::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::CreateItem");

    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================
// utility function to resize columns in a table widget

void QCustomDialog::DoResizeColumnsTableWidget(QTableWidget *Table) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoResizeColumnsTableWidget");

    #if QT_VERSION >= 0x050000
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    #else
    Table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    #endif
    Table->setVisible(false);                      // To ensure all items of all columns are used to compute size
    Table->resizeColumnsToContents();              // Resize column widht
    Table->resizeRowsToContents();                 // Resize row height
    Table->setVisible(true);                       // To allow display
    #if QT_VERSION >= 0x050000
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    #else
    Table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    #endif
}

//====================================================================================================================
// utility function to add a partial undo to the list

void QCustomDialog::AppendPartialUndo(int ActionType,QWidget *FocusWindow,bool ForceAdd) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::AppendPartialUndo");
    if ((ForceAdd)||(UndoDataList.count()==0)||(ActionType!=UndoDataList.last().ActionType)) {

        QDomDocument DomDocument("UNDO");
        QDomElement  root=DomDocument.createElement("UNDO-DATA");
        PreparePartialUndo(ActionType,root);
        DomDocument.appendChild(root);

        sUndoData   Data;
        Data.ActionType =ActionType;
        Data.Data       =DomDocument.toString(1);
        Data.FocusWindow=FocusWindow;
        UndoDataList.append(Data);
        if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
    }
}

//====================================================================================================================
// utility function to undo latest undo insertion

void QCustomDialog::RemoveLastPartialUndo() {
    UndoDataList.removeLast();
    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}

//====================================================================================================================
// utility function to undo latest action

void QCustomDialog::DoPartialUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::DoPartialUndo");
    if (UndoDataList.count()==0) {
        if (Undo!=NULL) DoGlobalUndo();
    } else {
        sUndoData Data=UndoDataList.last();
        UndoDataList.removeLast();
        QDomDocument DomDocument("UNDO");
        if (DomDocument.setContent(Data.Data,true)) {
            QDomElement root=DomDocument.documentElement();
            if (root.tagName()=="UNDO-DATA") ApplyPartialUndo(Data.ActionType,root);
        }
        if (Data.FocusWindow!=NULL) Data.FocusWindow->setFocus();
    }
    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}

//====================================================================================================================
// Utility function use to set current index in a combobox base on an itemdata

void QCustomDialog::SetCBIndex(QComboBox *CB,int ItemData) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomDialog::SetCBIndex");
    for (int i=0;i<CB->count();i++) if (CB->itemData(i).toInt()==ItemData) {
        CB->setCurrentIndex(i);
        break;
    }
}
