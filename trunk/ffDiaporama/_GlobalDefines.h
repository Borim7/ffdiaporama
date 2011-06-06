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

/*
    This file contains :
        - All standard library inclusions
        - All QT inclusions
        - Third party library inclusions
        - All global defines
        - Some global static values

    Note : Because this file is included in all other files, it is necessary to limit modification in this file !
*/

#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

//============================================
// Basic inclusions (common to all files)
//============================================

#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif

#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated
#include <math.h>

// SDL Library must be include before any other
extern "C" {
    #include <SDL/SDL.h>
}

#ifdef __MINGW32__
    #undef main // We don't want SDL to override our main()
#endif

//============================================
// Standard QT inclusions needed by all files
//============================================
#include <QtCore>
#include <QtDebug>
#include <QApplication>
#include <QBitmap>
#include <QByteArray>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCursor>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QHeaderView>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QList>
#include <QListView>
#include <QMainWindow>
#include <QMatrix>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QObject>
#include <QPaintDevice>
#include <QPainter>
#include <QProcess>
#include <QProgressBar>
#include <QScrollBar>
#include <QShowEvent>
#include <QSize>
#include <QSizeF>
#include <QSlider>
#include <QString>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QStyleOptionGraphicsItem>
#include <QStyleOptionViewItem>
#include <QTableWidget>
#include <QTextOption>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QTranslator>
#include <QWidget>
#include <QWindowsStyle>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//============================================
// Other third party library inclusions
//============================================

// ffmpeg
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

// FMT_FILTERS
#include "fmt_filters.h"

//============================================
// Basic class from this project
//============================================
#include "_ImageFileWrapper.h"
#include "_VideoFileWrapper.h"

//============================================
// Global defines
//============================================
#define APPLICATION_NAME                    "ffDiaporama"
#define APPLICATION_VERSION                 "0.99.alpha2"
#define CONFIGFILEEXT                       ".xml"                                  // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                         // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                               // Name of root node in the project xml file
#define GLOBALCONFIGFILE                    2                                       // Type of config file : Global
#define USERCONFIGFILE                      3                                       // Type of config file : USER
#define BUFFERING_NBR_FRAME                 5                                       // Number of frame wanted in the playing buffer

#define ICON_PLAYERPLAY                     "icons/player_play.png"                 // FileName of play icon
#define ICON_PLAYERPAUSE                    "icons/player_pause.png"                // FileName of pause icon
#define ICON_SHOTPRESENCE                   "icons/zoom.png"                        // FileName of icon representing shots in the timeline
#define ICON_VISIBLE_OK                     "icons/Visible_OK.png"
#define ICON_VISIBLE_KO                     "icons/Visible_KO.png"
#define ICON_SOUND_OK                       "icons/object_sound.png"
#define ICON_SOUND_KO                       "icons/sound_KO.png"

#define ICON_OBJECT_TEXT                    "icons/object_text.png"
#define ICON_OBJECT_SOUND                   "icons/object_sound.png"
#define ICON_OBJECT_MOVIE                   "icons/object_movie.png"
#define ICON_OBJECT_IMAGE                   "icons/object_image.png"
#define ICON_OBJECT_TEXTHIDE                "icons/object_textHide.png"
#define ICON_OBJECT_MOVIEHIDE               "icons/object_movieHide.png"
#define ICON_OBJECT_IMAGEHIDE               "icons/object_imageHide.png"
#define ICON_RULER_ON                       "icons/ruler_ok.png"
#define ICON_RULER_OFF                      "icons/ruler_ko.png"

// Project geometry definition
#define GEOMETRY_4_3                        0
#define GEOMETRY_16_9                       1
#define GEOMETRY_40_17                      2

// Image geometry definition
#define GEOMETRY_PROJECT                    0
#define GEOMETRY_IMAGE                      1
#define GEOMETRY_CUSTOM                     2

// Standard definition
#define STANDARD_PAL                        0
#define STANDARD_NTSC                       1

// Image size definition
#define NBR_SIZEDEF                         4
#define SIZE_VGA                            0
#define SIZE_DVD                            1
#define SIZE_720P                           2
#define SIZE_1080p                          3

