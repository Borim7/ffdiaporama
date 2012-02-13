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

#ifndef DLGJOBSETTINGS_H
#define DLGJOBSETTINGS_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"
#include "../sharedfiles/_QCustomDialog.h"

// Include some additional standard class
#include <QDialog>
#include <QString>

#include "cJobQueue.h"
#include "wgt_JobBase.h"

namespace Ui {
    class DlgJobSettings;
}

class DlgJobSettings : public QCustomDialog {
Q_OBJECT
public:
    cJob        *Job;
    cJobQueue   *JobQueue;
    QString     JobSummary;
    wgt_JobBase *wgt_Job;

    explicit DlgJobSettings(cJob *Job,cJobQueue *JobQueue,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent=0);
    ~DlgJobSettings();

    // function to be overloaded
    virtual void        DoInitDialog();                             // Initialise dialog
    virtual void        DoAccept()          {/*Nothing to do*/}     // Call when user click on Ok button
    virtual void        DoRejet()           {/*Nothing to do*/}     // Call when user click on Cancel button
    virtual void        PrepareGlobalUndo() {/*Nothing to do*/}     // Initiale Undo
    virtual void        DoGlobalUndo()      {/*Nothing to do*/}     // Apply Undo : call when user click on Cancel button

    virtual void        RefreshJobSummary();

private slots:
    void    RefreshControls();

    void    s_Source_Nothing_RD();
    void    s_Source_Delete_RD();
    void    s_Source_Rename_RD();
    void    s_Source_Move_RD();
    void    s_Destination_AddSuffix_RD();
    void    s_Destination_InFolder_RD();
    void    s_Source_Suffix_ED(const QString);
    void    s_Destination_Suffix_ED(const QString);
    void    s_Destination_Overwrite_RD();
    void    s_Source_Folder_BT();
    void    s_Destination_Folder_BT();

private:
    Ui::DlgJobSettings *ui;
};

#endif // DLGJOBSETTINGS_H