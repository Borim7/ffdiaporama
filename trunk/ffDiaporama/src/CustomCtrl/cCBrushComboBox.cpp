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

#include "cCBrushComboBox.h"

#define MAXBRUSHPATTERN  12

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cCBrushComboBoxItem::cCBrushComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBoxItem::cCBrushComboBoxItem");
}

//========================================================================================================================
void cCBrushComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBoxItem::paint");
    if ((!ComboBox)||(!ComboBox->Brush)) return;
    int ColorNum=index.row()*4+index.column();
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition Brush(ComboBox->Brush->ApplicationConfig,/*&BackgroundList*/NULL);
        Brush.ColorD     =ComboBox->Brush->ColorD;
        Brush.BrushType  =BRUSHTYPE_PATTERN;
        Brush.PatternType=ColorNum;
        painter->fillRect(option.rect,Brush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=Brush.GetBrush(option.rect,true,0,NULL,1,NULL);
        if (BR) {
            painter->setBrush(*BR);
            delete BR;
        }
        painter->drawRect(option.rect);
    } else {
        painter->fillRect(option.rect,Qt::white);
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(Qt::blue));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()-1,option.rect.height()-1);
        painter->drawRect(option.rect.x()+1,option.rect.y()+1,option.rect.width()-1-2,option.rect.height()-1-2);
        painter->setPen(QPen(Qt::black));
        painter->drawRect(option.rect.x()+2,option.rect.y()+2,option.rect.width()-1-4,option.rect.height()-1-4);
    }
}

//========================================================================================================================

QSize cCBrushComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBoxItem::sizeHint");
    return QSize(32,32);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cCBrushComboBox::cCBrushComboBox(QWidget *parent):QComboBox(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::cCBrushComboBox");
    STOPMAJ=false;
    Brush=NULL;
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,32);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<MAXBRUSHPATTERN) {
        if ((i/4)>=Table->rowCount()) {
            addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,32);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
    this->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->view()->setFixedSize(4*32,3*32);
    connect(Table,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//========================================================================================================================

void cCBrushComboBox::SetCurrentBrush(cBrushDefinition *TheBrush) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::SetCurrentBrush");
    if (STOPMAJ) return;
    Brush=TheBrush;
    ((QTableWidget *)view())->setCurrentCell(Brush->PatternType/4,Brush->PatternType-(Brush->PatternType/4)*4);
    setCurrentIndex(Brush->PatternType/4);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition *cCBrushComboBox::GetCurrentBrush() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::GetCurrentBrush");
    Brush->BrushType  =BRUSHTYPE_PATTERN;
    Brush->PatternType=currentIndex()*4+((QTableWidget *)view())->currentColumn();
    MakeIcons();
    return Brush;
}

//========================================================================================================================

void cCBrushComboBox::MakeIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::MakeIcons");
    if (!Brush) return;
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*4+CurrentCol;
    //setIconSize(QSize(width()-16,16));
    setIconSize(QSize(32,12));
    QPixmap  Image(iconSize());
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition TheBrush(Brush->ApplicationConfig,/*&BackgroundList*/NULL);
        TheBrush.ColorD=Brush->ColorD;
        TheBrush.BrushType  =BRUSHTYPE_PATTERN;
        TheBrush.PatternType=ColorNum;
        Painter.fillRect(QRectF(0,0,iconSize().width(),iconSize().height()),TheBrush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,iconSize().width(),iconSize().height()),true,0,NULL,1,NULL);
        if (BR) {
            Painter.setBrush(*BR);
            delete BR;
        }
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,iconSize().width(),iconSize().height()));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//========================================================================================================================

void cCBrushComboBox::s_ItemSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCBrushComboBox::s_ItemSelectionChanged");
    STOPMAJ=true;
    setCurrentIndex(((QTableWidget *)view())->currentRow());
    MakeIcons();
    emit currentIndexChanged(((QTableWidget *)view())->currentRow()*4+((QTableWidget *)view())->currentColumn());
    STOPMAJ=false;
}
