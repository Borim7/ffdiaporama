/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011 Dominique Levray <levray.dominique@bbox.fr>

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

#include <QCoreApplication>
#include <QtDebug>
#include <QComboBox>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QListView>
#include <QHeaderView>
#include "cdiaporama.h"
#include "fmt_filters.h"
#include "cCustomComboBox.h"

#define MAXCOLORREF             47
#define MAXBRUSHPATTERN         12
#define MAXGRADIENTORIENTATION  9
#define MAXONOFFFILTER          8
#define ICONWIDTH               64
#define ICONHEIGHT              16

//******************************************************************************************************************
// Custom Color ComboBox ITEM
//******************************************************************************************************************

QString  ColorRef[MAXCOLORREF]={
    "#ffffff","#ffffaa","#ffff66","#ffff00",    // Qt::white, very light yellow to yellow
    "#b4b400","#808000","#00ff00","#008000",    // to dark yellow, Qt::green to Qt::darkGreen
    "#ccff66","#99ff33","#66ff00","#498000",    // very light green to green
    "#338000","#205000","#ff00ff","#800080",    //to dark green Qt::magenta to Qt::darkMagenta
    "#ff0000","#ff99ff","#ff66cc","#ff3399",    // Qt::red then very light pink to dark pink
    "#ff0066","#800033","#ffcc66","#ff9933",    //very light brown to dark red
    "#ff6600","#804900","#803300","#800000",    // Qt::cyan
    "#00ffff","#66ccff","#3399ff","#0066ff",    //very light blue to dark blue
    "#0049b4","#003380","#0000ff","#000080",
    "#66ffcc","#33ffcc","#00ffcc","#00b4b4",    //very light cyan to dark cyan
    "#008080","#005050","#c0c0c0","#a0a0a4",    // Qt::lightGrayto Qt::darkGray
    "#808080","#333333","#000000"               // Qt::black
};

//========================================================================================================================

cCustomColorComboBoxItem::cCustomColorComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================

void cCustomColorComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*4+index.column();
    if (ColorNum<MAXCOLORREF) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(ColorRef[ColorNum])));
        painter->drawRect(option.rect);
    } else {
        if (ColorNum==MAXCOLORREF) {
            painter->fillRect(option.rect,QColor(ComboBox->CurrentColor));
            QImage  Img("icons/colorize.png");
            painter->drawImage(QRectF(option.rect.x()+option.rect.width()-16,option.rect.y()+option.rect.height()-16,16,16),Img);
        } else {
            painter->fillRect(option.rect,Qt::white);
        }
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

QSize cCustomColorComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

cCustomColorComboBox::cCustomColorComboBox(QWidget *parent):QComboBox(parent) {
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<=MAXCOLORREF) {    // = to add extra custom color (with icon)
        if ((i/4)>=Table->rowCount()) {
            addItem("");        // automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    CurrentColor="#FEFEFE";
    MakeIcons();
}

//========================================================================================================================

void cCustomColorComboBox::SetCurrentColor(QString Color) {
    int i=0;
    while ((i<MAXCOLORREF)&&(ColorRef[i]!=Color)) i++;
    ((QTableWidget *)view())->setCurrentCell(i/4,i-(i/4)*4);
    setCurrentIndex(i/4);
    StandardColor=(i<MAXCOLORREF);
    if (i>=MAXCOLORREF) CurrentColor=Color;
    MakeIcons();
}

//========================================================================================================================

QString cCustomColorComboBox::GetCurrentColor() {
    int i=currentIndex()*4+((QTableWidget *)view())->currentColumn();
    if ((i>=0)&&(i<MAXCOLORREF)) return ColorRef[i]; else return CurrentColor;
}

//========================================================================================================================

void cCustomColorComboBox::MakeIcons() {
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*4+CurrentCol;
    QPixmap  Image(64,16);
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXCOLORREF) Painter.fillRect(QRectF(0,0,64,16),QColor(ColorRef[ColorNum])); else {
        Painter.fillRect(QRectF(0,0,64,16),QColor(CurrentColor));
        QImage  Img("icons/colorize.png");
        Painter.drawImage(QRectF(64-16,0,16,16),Img,QRectF(0,0,Img.width(),Img.height()));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//******************************************************************************************************************
// Custom Brush ComboBox ITEM
//******************************************************************************************************************

cCustomBrushComboBoxItem::cCustomBrushComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================
void cCustomBrushComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*4+index.column();
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition Brush=ComboBox->Brush;
        Brush.BrushType  =BRUSHTYPE_PATTERN;
        Brush.PatternType=ColorNum;
        painter->fillRect(option.rect,Brush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=Brush.GetBrush(option.rect);
        painter->setBrush(*BR);
        delete BR;
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

QSize cCustomBrushComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    return QSize(24,24);
}

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

cCustomBrushComboBox::cCustomBrushComboBox(QWidget *parent):QComboBox(parent) {
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    int i=0;
    while (i<MAXBRUSHPATTERN) {
        if ((i/4)>=Table->rowCount()) {
            addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
}

//========================================================================================================================

void cCustomBrushComboBox::SetCurrentBrush(cBrushDefinition TheBrush) {
    Brush=TheBrush;
    setCurrentIndex(Brush.PatternType/4);
    ((QTableWidget *)view())->setCurrentCell(Brush.PatternType/4,Brush.PatternType-(Brush.PatternType/4)*4);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition cCustomBrushComboBox::GetCurrentBrush() {
    Brush.BrushType  =BRUSHTYPE_PATTERN;
    Brush.PatternType=currentIndex()*4+((QTableWidget *)view())->currentColumn();
    MakeIcons();
    return Brush;
}

//========================================================================================================================

void cCustomBrushComboBox::MakeIcons() {
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*4+CurrentCol;
    QPixmap  Image(64,16);
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXBRUSHPATTERN) {
        cBrushDefinition TheBrush=Brush;
        TheBrush.BrushType  =BRUSHTYPE_PATTERN;
        TheBrush.PatternType=ColorNum;
        Painter.fillRect(QRectF(0,0,64,16),TheBrush.ColorD.toLower()!="#ffffff"?Qt::white:Qt::black);
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,64,16));
        Painter.setBrush(*BR);
        delete BR;
        Painter.drawRect(QRectF(0,0,64,16));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,64,16));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//******************************************************************************************************************
// Custom Gradient Orientation ComboBox ITEM
//******************************************************************************************************************

cGradientOrientationComboBoxItem::cGradientOrientationComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================
void cGradientOrientationComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*3+index.column();
    if (ColorNum<MAXGRADIENTORIENTATION) {
        cBrushDefinition Brush=ComboBox->Brush;
        Brush.GradientOrientation=ColorNum;
        QBrush *BR=Brush.GetBrush(option.rect);
        painter->setBrush(*BR);
        delete BR;
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

QSize cGradientOrientationComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    return QSize(32,32);
}

//******************************************************************************************************************
// Custom Gradient Orientation ComboBox
//******************************************************************************************************************

cGradientOrientationComboBox::cGradientOrientationComboBox(QWidget *parent):QComboBox(parent) {
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
    while (i<MAXGRADIENTORIENTATION) {
        if ((i/3)>=Table->rowCount()) {
            addItem("");    //automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,32);
        }
        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    MakeIcons();
}

//========================================================================================================================

void cGradientOrientationComboBox::SetCurrentBrush(cBrushDefinition TheBrush) {
    Brush=TheBrush;
    setCurrentIndex(Brush.GradientOrientation/3);
    ((QTableWidget *)view())->setCurrentCell(Brush.GradientOrientation/3,Brush.GradientOrientation-(Brush.GradientOrientation/3)*3);
    MakeIcons();
}

//========================================================================================================================

cBrushDefinition cGradientOrientationComboBox::GetCurrentBrush() {
    Brush.GradientOrientation=currentIndex()*3+((QTableWidget *)view())->currentColumn();
    MakeIcons();
    return Brush;
}

//========================================================================================================================

void cGradientOrientationComboBox::MakeIcons() {
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    int ColorNum=CurrentRow*3+CurrentCol;
    QPixmap  Image(64,16);
    QPainter Painter;
    Painter.begin(&Image);
    if (ColorNum<MAXGRADIENTORIENTATION) {
        cBrushDefinition TheBrush=Brush;
        TheBrush.GradientOrientation=ColorNum;
        QBrush *BR=TheBrush.GetBrush(QRectF(0,0,64,16));
        Painter.setBrush(*BR);
        delete BR;
        Painter.drawRect(QRectF(0,0,64,16));
    } else {
        Painter.setBrush(QBrush(Qt::white));
        Painter.drawRect(QRectF(0,0,64,16));
    }
    Painter.end();
    setItemIcon(CurrentRow,QIcon(Image));
}

//******************************************************************************************************************
// Custom QAbstractItemDelegate for OnOffFilter ComboBox
//******************************************************************************************************************

cOnOffFilterComboBoxItem::cOnOffFilterComboBoxItem(QObject *parent):QStyledItemDelegate(parent) {
}

//========================================================================================================================

void cOnOffFilterComboBoxItem::paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    int ColorNum=index.row()*4+index.column();
    // Prepare OnOff filter pixmaps
    QImage              Image=ComboBox->SourceImage.copy();
    fmt_filters::image  Img(Image.bits(),Image.width(),Image.height());
    if ((ColorNum & FilterDespeckle)==FilterDespeckle)  fmt_filters::despeckle(Img);
    if ((ColorNum & FilterEqualize)==FilterEqualize)    fmt_filters::equalize(Img);
    if ((ColorNum & FilterGray)==FilterGray)            fmt_filters::gray(Img);
    painter->drawImage(option.rect,Image);
    QString Text="";
    if ((ColorNum & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
    if ((ColorNum & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
    if ((ColorNum & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
    if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
    painter->drawText(option.rect,Text);
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

QSize cOnOffFilterComboBoxItem::sizeHint(const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const {
    return QSize(ComboBox->SourceImage.width(),ComboBox->SourceImage.height());
}

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

cOnOffFilterComboBox::cOnOffFilterComboBox(QWidget *parent):QComboBox(parent) {
    QTableWidget    *Table=new QTableWidget();
    Table->horizontalHeader()->hide();
    Table->verticalHeader()->hide();
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    Table->insertColumn(Table->columnCount());  Table->setColumnWidth(Table->columnCount()-1,24);
    setModel(Table->model());
    setView(Table);
    QString Text="";
    int i=0;
    while (i<MAXONOFFFILTER) {    // = to add extra custom color (with icon)
        if ((i/4)>=Table->rowCount()) {
            addItem("");        // automaticaly do a Table->insertRow(Table->rowCount());
            Table->setRowHeight(Table->rowCount()-1,24);
        }
        Text="";
        if ((i & FilterGray)==FilterGray)            Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Gray");
        if ((i & FilterEqualize)==FilterEqualize)    Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Equalize");
        if ((i & FilterDespeckle)==FilterDespeckle)  Text=Text+(Text!=""?"+":"")+QCoreApplication::translate("wgt_QImageFilterTransform","Despeckle");
        if (Text=="") Text=QCoreApplication::translate("wgt_QImageFilterTransform","No transformation");
        Table->setItem(i/4,i-(i/4)*4,new QTableWidgetItem(Text));

        i++;
    }
    ItemDelegate.ComboBox=this;
    setItemDelegate(&ItemDelegate);
    CurrentFilter=0;
}

//========================================================================================================================

void cOnOffFilterComboBox::SetCurrentFilter(QImage *TheSourceImage,int OnOffFilter) {
    if (TheSourceImage==NULL) return;
    CurrentFilter=OnOffFilter;
    SourceImage=TheSourceImage->scaledToWidth(this->width()/4);
    setCurrentIndex(OnOffFilter/4);
    ((QTableWidget *)view())->setCurrentCell(OnOffFilter/4,OnOffFilter-(OnOffFilter/4)*4);
    for (int i=0;i<4;i++)                  ((QTableWidget *)view())->setColumnWidth(i,SourceImage.width());
    for (int i=0;i<(MAXONOFFFILTER/4);i++) ((QTableWidget *)view())->setRowHeight(i,SourceImage.height());
}

//========================================================================================================================

int cOnOffFilterComboBox::GetCurrentFilter() {
    int CurrentRow=currentIndex();
    if (CurrentRow<0) return 0;
    int CurrentCol=((QTableWidget *)view())->currentColumn();
    if (CurrentCol<0) CurrentCol=0;
    CurrentFilter=CurrentRow*4+CurrentCol;
    return CurrentFilter;
}