// Object type definition
#define DIAPORAMAOBJECTTYPE_EMPTY           0
#define DIAPORAMAOBJECTTYPE_IMAGE           1
#define DIAPORAMAOBJECTTYPE_VIDEO           2

// Composition type definition
#define COMPOSITIONTYPE_BACKGROUND          0
#define COMPOSITIONTYPE_OBJECT              1
#define COMPOSITIONTYPE_SHOT                2

// Shot type definition
#define SHOTTYPE_STATIC                     0
#define SHOTTYPE_MOBIL                      1
#define SHOTTYPE_VIDEO                      2

// Thumbnail type definition
#define THUMBNAILTYPE_OBJECTBACKGROUND      0
#define THUMBNAILTYPE_OBJECTSEQUENCE        1
#define THUMBNAILTYPE_OBJECTMUSIC           2
#define THUMBNAILTYPE_SHOT                  3

// Speed wave forme definition
#define SPEEDWAVE_LINEAR                    0
#define SPEEDWAVE_SINQUARTER                1

// Track definition
#define TRACKBACKGROUND                     0
#define TRACKMONTAGE                        1
#define TRACKMUSIC                          2

// Thumbnails definitions
#define ADJUSTXCOLUMN                       10      // width to add to object image for display transition
#define TIMELINEMINHEIGH                    80      // min heigh of montage track
#define TIMELINEMAXHEIGH                    160     // max heigh of montage track
#define TIMELINESOUNDHEIGHT                 18      // Height of sound & music track

// Composition parameters
#define SCALINGTEXTFACTOR                   400

// Brush type definition
#define BRUSHTYPE_NOBRUSH                   0
#define BRUSHTYPE_SOLID                     1
#define BRUSHTYPE_PATTERN                   2
#define BRUSHTYPE_GRADIENT2                 3
#define BRUSHTYPE_GRADIENT3                 4
#define BRUSHTYPE_IMAGELIBRARY              5
#define BRUSHTYPE_IMAGEDISK                 6

// Brush Gradient orientation
#define GRADIENTORIENTATION_UPLEFT          0
#define GRADIENTORIENTATION_UP              1
#define GRADIENTORIENTATION_UPRIGHT         2
#define GRADIENTORIENTATION_LEFT            3
#define GRADIENTORIENTATION_RADIAL          4
#define GRADIENTORIENTATION_RIGHT           5
#define GRADIENTORIENTATION_BOTTOMLEFT      6
#define GRADIENTORIENTATION_BOTTOM          7
#define GRADIENTORIENTATION_BOTTOMRIGHT     8

// OnOffFilter mask definition
#define FilterEqualize                      0x01
#define FilterDespeckle                     0x02
#define FilterGray                          0x04

// Transition familly definition
#define TRANSITIONFAMILLY_BASE              0
#define TRANSITIONFAMILLY_ZOOMINOUT         1
#define TRANSITIONFAMILLY_SLIDE             2
#define TRANSITIONFAMILLY_PUSH              3
#define TRANSITIONFAMILLY_LUMA_BAR          4
#define TRANSITIONFAMILLY_LUMA_BOX          5
#define TRANSITIONFAMILLY_LUMA_CENTER       6
#define TRANSITIONFAMILLY_LUMA_CHECKER      7
#define TRANSITIONFAMILLY_LUMA_CLOCK        8
#define TRANSITIONFAMILLY_LUMA_SNAKE        9

// No luma transition : number of sub type
#define TRANSITIONMAXSUBTYPE_BASE           2
#define TRANSITIONMAXSUBTYPE_ZOOMINOUT      18
#define TRANSITIONMAXSUBTYPE_SLIDE          16
#define TRANSITIONMAXSUBTYPE_PUSH           8

//============================================
// Default values
//============================================
#define DEFAULT_FONT_FAMILLY                "Arial"
#define DEFAULT_FONT_SIZE                   12
#define DEFAULT_FONT_COLOR                  "#ffffaa"
#define DEFAULT_FONT_SHADOWCOLOR            "#333333"
#define DEFAULT_FONT_ISBOLD                 false
#define DEFAULT_FONT_ISITALIC               false
#define DEFAULT_FONT_ISUNDERLINE            false
#define DEFAULT_FONT_HALIGN                 1
#define DEFAULT_FONT_VALIGN                 1
#define DEFAULT_FONT_TEXTEFFECT             5

