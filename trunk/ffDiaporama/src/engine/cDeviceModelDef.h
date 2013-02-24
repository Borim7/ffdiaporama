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

#ifndef CDEVICEMODELDEF_H
#define CDEVICEMODELDEF_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"
#include "_QCustomDialog.h"

// Include some additional standard class
#include <QString>
#include <QStringList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

// Include some common various class
#include "cBaseApplicationConfig.h"

//****************************************************************************************************************************************************************
// LIBAV inclusion
//****************************************************************************************************************************************************************

extern "C" {
    #ifndef INT64_C
    #define INT64_C(c) (c ## LL)
    #define UINT64_C(c) (c ## ULL)
    #endif

    #include <libavcodec/version.h>
    #include <libavdevice/avdevice.h>
    #if LIBAVCODEC_VERSION_INT <= AV_VERSION_INT(53,8,0)
        #include "libavfilter/avfilter.h"
    #else
        #include <libavfilter/version.h>
    #endif
    #include <libavformat/version.h>
    #include <libavutil/avutil.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>

    #include <libavutil/mathematics.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/audioconvert.h>

    #include <libavcodec/avcodec.h>

    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>

    #ifdef USELIBAVRESAMPLE
        #include "libavresample/avresample.h"
    #endif

    // include for libavfilter
    #if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(2,13,0)       // For all
        #if LIBAVCODEC_VERSION_INT > AV_VERSION_INT(53,8,0)
            #include "libavfilter/avfilter.h"
        #endif
        #include "libavfilter/avfiltergraph.h"
        #if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,23,0)    // From 2.13 to 2.23
            #include "libavfilter/buffersrc.h"
            #include "libavfilter/vsrc_buffer.h"
        #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,60,0)   // From 2.23 to 2.60
            #include "libavfilter/avcodec.h"
            #include "libavfilter/vsrc_buffer.h"
        #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,0,0)   // From 2.6 to 3.0
            #include "libavfilter/buffersink.h"
            #include "libavfilter/avcodec.h"
        #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,1,0)   // From 3.0 to 3.1
            #include "libavfilter/buffersink.h"
            #include "libavfilter/avcodec.h"
        #else                                                   // From 3.1
            #if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(3,17,0)   // From 3.17
                #include "libavfilter/avcodec.h"
            #endif
            #include "libavfilter/buffersink.h"
            #include "libavfilter/buffersrc.h"
        #endif
    #endif
}

#if ((LIBAVUTIL_VERSION_INT>=AV_VERSION_INT(52,0,0))&&(LIBAVCODEC_VERSION_INT>=AV_VERSION_INT(54,31,0))&&(LIBAVFORMAT_VERSION_INT>=AV_VERSION_INT(54,19,0)))
    #define LIBAV_09
#elif ((LIBAVUTIL_VERSION_INT>=AV_VERSION_INT(51,22,0))&&(LIBAVCODEC_VERSION_INT>=AV_VERSION_INT(53,35,0))&&(LIBAVFORMAT_VERSION_INT>=AV_VERSION_INT(53,21,0)))
    #define LIBAV_08
#elif ((LIBAVUTIL_VERSION_INT>=AV_VERSION_INT(51,7,0))&&(LIBAVCODEC_VERSION_INT>=AV_VERSION_INT(53,6,0))&&(LIBAVFORMAT_VERSION_INT>=AV_VERSION_INT(53,3,0)))
    #define LIBAV_07
#endif

