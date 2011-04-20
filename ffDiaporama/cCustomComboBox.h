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

#ifndef cCustomColorComboBox_H
#define cCustomColorComboBox_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

//======================================
// Specific defines for this dialog box
//======================================
#define COMBOTYPEITEM_COLOR     0
#define COMBOTYPEITEM_BRUSH     1

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Color ComboBox
//******************************************************************************************************************

class cCustomColorComboBox;
class cCustomColorComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cCustomColorComboBox    *ComboBox;

    explicit cCustomColorComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Color ComboBox
//******************************************************************************************************************

class cCustomColorComboBox : public QComboBox {
Q_OBJECT
public:
    QString                     CurrentColor;
    bool                        StandardColor;
    cCustomColorComboBoxItem    ItemDelegate;

    explicit    cCustomColorComboBox(QWidget *parent = 0);
    void        MakeIcons();
    void        SetCurrentColor(QString Color);
    QString     GetCurrentColor();

protected:

signals:

public slots:
};

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Brush ComboBox
//******************************************************************************************************************

class cCustomBrushComboBox;
class cCustomBrushComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cCustomBrushComboBox    *ComboBox;

    explicit cCustomBrushComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

class cCustomBrushComboBox : public QComboBox {
Q_OBJECT
public:
    cBrushDefinition            Brush;
    cCustomBrushComboBoxItem    ItemDelegate;

    explicit            cCustomBrushComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBrush(cBrushDefinition Brush);
    cBrushDefinition    GetCurrentBrush();

protected:

signals:

public slots:
};


//******************************************************************************************************************
// Custom QAbstractItemDelegate for Brush ComboBox
//******************************************************************************************************************

class cGradientOrientationComboBox;
class cGradientOrientationComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cGradientOrientationComboBox    *ComboBox;

    explicit cGradientOrientationComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

class cGradientOrientationComboBox : public QComboBox {
Q_OBJECT
public:
    cBrushDefinition                    Brush;
    cGradientOrientationComboBoxItem    ItemDelegate;

    explicit            cGradientOrientationComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBrush(cBrushDefinition Brush);
    cBrushDefinition    GetCurrentBrush();

protected:

signals:

public slots:
};

//******************************************************************************************************************
// Custom QAbstractItemDelegate for OnOffFilter ComboBox
//******************************************************************************************************************

class cOnOffFilterComboBox;
class cOnOffFilterComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cOnOffFilterComboBox    *ComboBox;

    explicit cOnOffFilterComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom OnOff-filter ComboBox
//******************************************************************************************************************

class cOnOffFilterComboBox : public QComboBox {
Q_OBJECT
public:
    int                         CurrentFilter;
    cOnOffFilterComboBoxItem    ItemDelegate;
    QImage                      SourceImage;

    explicit    cOnOffFilterComboBox(QWidget *parent = 0);
    void        SetCurrentFilter(QImage *TheSourceImage,int OnOffFilter);
    int         GetCurrentFilter();

protected:

signals:

public slots:
};

#endif // cCustomColorComboBox_H
