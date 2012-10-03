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

#ifndef cCustomColorComboBox_H
#define cCustomColorComboBox_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

#include <QStyledItemDelegate>
#include <QLabel>

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
    QString                     *CurrentColor;
    QString                     SavedCustomColor;
    bool                        StandardColor;
    bool                        STOPMAJ;
    cCustomColorComboBoxItem    ItemDelegate;

    explicit    cCustomColorComboBox(QWidget *parent = 0);
    void        MakeIcons();
    void        SetCurrentColor(QString *Color);
    QString     GetCurrentColor();

protected:

signals:

public slots:
    void        s_ItemSelectionChanged();
    void        s_ItemPressed(int Row,int Col);
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
    bool                        STOPMAJ;
    cBrushDefinition            *Brush;
    cCustomBrushComboBoxItem    ItemDelegate;

    explicit            cCustomBrushComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBrush(cBrushDefinition *Brush);
    cBrushDefinition    *GetCurrentBrush();

protected:

signals:

public slots:
    void s_ItemSelectionChanged();
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
    bool                                STOPMAJ;
    cBrushDefinition                    *Brush;
    cGradientOrientationComboBoxItem    ItemDelegate;

    explicit            cGradientOrientationComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBrush(cBrushDefinition *Brush);
    cBrushDefinition    *GetCurrentBrush();

protected:

signals:

public slots:
    void s_ItemSelectionChanged();
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
    bool                        STOPMAJ;
    int                         *CurrentFilter;
    cOnOffFilterComboBoxItem    ItemDelegate;
    QImage                      SourceImage;

    explicit    cOnOffFilterComboBox(QWidget *parent = 0);
    void        SetCurrentFilter(QImage *TheSourceImage,int *OnOffFilter);
    int         GetCurrentFilter();

protected:

signals:

public slots:
    void s_ItemSelectionChanged();
};

//******************************************************************************************************************
// Custom QAbstractItemDelegate for Background ComboBox
//******************************************************************************************************************

class cBackgroundComboBox;
class cBackgroundComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cBackgroundComboBox    *ComboBox;

    explicit cBackgroundComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom Brush ComboBox
//******************************************************************************************************************

class cBackgroundComboBox : public QComboBox {
Q_OBJECT
public:
    bool                       STOPMAJ;
    QString                    BrushImage;
    cBackgroundComboBoxItem    ItemDelegate;

    explicit            cBackgroundComboBox(QWidget *parent = 0);
    void                MakeIcons();
    void                SetCurrentBackground(QString BrushImage);
    QString             GetCurrentBackground();

protected:

signals:

public slots:
    void s_ItemSelectionChanged();
};

//******************************************************************************************************************
// Custom QAbstractItemDelegate for AutoFraming ComboBox
//******************************************************************************************************************

class cFramingComboBox;
class cFramingComboBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    cFramingComboBox    *ComboBox;

    explicit cFramingComboBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//******************************************************************************************************************
// Custom AutoFraming ComboBox
//******************************************************************************************************************

#define FILTERFRAMING_CUSTOM    0x01
#define FILTERFRAMING_IMAGE     0x02
#define FILTERFRAMING_PROJECT   0x04
#define FILTERFRAMING_ALL       0x07
#define FILTERFRAMING_INTERNAL  0x08

class cFramingStyleTableItem {
public:
    QImage  Image;
    int     FrameStyle;
    cFramingStyleTableItem(QImage *Image,int FrameStyle);
};

class cFramingComboBox : public QComboBox {
Q_OBJECT
public:
    QList<cFramingStyleTableItem>   FramingStyleTable;
    bool                            STOPMAJ;
    cFramingComboBoxItem            ItemDelegate;
    qreal                           X,Y,ZoomFactor,AspectRatio;
    int                             CurrentFilter,CurrentFramingStyle,CurrentNbrITem;

    explicit            cFramingComboBox(QWidget *parent = 0);
    void                PrepareFramingStyleTable(bool ResetContent,int Filter,cBrushDefinition *Brush,QImage *SourceImage,int BackgroundForm,qreal ProjectGeometry);
    void                MakeIcons();
    void                SetCurrentFraming(int AutoFraming);
    int                 GetCurrentFraming();

protected:
    virtual void        hidePopup();
    virtual void        keyReleaseEvent(QKeyEvent *event);

signals:
    void itemSelectionHaveChanged();

public slots:
    void s_ItemSelectionChanged();
};

//******************************************************************************************************************
// Custom Frame shape ComboBox
//******************************************************************************************************************

#define FILTERFRAMESHAPE_OLDTRIANGLE        0x01

class ccFrameShapeBox;
class cFrameShapeBoxItem : public QStyledItemDelegate {
Q_OBJECT
public:
    ccFrameShapeBox    *ComboBox;

    explicit cFrameShapeBoxItem(QObject *parent=0);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
    virtual QSize   sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

class cFrameShapeTableItem {
public:
    QImage  Image;
    int     FrameStyle;
    cFrameShapeTableItem(QImage *Image,int FrameStyle);
};

class ccFrameShapeBox : public QComboBox {
Q_OBJECT
public:
    QList<cFrameShapeTableItem>     FrameShapeTable;
    bool                            STOPMAJ;
    cFrameShapeBoxItem              ItemDelegate;
    int                             CurrentFilter,CurrentFramingStyle,CurrentNbrITem;
    cApplicationConfig              *ApplicationConfig;

    explicit            ccFrameShapeBox(QWidget *parent = 0);
    void                PrepareFrameShapeTable(bool ResetContent,int Filter,int CurrentBackgroundForm,cApplicationConfig *ApplicationConfig);
    void                MakeIcons();
    void                SetCurrentFrameShape(int FrameShape);
    int                 GetCurrentFrameShape();

protected:
    virtual void        hidePopup();
    virtual void        keyReleaseEvent(QKeyEvent *event);

signals:
    void itemSelectionHaveChanged();

public slots:
    void s_ItemSelectionChanged();
};

//******************************************************************************************************************
// Custom QLabel
//******************************************************************************************************************

class cCustomLabel : public QLabel {
Q_OBJECT
public:
    explicit cCustomLabel(QWidget *parent=0);

    void DisplayCustomText(QString Text);

protected:
    virtual void paintEvent (QPaintEvent *);
};

#endif // cCustomColorComboBox_H