#if !defined(FF_API_CODEC_ID)
    #define AV_CODEC_ID_NONE        CODEC_ID_NONE
    #define AV_CODEC_ID_MJPEG       CODEC_ID_MJPEG
    #define AV_CODEC_ID_MPEG2VIDEO  CODEC_ID_MPEG2VIDEO
    #define AV_CODEC_ID_MPEG4       CODEC_ID_MPEG4
    #define AV_CODEC_ID_H264        CODEC_ID_H264
    #define AV_CODEC_ID_VP8         CODEC_ID_VP8
    #define AV_CODEC_ID_FLV1        CODEC_ID_FLV1
    #define AV_CODEC_ID_THEORA      CODEC_ID_THEORA
    #define AV_CODEC_ID_WMV1        CODEC_ID_WMV1
    #define AV_CODEC_ID_WMV2        CODEC_ID_WMV2
    #define AV_CODEC_ID_WMV3        CODEC_ID_WMV3
    #define AV_CODEC_ID_PCM_S16LE   CODEC_ID_PCM_S16LE
    #define AV_CODEC_ID_MP3         CODEC_ID_MP3
    #define AV_CODEC_ID_AAC         CODEC_ID_AAC
    #define AV_CODEC_ID_AC3         CODEC_ID_AC3
    #define AV_CODEC_ID_VORBIS      CODEC_ID_VORBIS
    #define AV_CODEC_ID_MP2         CODEC_ID_MP2
    #define AV_CODEC_ID_AMR_WB      CODEC_ID_AMR_WB
    #define AV_CODEC_ID_AMR_NB      CODEC_ID_AMR_NB
    #define AV_CODEC_ID_FLAC        CODEC_ID_FLAC
    #define AV_CODEC_ID_WMAV1       CODEC_ID_WMAV1
    #define AV_CODEC_ID_WMAV2       CODEC_ID_WMAV2
#endif

//****************************************************************************************************************************************************************

#define CONFIGFILENAME      "Devices"
#define CONFIGDOCNAME       "DEVICES"
#define CONFIGROOTNAME      "Configuration"

// Export mode definition
#define EXPORTMODE_ADVANCED                 0
#define ICON_ADVANCED                       ":/img/render.png"
#define MODE_SMARTPHONE                     1
#define ICON_SMARTPHONE                     ":/img/Smartphone.png"
#define MODE_MULTIMEDIASYS                  2
#define ICON_MULTIMEDIASYS                  ":/img/tv.png"
#define MODE_FORTHEWEB                      3
#define ICON_FORTHEWEB                      ":/img/Internet.png"
#define MODE_LOSSLESS                       4
#define ICON_LOSSLESS                       ":/img/Lossless.png"
#define MODE_SOUNDTRACK                     5
#define ICON_SOUNDTRACK                     ":/img/object_sound.png"

//============================================

// Standard definition
#define STANDARD_PAL                        0
#define STANDARD_NTSC                       1

// Image size definition
#define NBR_SIZEDEF                         11
#define SIZE_QVGA                           0
#define SIZE_HVGA                           1
#define SIZE_VGA                            2
#define SIZE_DVD                            3
#define SIZE_WVGA                           4
#define SIZE_XGA                            5
#define SIZE_720P                           6
#define SIZE_1080p                          7
#define SIZE_RIM240                         8

//============================================
// Image format definition
//============================================
struct sIMAGEDEF {
    int         Width;                                          // Width
    int         Height;                                         // Height
    int         PARNUM;                                         // Pixel aspect ratio (num)
    int         PARDEN;                                         // Pixel aspect ratio (den)
    double      dFPS;                                           // Frame per second
    char        FPS[20];                                        // Frame per second
    AVRational  AVFPS;
    char        Name[100];                                      // Display name
    int         Extend;                                         // Padding for cinema mode with DVD
};
extern sIMAGEDEF DefImageFormat [2][3][NBR_SIZEDEF];            // Image format definition
extern QString   IMAGEDEFGENNAME[2][NBR_SIZEDEF];               // Image format generic name
extern int       ORDERIMAGENAME[2][NBR_SIZEDEF];                // Display order of image size