#define DEFAULT_SHAPE_OPACITY               0
#define DEFAULT_SHAPE_BORDERSIZE            1
#define DEFAULT_SHAPE_BORDERCOLOR           "#333333"
#define DEFAULT_SHAPE_INTERNALBORDERSIZE    6
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR1  "#808080"
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR2  "#c0c0c0"
#define DEFAULT_SHAPE_BRUSHCOLORD           "#808080"

//============================================
// String to include in the system properties
//============================================

#define OPERATINGSYSTEM_STR                 "Operating system version\t: "
#define STARTINGPATH_STR                    "Starting path\t: "
#define WORKINGPATH_STR                     "Working path set to\t: "
#define SYSTEMLOCAL_STR                     "Detected system locale\t: "
#define LOADEDLOCAL_STR                     "Translation file loaded\t: "
#define BUILDVERSION_STR                    "Build version\t\t: "
#define COMMONPARTVERSION_STR               "Common part version\t: "
#define VERSIONQT_STR                       "QT version\t\t: "
#define FMTFILTERVERSION_STR                "fmt_filters version\t: "
#define SDLVERSION_STR                      "SDL version\t\t: "
#define LIBAVCODECVERSION_STR               "LIBAVCODEC version\t: "
#define LIBAVFORMATVERSION_STR              "LIBAVFORMAT version\t: "
#define LIBSWSCALEVERSION_STR               "LIBSWSCALE version\t: "
#define TEMPDIR_STR                         "Temp directory to be use\t: "
#define USERCONFIGFILE_STR                  "User config file to be use\t: "

//============================================
// Image format definition
//============================================
struct sIMAGEDEF {
    int     Width;                                              // Width
    int     Height;                                             // Height
    int     PARNUM;                                             // Pixel aspect ratio (num)
    int     PARDEN;                                             // Pixel aspect ratio (den)
    double  dFPS;                                               // Frame per second
    char    FPS[20];                                            // Frame per second
    char    Name[100];                                          // Display name
    int     Extend;                                             // Padding for cinema mode with DVD
};
extern sIMAGEDEF DefImageFormat [2][3][4];                      // Real definition in capplicationconfig.cpp

//============================================
// Video codec definitions
//============================================
struct sVideoCodecDef {
    bool    IsFind;                                             // true if codec is supported by installed version of ffmpeg
    int     Codec_id;                                           // ffmpeg codec id
    char    ShortName[50];                                      // short name of the codec (copy of the ffmpeg value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate[200];                               // list of possible compression bit rate (define by this application)
    char    DefaultBitrate[NBR_SIZEDEF][10];                    // prefered compression bit rate for each possible size
};
#define NBR_VIDEOCODECDEF   6
extern struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF];  // Real definition in capplicationconfig.cpp

//============================================
// Audio codec definitions
//============================================
struct sAudioCodecDef {
    bool    IsFind;                                             // true if codec is supported by installed version of ffmpeg
    int     Codec_id;                                           // ffmpeg codec id
    char    ShortName[50];                                      // short name of the codec (copy of the ffmpeg value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate2CH[200];                            // list of possible compression bit rate in stereo mode (define by this application)
    bool    Possibly6CH;                                        // true if this codec support 5.1/6 chanels mode
    char    PossibleBitrate6CH[200];                            // list of possible compression bit rate in 5.1/6 chanels mode (define by this application)
    char    Default[10];                                        // prefered compression bit rate
};
#define NBR_AUDIOCODECDEF   6
extern struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF];  // Real definition in capplicationconfig.cpp

//============================================
// Format container definitions
//============================================
struct sFormatDef {
    bool    IsFind;                                             // true if format container is supported by installed version of ffmpeg
    char    ShortName[50];                                      // short name of the format container (copy of the ffmpeg value)
    char    FileExtension[10];                                  // prefered file extension for the format container (define by this application)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleVideoCodec[200];                            // list of possible video codec for this format container (define by this application)
    char    PossibleAudioCodec[200];                            // list of possible audio codec for this format container (define by this application)
};
#define NBR_FORMATDEF   9
extern struct sFormatDef FORMATDEF[NBR_FORMATDEF];              // Real definition in capplicationconfig.cpp

#endif // GLOBALDEFINES_H