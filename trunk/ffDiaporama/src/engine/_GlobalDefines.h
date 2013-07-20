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

#ifndef _GLOBALDEFINES_H
#define _GLOBALDEFINES_H

// Remove unwanted warnings when using MSVC
#ifndef _DEPRECATION_DISABLE                /* One time only */
    #define _DEPRECATION_DISABLE            /* Disable deprecation true */
    #if (_MSC_VER >= 1400)                  /* Check version */
        #pragma warning(disable: 4996)      /* Disable deprecation */
        #pragma warning(disable: 4005)      /* Disable warning on macro redefinition */
    #endif                                  /* #if defined(NMEA_WIN) && (_MSC_VER >= 1400) */
#endif                                      /* #ifndef _DEPRECATION_DISABLE */

/* ======================================================================
    THIS FILE MUST ABSOLUTELY BE REFERENCED AT FIRST IN ALL .h FILES OF
    THE PROJECT
   ====================================================================== */

//============================================
// Activate standard stdint macro
//============================================
#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif

#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated

//============================================
// Specific for MSVC
//============================================

#ifdef _MSC_VER
    #define snprintf    _snprintf_s
    #define AVRCAST                     // MSC doesn't allow CAST in struct constant definition
#else
    #define AVRCAST (AVRational)        // mingw need CAST in struct constant definition
#endif

//============================================
// Minimum QT inclusions needed by all files
//============================================
#include <QtCore>
#include <QApplication>
#include <QtDebug>
#include <QDomElement>

#if QT_VERSION >= 0x050000

    #include <QtConcurrent>

#else   // Qt4 code

    #ifdef Q_OS_WIN
    void    SetLFHeap();
    #endif

#endif

//====================================================================
// For windows, windows.h and winbase.h must be included after QtCore
//====================================================================
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <winbase.h>
    extern bool IsWindowsXP;    // True if OS is Windows/XP

    // These type are undefined under Windows with mingw 4.4
    typedef unsigned char       u_int8_t;
    typedef unsigned short      u_int16_t;
    typedef unsigned            u_int32_t;
    typedef unsigned long long  u_int64_t;
#endif


//====================================================================

// Note : Application version and revision are in BUILDVERSION.txt
// Syntax for BUILDVERSION.txt is : <Version MAJOR.MINOR[.SUB|_beta_VERSION|_devel]>
#define APPLICATION_NAME                    "ffDiaporama"
#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                             // Name of root node in the project xml file
#define THUMBMODEL_ROOTNAME                 "Thumbnail"                           // Name of root node in the thumbnail xml files
#define THUMBWITH                           600
#define THUMBHEIGHT                         800
#define THUMBGEOMETRY                       (double(THUMBWITH)/double(THUMBHEIGHT))

// Application version : url to file on internet
//#define BUILDVERSION_WEBURL                "http://ffdiaporama.tuxfamily.org/Stable/BUILDVERSION.txt"
#define BUILDVERSION_WEBURL                 "http://ffdiaporama.tuxfamily.org/Devel/BUILDVERSION.txt"

// Global values
extern QString CurrentAppName;              // Application name (including devel, beta, ...)
extern QString CurrentAppVersion;           // Application version read from BUILDVERSION.txt
extern double  ScreenFontAdjust;            // System Font adjustement

// URL to link to help page
#define HELPFILE_CAT                        "http://ffdiaporama.tuxfamily.org/?cat=%1&lang=%2"
#define HELPFILE_DEF                        "http://ffdiaporama.tuxfamily.org/?page_id=%1&lang=%2"
#define ALLOWEDWEBLANGUAGE                  "en;fr;it;es;el;de;nl"

//====================================================================

enum    FilterFile          {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
enum    LoadConfigFileType  {USERCONFIGFILE,GLOBALCONFIGFILE};

//====================================================================
// Internal log defines and functions
//====================================================================

// Log level for message
#define LOGMSG_DEBUGTRACE                   1
#define LOGMSG_INFORMATION                  2
#define LOGMSG_WARNING                      3
#define LOGMSG_CRITICAL                     4

const QEvent::Type BaseAppEvent = (QEvent::Type)2000;   // The custom event will be send to EventReceiver (if EventReceiver not null)
#define EVENT_GeneralLogChanged             1           // General internal event code to display log message

extern int          LogMsgLevel;                        // Level from wich debug message was print to stdout
extern QStringList  EventList;                          // Internal event queue
extern QObject      *EventReceiver;                     // Windows wich receive event

void    PostEvent(int EventType,QString EventParam="");
void    ToLog(int MessageType,QString Message,QString Source="internal",bool AddBreak=true);
QString ito2a(int val);
QString ito3a(int val);
double  GetDoubleValue(QDomElement CorrectElement,QString Name);    // Load a double value from an XML element
double  GetDoubleValue(QString sValue);                             // Load a double value from a string

//====================================================================
// VARIOUS
//====================================================================

enum SELECTMODE {SELECTMODE_NONE, SELECTMODE_ONE, SELECTMODE_MULTIPLE};

//====================================================================

// Table of sentence that ffDText could return
enum ffDSection_ID {
    // Section definition
    ffDSection_CommonErrorMsg,
    ffDSection_CommonInfoMsg,
    ffDSection_DlgImageCorrection
};

#define TEXTID(Section,Id) (Section*1000+Id)

QString ffDText(ffDSection_ID SectionId,int TextId);
QString AdjustDirForOS(QString Dir);                                                                                // Adjust separator in pathname depending on operating system

#endif // _GLOBALDEFINES_H