//============================================
// Video codec definitions
//============================================
#define     VCODEC_MJPEG        0                               // Motion JPEG
#define     VCODECST_MJPEG      "MJPEG"                         // String Motion JPEG
#define     VCODEC_MPEG         1                               // MPEG-2 video
#define     VCODECST_MPEG       "MPEG"                          // String MPEG-2 video
#define     VCODEC_MPEG4        2                               // DivX/XVid/MPEG-4
#define     VCODECST_MPEG4      "MPEG4"                         // String DivX/XVid/MPEG-4
#define     VCODEC_H264HQ       3                               // H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset
#define     VCODECST_H264HQ     "H264HQ"                        // String H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset
#define     VCODEC_H264PQ       4                               // H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset ********
#define     VCODECST_H264PQ     "H264PQ"                        // String H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset ********
#define     VCODEC_VP8          5                               // WebM-VP8
#define     VCODECST_VP8        "VP8"                           // String WebM-VP8
#define     VCODEC_H263         6                               // Flash Video / Sorenson H.263
#define     VCODECST_H263       "H263"                          // String Flash Video / Sorenson H.263
#define     VCODEC_THEORA       7                               // Theora
#define     VCODECST_THEORA     "THEORA"                        // String Theora
#define     VCODEC_X264LL       8                               // x264 lossless + VPRE libx264-lossless.ffpreset ********
#define     VCODECST_X264LL     "X264LL"                        // String x264 lossless ********
#define     VCODEC_WMV1         9                               // Windows Media Video 7
#define     VCODECST_WMV1       "WMV1"                          // String Windows Media Video 7
#define     VCODEC_WMV2         10                              // Windows Media Video 8
#define     VCODECST_WMV2       "WMV2"                          // String Windows Media Video 8
#define     VCODEC_WMV3         11                              // Windows Media Video 9
#define     VCODECST_WMV3       "WMV3"                          // String Windows Media Video 9

struct sVideoCodecDef {
    bool    IsFind,IsRead;                                      // true if codec is supported for write,read by installed version of libav
    int     Codec_id;                                           // libav codec id
    int     FFD_VCODEC;                                         // ffDiaporama video codec id
    char    FFD_VCODECST[10];                                   // ffDiaporama video codec id string
    char    ShortName[50];                                      // short name of the codec (copy of the libav value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate[200];                               // list of possible compression bit rate (define by this application)
    char    DefaultBitrate[2][NBR_SIZEDEF][10];                 // prefered compression bit rate for each possible size
};
#define NBR_VIDEOCODECDEF   12
extern struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF];

//============================================
// Audio codec definitions
//============================================

struct sAudioCodecDef {
    bool    IsFind,IsRead;                                      // true if codec is supported for write,read by installed version of libav
    int     Codec_id;                                           // libav codec id
    char    ShortName[50];                                      // short name of the codec (copy of the libav value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate2CH[200];                            // list of possible compression bit rate in stereo mode (define by this application)
    bool    Possibly6CH;                                        // true if this codec support 5.1/6 chanels mode
    char    PossibleBitrate6CH[200];                            // list of possible compression bit rate in 5.1/6 chanels mode (define by this application)
    char    Default[10];                                        // prefered compression bit rate
    char    PossibleFrequency[200];                             // list of possible audio frequency
    int     PreferedAudioContainer;                             // If use alone : preferred AFORMAT_ID
};
#define NBR_AUDIOCODECDEF   11
extern struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF];

//============================================
// Format container definitions
//============================================

enum VFORMAT_ID {
    VFORMAT_3GP,
    VFORMAT_AVI,
    VFORMAT_MKV,
    VFORMAT_MJPEG,
    VFORMAT_MP4,
    VFORMAT_MPEG,
    VFORMAT_WEBM,
    VFORMAT_OLDFLV,
    VFORMAT_FLV,
    VFORMAT_OGV,
    VFORMAT_WMV,
    VFORMAT_NBR
};

struct sFormatDef {
    bool    IsFind,IsRead;                                      // true if format container is supported for write,read by installed version of libav
    char    ShortName[50];                                      // short name of the format container (copy of the libav value)
    char    FileExtension[10];                                  // prefered file extension for the format container (define by this application)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleVideoCodec[200];                            // list of possible video codec for this format container (using VCODECST String define)
    char    PossibleAudioCodec[200];                            // list of possible audio codec for this format container (define by this application)
    char    PossibleFrequency[200];                             // list of possible audio frequency
    char    DefaultAudioFreq[10];                               // prefered audio frequency
};
extern struct sFormatDef FORMATDEF[VFORMAT_NBR];

