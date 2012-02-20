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

#ifndef WGT_JOBCONVERTAUDIO_H
#define WGT_JOBCONVERTAUDIO_H

#include <QWidget>

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

#include "wgt_JobBase.h"

namespace Ui {
    class wgt_JobConvertAudio;
}

class wgt_JobConvertAudio : public wgt_JobBase {
Q_OBJECT
public:
    bool    StopRefreshCB;

    explicit wgt_JobConvertAudio(QCustomDialog *Dialog,QWidget *parent = 0);
    ~wgt_JobConvertAudio();

    virtual void    DoInitDialog();
    virtual void    RefreshControls();
    virtual void    AppendJobSummary(int index,QString *JobSummary,cJobQueue *JobQueue);

private slots:
    void    s_AudioFormatCB(int);
    void    s_AudioBitRateCB(int);
    void    s_ChAudioFreqCB();
    void    s_AudioFreqCB(int);
    void    s_ForceToMonoCB();
    void    s_ForceToStereoCB();

private:
    Ui::wgt_JobConvertAudio *ui;
};

#endif // WGT_JOBCONVERTAUDIO_H
