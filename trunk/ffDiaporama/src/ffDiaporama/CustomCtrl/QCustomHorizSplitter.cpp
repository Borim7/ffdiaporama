/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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

// This class handle a standard horizontal QSplitter with 2 children. Only second child is expanding when splitter is resizing

#include "QCustomHorizSplitter.h"

#include <QList>
#include <QSize>

QCustomHorizSplitter::QCustomHorizSplitter(QWidget *parent):QSplitter(parent) {
    IsFirstInitDone=false;
}

void QCustomHorizSplitter::resizeEvent(QResizeEvent *event) {
    if (!IsFirstInitDone) QSplitter::resizeEvent(event); else {
        QList<int> List=sizes();
        List[1]+=(event->size().width()-event->oldSize().width());
        setSizes(List);
    }
}

void QCustomHorizSplitter::showEvent(QShowEvent *event) {
    QSplitter::showEvent(event);
    IsFirstInitDone=true;
}