enum AFORMAT_ID {
    AFORMAT_3GP,
    AFORMAT_AC3,
    AFORMAT_AAC,
    AFORMAT_FLAC,
    AFORMAT_MP4,
    AFORMAT_MP2,
    AFORMAT_MP3,
    AFORMAT_OGG,
    AFORMAT_WAV,
    NBR_AFORMAT     // Last of the list !
};
extern struct sFormatDef AUDIOFORMATDEF[NBR_AFORMAT];

//============================================
// Device model class definition
//============================================

class cDeviceModelDef {
public:
    bool    FromGlobalConf;                                     // true if device model is defined in global config file
    bool    FromUserConf;                                       // true if device model is defined in user config file
    bool    IsFind;                                             // true if device model format is supported by installed version of ffmpeg
    int     DeviceIndex;                                        // Device number index key
    QString DeviceName;                                         // long name for the device model
    int     DeviceType;                                         // device type
    int     DeviceSubtype;                                      // device Subtype
    int     Standard;                                           // standard : PAL/NTSC
    int     FileFormat;                                         // sFormatDef number
    int     ImageSize;                                          // DefImageFormat number
    int     VideoCodec;                                         // sVideoCodecDef number
    int     VideoBitrate;                                       // Bitrate number in sVideoCodecDef
    int     AudioCodec;                                         // sAudioCodecDef number
    int     AudioBitrate;                                       // Bitrate number in sAudioCodecDef

    // Save value to be able to reset to default
    QString BckDeviceName;                                      // long name for the device model
    int     BckDeviceType;                                      // device type
    int     BckDeviceSubtype;                                   // device Subtype
    int     BckStandard;                                        // standard : PAL/NTSC
    int     BckFileFormat;                                      // sFormatDef number
    int     BckImageSize;                                       // DefImageFormat number
    int     BckVideoCodec;                                      // sVideoCodecDef number
    int     BckVideoBitrate;                                    // Bitrate number in sVideoCodecDef
    int     BckAudioCodec;                                      // sAudioCodecDef number
    int     BckAudioBitrate;                                    // Bitrate number in sAudioCodecDef

    cDeviceModelDef(bool IsGlobalConf,int IndexKey);
    virtual         ~cDeviceModelDef();

    virtual void    SaveToXML(QDomElement &domDocument,QString ElementName);
    virtual bool    LoadFromXML(QDomElement domDocument,QString ElementName,bool IsUserConfigFile);
};

//============================================
// Device model list definition
//============================================

class cDeviceModelList {
public:
    QList<cDeviceModelDef *>    RenderDeviceModel;                          // List of known rendering device model
    QStringList                 TranslatedRenderType;                       // Translated render device type
    QStringList                 TranslatedRenderSubtype[4];                 // Translated render device subtype

    cDeviceModelList();
    virtual         ~cDeviceModelList();

    virtual bool    LoadConfigurationFile(QString ConfigFileName,cBaseApplicationConfig::LoadConfigFileType TypeConfigFile);
    virtual bool    SaveConfigurationFile(QString ConfigFileName);
    virtual bool    LoadFromXML(QDomElement domDocument,cBaseApplicationConfig::LoadConfigFileType TypeConfigFile);

    virtual void    TranslatRenderType();
    virtual bool    Initffmpeg(QString &BinaryEncoderPath);
};

//============================================
// Allowed file extensions for reading
//============================================

extern QString AllowVideoExtensions;       // List of all file extension allowed for video
extern QString AllowImageExtensions;       // List of all file extension allowed for image
extern QString AllowMusicExtensions;       // List of all file extension allowed for musique

#endif // CDEVICEMODELDEF_H
