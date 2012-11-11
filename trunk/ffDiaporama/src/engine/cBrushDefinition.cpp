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

#include "cBrushDefinition.h"

//============================================
// Global static
//============================================

cBackgroundList BackgroundList;

#define PI              3.14159265
QBrush  Transparent;                    // Transparent brush

//*********************************************************************************************************************************************

cAutoFramingDef AUTOFRAMINGDEF[NBR_AUTOFRAMING];

cAutoFramingDef::cAutoFramingDef(QString ToolTip,bool IsInternal,int GeometryType) {
    this->ToolTip       =ToolTip;
    this->GeometryType  =GeometryType;
    this->IsInternal    =IsInternal;
}

void AutoFramingDefInit() {
    for (int i=0;i<NBR_AUTOFRAMING;i++) switch(i) {
        case AUTOFRAMING_CUSTOMUNLOCK   :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom geometry - unlocked"),false,                 AUTOFRAMING_GEOMETRY_CUSTOM);   break;
        case AUTOFRAMING_CUSTOMLOCK     :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom geometry - locked"),false,                   AUTOFRAMING_GEOMETRY_CUSTOM);   break;
        case AUTOFRAMING_CUSTOMIMGLOCK  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom size - Image geometry"),false,               AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_CUSTOMPRJLOCK  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom size - Project geometry"),false,             AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_FULLMAX        :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Full image"),false,                                 AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_FULLMIN        :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Full inner image"),true,                            AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - to the left"),false,               AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - to the left"),true,          AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - in the center"),false,             AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - in the center"),true,        AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - to the right"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - to the right"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHTOPMAX    :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - at the top"),false,                 AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHTOPMIN    :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - at the top"),true,            AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - in the middle"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - in the middle"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - at the bottom"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - at the bottom"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
    }
}

//*********************************************************************************************************************************************
// Utility function to create a gradient brush
//*********************************************************************************************************************************************

QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:GetGradientBrush");

    QGradient Gradient;
    switch (GradientOrientation) {
        case GRADIENTORIENTATION_UPLEFT:        Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()));                                                                                             break;          // Up-Left
        case GRADIENTORIENTATION_UP:            Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()));                                                                            break;          // Up
        case GRADIENTORIENTATION_UPRIGHT:       Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()),QPointF(Rect.x(),Rect.y()+Rect.height()));                                                                                             break;          // Up-right
        case GRADIENTORIENTATION_LEFT:          Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()/2),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2));                                                                           break;          // Left
        case GRADIENTORIENTATION_RADIAL:        Gradient=QRadialGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2),Rect.width()>Rect.height()?Rect.width():Rect.height(),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2));    break;                                                                                                                                      // Radial
        case GRADIENTORIENTATION_RIGHT:         Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2),QPointF(Rect.x(),Rect.y()+Rect.height()/2));                                                                           break;          // Right
        case GRADIENTORIENTATION_BOTTOMLEFT:    Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width(),Rect.y()));                                                                                             break;          // bt-Left
        case GRADIENTORIENTATION_BOTTOM:        Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width()/2,Rect.y()));                                                                            break;          // bottom
        case GRADIENTORIENTATION_BOTTOMRIGHT:   Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()),QPointF(Rect.x(),Rect.y()));                                                                                             break;          // bt-right
    }
    Gradient.setColorAt(0,QColor(ColorD));
    Gradient.setColorAt(1,QColor(ColorF));
    if (BrushType==BRUSHTYPE_GRADIENT3) Gradient.setColorAt(Intermediate,QColor(ColorIntermed));
    return new QBrush(Gradient);
}

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cBackgroundObject::cBackgroundObject(QString FileName,int TheGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundObject::cBackgroundObject");

    IsValide    = false;
    FilePath    = FileName;
    Name        = QFileInfo(FileName).baseName();

    // Load file
    BackgroundImage.load(FilePath);

    // Make Icon
    QImage *BrushImage=new QImage(BackgroundImage.copy());
    if (!BrushImage->isNull()) {
        Geometry = TheGeometry;
        int     H,W;
        QImage  *NewImg;
        switch (Geometry) {
            case GEOMETRY_4_3 :
                H=BrushImage->height();
                W=int(double(4)*(double(H)/double(3)));
                NewImg=new QImage(BrushImage->copy((BrushImage->width()-W)/2,(BrushImage->height()-H)/2,W,H));
                delete BrushImage;
                BrushImage=NewImg;
                break;
            case GEOMETRY_40_17 :
                W=BrushImage->width();
                H=int(double(17)*(double(W)/double(40)));
                NewImg=new QImage(BrushImage->copy((BrushImage->width()-W)/2,(BrushImage->height()-H)/2,W,H));
                delete BrushImage;
                BrushImage=NewImg;
                break;
        }

        Icon=QPixmap(QPixmap::fromImage(BrushImage->scaledToHeight(64)));
        delete BrushImage;
    }
    IsValide=!Icon.isNull();
}

//*********************************************************************************************************************************************
// Global class containing background library
//*********************************************************************************************************************************************
cBackgroundList::cBackgroundList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::cBackgroundList");

    Geometry=-1;
}

//====================================================================================================================

void cBackgroundList::ScanDisk(QString Path,int TheGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::ScanDisk");

    if (Geometry==TheGeometry) return;
    Geometry=TheGeometry;

    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList();;

    List.clear();
    for (int i=0;i<Files.count();i++) if (Files[i].isFile() && ((QString(Files[i].suffix()).toLower()=="jpg")||(QString(Files[i].suffix()).toLower()=="png"))) {
        QString FileName=QFileInfo(Files[i]).absoluteFilePath();
        if (QFileInfo(QString(FileName)).isFile()) List.append(cBackgroundObject(Files[i].absoluteFilePath(),Geometry));
    }
}

//====================================================================================================================

int cBackgroundList::SearchImage(QString NameToFind) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::SearchImage");

    int Ret=-1;
    int j=0;
    while ((j<List.count())&&(Ret==-1)) if (List[j].Name==NameToFind) Ret=j; else j++;
    if (Ret==-1) Ret=0; // If not found : switch to first background
    return Ret;
}

//*********************************************************************************************************************************************
// Object for Brush definition
//*********************************************************************************************************************************************

cBrushDefinition::cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::cBrushDefinition");

    TypeComposition     =COMPOSITIONTYPE_BACKGROUND;
    BrushType           =BRUSHTYPE_SOLID;               // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors, 5=brush library, 6=image disk
    PatternType         =Qt::Dense4Pattern;             // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    ColorD              ="#C0C0C0";                     // First Color
    ColorIntermed       ="#777777";                     // Intermediate Color
    ColorF              ="#000000";                     // Last Color
    Intermediate        =0.1;                           // Intermediate position of 2nd color (in %)
    GradientOrientation =6;                             // 1=Up-Left, 2=Up, 3=Up-right, ...
    BrushImage          ="";                            // Image name if image from library
    Image               =NULL;
    Video               =NULL;
    SoundVolume         =1;                             // Volume of soundtrack
    Deinterlace         =false;                         // Add a YADIF filter to deinterlace video (on/off)
    ApplicationConfig   =TheApplicationConfig;
    BackgroundList      =TheBackgroundList;

    // Image correction part
    ImageRotation           =0;                             // Image rotation
    X                       =0;                             // X position (in %) relative to up/left corner
    Y                       =0;                             // Y position (in %) relative to up/left corner
    ZoomFactor              =1;                             // Zoom factor (in %)
    Brightness              =0;
    Contrast                =0;
    Gamma                   =1;
    Red                     =0;
    Green                   =0;
    Blue                    =0;
    LockGeometry            =false;
    AspectRatio             =1;
    FullFilling             =false;
    GaussBlurSharpenSigma   =0;
    BlurSharpenRadius       =5;
    QuickBlurSharpenSigma   =0;
    TypeBlurSharpen         =0; // 0=Quick, 1=Gaussian
    Desat                   =0;
    Swirl                   =0;
    Implode                 =0;
    WaveAmp                 =0;
    WaveFreq                =15;
    OnOffFilter             =0;
    ImageSpeedWave          =SPEEDWAVE_PROJECTDEFAULT;
}

//====================================================================================================================
cBrushDefinition::~cBrushDefinition() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::~cBrushDefinition");

    if (Image) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) delete Image;
        Image=NULL;
    }
    if (Video) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) delete Video;
        Video=NULL;
    }
}

//====================================================================================================================

QBrush *cBrushDefinition::GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetBrush");

    QBrush  *Br=NULL;

    switch (BrushType) {
        case BRUSHTYPE_NOBRUSH :        Br=new QBrush(Qt::NoBrush);                                                                                             break;
        case BRUSHTYPE_SOLID :          Br=new QBrush(QColor(ColorD),Qt::SolidPattern);                                                                         break;
        case BRUSHTYPE_PATTERN :        Br=new QBrush(QColor(ColorD),(Qt::BrushStyle)(PatternType+3));                                                          break;
        case BRUSHTYPE_GRADIENT2 :      Br=GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);                       break;
        case BRUSHTYPE_GRADIENT3 :      Br=GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);                       break;
        case BRUSHTYPE_IMAGELIBRARY :   Br=GetLibraryBrush(Rect);                                                                                               break;
        case BRUSHTYPE_IMAGEDISK :      Br=GetImageDiskBrush(Rect,PreviewMode,Position,StartPosToAdd,SoundTrackMontage,PctDone,PreviousBrush,UseBrushCache);    break;
        default :                       Br=new QBrush(Qt::NoBrush);                                                                                             break;
    }
    return Br;
}

//====================================================================================================================

QBrush *cBrushDefinition::GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetImageDiskBrush");

    // If not an image or a video or filename is empty then return
    if ((Image?Image->FileName:Video?Video->FileName:"")=="") return new QBrush(Qt::NoBrush);

    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((Rect.width()==0)&&(Rect.height()==0));

    if (!SoundOnly) {
        QImage *RenderImage=NULL;
        if (Video) {
            // Only slide dialog set UseBrushCache to true => use LuloImageCache to Cache rendered image
            if (UseBrushCache) {
                QImage                *LN_Image=NULL;
                cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(Video,Position+StartPosToAdd,(!PreviewMode || ApplicationConfig->Smoothing),true);
                if (PreviewMode) LN_Image=ImageObject->CachePreviewImage;
                    else         LN_Image=ImageObject->CacheRenderImage;
                if (LN_Image) RenderImage=new QImage(LN_Image->copy());
                    else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
                if (!LN_Image) {
                    if (PreviewMode)    ImageObject->CachePreviewImage=new QImage(RenderImage->copy());
                        else            ImageObject->CacheRenderImage=new QImage(RenderImage->copy());
                }
            } else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
        } else if (Image) RenderImage=Image->ImageAt(PreviewMode);

        QBrush *Ret=NULL;

        if (RenderImage) {
            if (FullFilling) {
                // Create brush image with distortion
                Ret=new QBrush(RenderImage->scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                delete RenderImage;
                RenderImage=NULL;
            } else {
                // Create brush image with ken burns effect !
                double  TheXFactor      =X;
                double  TheYFactor      =Y;
                double  TheZoomFactor   =ZoomFactor;
                double  TheRotateFactor =ImageRotation;
                double  TheBrightness   =Brightness;
                double  TheContrast     =((OnOffFilter & FilterNormalize)==0)?Contrast:0;
                double  TheGamma        =Gamma;
                double  TheRed          =Red;
                double  TheGreen        =Green;
                double  TheBlue         =Blue;
                double  TheDesat        =Desat;
                double  TheSwirl        =Swirl;
                double  TheImplode      =Implode;
                double  TheWaveAmp      =WaveAmp;
                double  TheWaveFreq     =WaveFreq;
                double  TheAspectRatio  =AspectRatio;
                int     TheOnOffFilter  =OnOffFilter;
                bool    ProgressifOnOffFilter=false;

                // Adjust values depending on PctDone and previous Filter (if exist)
                if (PreviousBrush) {
                    if (((PreviousBrush->OnOffFilter & FilterNormalize)==0)&&(PreviousBrush->Contrast!=TheContrast))    TheContrast     =PreviousBrush->Contrast+(TheContrast-PreviousBrush->Contrast)*PctDone;
                    if (PreviousBrush->AspectRatio!=TheAspectRatio)                                                     TheAspectRatio  =PreviousBrush->AspectRatio+(TheAspectRatio-PreviousBrush->AspectRatio)*PctDone;
                    if (PreviousBrush->X!=TheXFactor)                                                                   TheXFactor      =PreviousBrush->X+(TheXFactor-PreviousBrush->X)*PctDone;
                    if (PreviousBrush->Y!=TheYFactor)                                                                   TheYFactor      =PreviousBrush->Y+(TheYFactor-PreviousBrush->Y)*PctDone;
                    if (PreviousBrush->ZoomFactor!=TheZoomFactor)                                                       TheZoomFactor   =PreviousBrush->ZoomFactor+(TheZoomFactor-PreviousBrush->ZoomFactor)*PctDone;
                    if (PreviousBrush->ImageRotation!=TheRotateFactor)                                                  TheRotateFactor =PreviousBrush->ImageRotation+(TheRotateFactor-PreviousBrush->ImageRotation)*PctDone;
                    if (PreviousBrush->Brightness!=TheBrightness)                                                       TheBrightness   =PreviousBrush->Brightness+(TheBrightness-PreviousBrush->Brightness)*PctDone;
                    if (PreviousBrush->Gamma!=TheGamma)                                                                 TheGamma        =PreviousBrush->Gamma+(TheGamma-PreviousBrush->Gamma)*PctDone;
                    if (PreviousBrush->Red!=TheRed)                                                                     TheRed          =PreviousBrush->Red+(TheRed-PreviousBrush->Red)*PctDone;
                    if (PreviousBrush->Green!=TheGreen)                                                                 TheGreen        =PreviousBrush->Green+(TheGreen-PreviousBrush->Green)*PctDone;
                    if (PreviousBrush->Blue!=TheBlue)                                                                   TheBlue         =PreviousBrush->Blue+(TheBlue-PreviousBrush->Blue)*PctDone;
                    if (PreviousBrush->Desat!=TheDesat)                                                                 TheDesat        =PreviousBrush->Desat+(TheDesat-PreviousBrush->Desat)*PctDone;
                    if (PreviousBrush->Swirl!=TheSwirl)                                                                 TheSwirl        =PreviousBrush->Swirl+(TheSwirl-PreviousBrush->Swirl)*PctDone;
                    if (PreviousBrush->Implode!=TheImplode)                                                             TheImplode      =PreviousBrush->Implode+(TheImplode-PreviousBrush->Implode)*PctDone;
                    if (PreviousBrush->WaveAmp!=TheWaveAmp)                                                             TheWaveAmp      =PreviousBrush->WaveAmp+(TheWaveAmp-PreviousBrush->WaveAmp)*PctDone;
                    if (PreviousBrush->WaveFreq!=TheWaveFreq)                                                           TheWaveFreq     =PreviousBrush->WaveFreq+(TheWaveFreq-PreviousBrush->WaveFreq)*PctDone;

                    if ((PreviousBrush->OnOffFilter!=TheOnOffFilter)||
                        (PreviousBrush->GaussBlurSharpenSigma!=GaussBlurSharpenSigma)||
                        (PreviousBrush->QuickBlurSharpenSigma!=QuickBlurSharpenSigma)||
                        (PreviousBrush->BlurSharpenRadius!=BlurSharpenRadius)||
                        (PreviousBrush->TypeBlurSharpen!=TypeBlurSharpen)
                       )ProgressifOnOffFilter=true;
                }

                // Prepare values from sourceimage size
                double   RealImageW=RenderImage->width();
                double   RealImageH=RenderImage->height();
                double   Hyp       =round(sqrt(RealImageW*RealImageW+RealImageH*RealImageH));
                double   HypPixel  =Hyp*TheZoomFactor;

                // Expand canvas
                QImage   NewRenderImage(Hyp,Hyp,QImage::Format_ARGB32_Premultiplied);
                QPainter Painter;
                Painter.begin(&NewRenderImage);
                Painter.setCompositionMode(QPainter::CompositionMode_Source);
                Painter.fillRect(QRect(0,0,NewRenderImage.width(),NewRenderImage.height()),Qt::transparent);
                Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                Painter.drawImage(QPoint((Hyp-RealImageW)/2,(Hyp-RealImageH)/2),*RenderImage);
                Painter.end();
                delete RenderImage;

                // Rotate image (if needed)
                if (TheRotateFactor!=0) {
                    QTransform matrix;
                    matrix.rotate(TheRotateFactor,Qt::ZAxis);
                    int W=NewRenderImage.width();
                    int H=NewRenderImage.height();
                    NewRenderImage=NewRenderImage.transformed(matrix,ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);
                    int ax=NewRenderImage.width()-W;
                    int ay=NewRenderImage.height()-H;
                    NewRenderImage=NewRenderImage.copy(ax/2,ay/2,NewRenderImage.width()-ax,NewRenderImage.height()-ay);
                }

                // Get part we need and scaled it to destination size
                NewRenderImage=NewRenderImage.copy(Hyp*TheXFactor,Hyp*TheYFactor,HypPixel,HypPixel*TheAspectRatio)
                                    .scaled(Rect.width(),Rect.width()*TheAspectRatio,Qt::IgnoreAspectRatio,
                                    ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);

                // Apply correction filters to DestImage
                fmt_filters::image img(NewRenderImage.bits(),NewRenderImage.width(),NewRenderImage.height());
                if (TheBrightness!=0)                                           fmt_filters::brightness(img,TheBrightness);
                if (TheContrast!=0)                                             fmt_filters::contrast(img,TheContrast);
                if (TheGamma!=1)                                                fmt_filters::gamma(img,TheGamma);
                if ((TheRed!=0)||(TheGreen!=0)||(TheBlue!=0))                   fmt_filters::colorize(img,TheRed,TheGreen,TheBlue);
                if (TheDesat!=0)                                                Blitz::desaturate(NewRenderImage,TheDesat);
                if (TheSwirl!=0)                                                NewRenderImage=Blitz::swirl(NewRenderImage,-TheSwirl);
                if (TheImplode!=0)                                              NewRenderImage=Blitz::implode(NewRenderImage,TheImplode);
                if ((TheWaveAmp!=0)&&(TheWaveFreq!=0))                          NewRenderImage=Blitz::wave(NewRenderImage,NewRenderImage.height()*(TheWaveAmp/100),NewRenderImage.width()/(TheWaveFreq/10)).convertToFormat(QImage::Format_ARGB32_Premultiplied);

                if ((ProgressifOnOffFilter)||(GaussBlurSharpenSigma!=0)||(QuickBlurSharpenSigma!=0)||(OnOffFilter!=0)) {
                    QImage PreviousImage=NewRenderImage.copy();
                    // Apply previous filter to image
                    if ((TypeBlurSharpen==1)&&(GaussBlurSharpenSigma<0))        NewRenderImage=Blitz::gaussianBlur(NewRenderImage,BlurSharpenRadius,-GaussBlurSharpenSigma);
                    if ((TypeBlurSharpen==1)&&(GaussBlurSharpenSigma>0))        NewRenderImage=Blitz::gaussianSharpen(NewRenderImage,BlurSharpenRadius,GaussBlurSharpenSigma);
                    if ((TypeBlurSharpen==0)&&(QuickBlurSharpenSigma<0))        NewRenderImage=Blitz::blur(NewRenderImage,-QuickBlurSharpenSigma);
                    if ((TypeBlurSharpen==0)&&(QuickBlurSharpenSigma>0))        NewRenderImage=Blitz::sharpen(NewRenderImage,QuickBlurSharpenSigma);
                    if ((OnOffFilter & FilterDespeckle)!=0)                     Blitz::despeckle(NewRenderImage);
                    if ((OnOffFilter & FilterEqualize)!=0)                      Blitz::equalize(NewRenderImage);
                    if ((OnOffFilter & FilterGray)!=0)                          Blitz::grayscale(NewRenderImage,false);
                    if ((OnOffFilter & FilterNegative)!=0)                      Blitz::invert(NewRenderImage,QImage::InvertRgb);
                    if ((OnOffFilter & FilterEmboss)!=0)                        NewRenderImage=Blitz::emboss(NewRenderImage,0,1,Blitz::High);
                    if ((OnOffFilter & FilterEdge)!=0)                          NewRenderImage=Blitz::edge(NewRenderImage);
                    if ((OnOffFilter & FilterAntialias)!=0)                     NewRenderImage=Blitz::antialias(NewRenderImage);
                    if ((OnOffFilter & FilterNormalize)!=0)                     Blitz::normalize(NewRenderImage);
                    if ((OnOffFilter & FilterCharcoal)!=0)                      NewRenderImage=Blitz::charcoal(NewRenderImage);
                    if ((OnOffFilter & FilterOil)!=0)                           NewRenderImage=Blitz::oilPaint(NewRenderImage);

                    if (ProgressifOnOffFilter) {
                        // Apply previous filter to copied image
                        if ((PreviousBrush->TypeBlurSharpen==1)&&(PreviousBrush->GaussBlurSharpenSigma<0))    PreviousImage=Blitz::gaussianBlur(PreviousImage,PreviousBrush->BlurSharpenRadius,-PreviousBrush->GaussBlurSharpenSigma);
                        if ((PreviousBrush->TypeBlurSharpen==1)&&(PreviousBrush->GaussBlurSharpenSigma>0))    PreviousImage=Blitz::gaussianSharpen(PreviousImage,PreviousBrush->BlurSharpenRadius,PreviousBrush->GaussBlurSharpenSigma);
                        if ((PreviousBrush->TypeBlurSharpen==0)&&(PreviousBrush->QuickBlurSharpenSigma<0))    PreviousImage=Blitz::blur(PreviousImage,-PreviousBrush->QuickBlurSharpenSigma);
                        if ((PreviousBrush->TypeBlurSharpen==0)&&(PreviousBrush->QuickBlurSharpenSigma>0))    PreviousImage=Blitz::sharpen(PreviousImage,PreviousBrush->QuickBlurSharpenSigma);
                        if ((PreviousBrush->OnOffFilter & FilterDespeckle)!=0)  Blitz::despeckle(PreviousImage);
                        if ((PreviousBrush->OnOffFilter & FilterEqualize)!=0)   Blitz::equalize(PreviousImage);
                        if ((PreviousBrush->OnOffFilter & FilterGray)!=0)       Blitz::grayscale(PreviousImage,false);
                        if ((PreviousBrush->OnOffFilter & FilterNegative)!=0)   Blitz::invert(PreviousImage,QImage::InvertRgb);
                        if ((PreviousBrush->OnOffFilter & FilterEmboss)!=0)     PreviousImage=Blitz::emboss(PreviousImage,0,1,Blitz::High);
                        if ((PreviousBrush->OnOffFilter & FilterEdge)!=0)       PreviousImage=Blitz::edge(PreviousImage);
                        if ((PreviousBrush->OnOffFilter & FilterAntialias)!=0)  PreviousImage=Blitz::antialias(PreviousImage);
                        if ((PreviousBrush->OnOffFilter & FilterNormalize)!=0)  Blitz::normalize(PreviousImage);
                        if ((OnOffFilter & FilterCharcoal)!=0)                  PreviousImage=Blitz::charcoal(PreviousImage);
                        if ((OnOffFilter & FilterOil)!=0)                       PreviousImage=Blitz::oilPaint(PreviousImage);

                        // Mix images
                        QPainter P;
                        P.begin(&PreviousImage);
                        P.setOpacity(PctDone);
                        P.drawImage(0,0,NewRenderImage);
                        P.setOpacity(1);
                        P.end();
                        if (!PreviousImage.isNull())     Ret=new QBrush(PreviousImage.scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                    } else if (!NewRenderImage.isNull()) Ret=new QBrush(NewRenderImage.scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                } else if (!NewRenderImage.isNull())     Ret=new QBrush(NewRenderImage.scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            }
        }
        return Ret;
    } else {
        // Force loading of sound of video
        if (Video) {
            QImage *RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
            if (RenderImage) delete RenderImage;
        }
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================
// Note:This function is use only by cBrushDefinition !
QImage cBrushDefinition::ApplyFilter(QImage Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyFilter");

    if (Image.isNull()) return Image;
    fmt_filters::image img(Image.bits(),Image.width(),Image.height());
    if (Brightness!=0)                                          fmt_filters::brightness(img,Brightness);
    if ((Contrast!=0)&&((OnOffFilter & FilterNormalize)==0))    fmt_filters::contrast(img,Contrast);
    if (Gamma!=1)                                               fmt_filters::gamma(img,Gamma);
    if ((Red!=0)||(Green!=0)||(Blue!=0))                        fmt_filters::colorize(img,Red,Green,Blue);
    if ((TypeBlurSharpen==0)&&(QuickBlurSharpenSigma<0))        Image=Blitz::blur(Image,-QuickBlurSharpenSigma);
    if ((TypeBlurSharpen==0)&&(QuickBlurSharpenSigma>0))        Image=Blitz::sharpen(Image,QuickBlurSharpenSigma);
    if ((TypeBlurSharpen==1)&&(GaussBlurSharpenSigma<0))        Image=Blitz::gaussianBlur(Image,BlurSharpenRadius,-GaussBlurSharpenSigma);
    if ((TypeBlurSharpen==1)&&(GaussBlurSharpenSigma>0))        Image=Blitz::gaussianSharpen(Image,BlurSharpenRadius,GaussBlurSharpenSigma);
    if ((OnOffFilter & FilterDespeckle)!=0)                     Blitz::despeckle(Image);
    if ((OnOffFilter & FilterEqualize)!=0)                      Blitz::equalize(Image);
    if ((OnOffFilter & FilterGray)!=0)                          Blitz::grayscale(Image,false);
    if ((OnOffFilter & FilterNegative)!=0)                      Blitz::invert(Image,QImage::InvertRgb);
    if ((OnOffFilter & FilterEmboss)!=0)                        Image=Blitz::emboss(Image,0,1,Blitz::High);
    if ((OnOffFilter & FilterEdge)!=0)                          Image=Blitz::edge(Image);
    if ((OnOffFilter & FilterAntialias)!=0)                     Image=Blitz::antialias(Image);
    if ((OnOffFilter & FilterNormalize)!=0)                     Blitz::normalize(Image);
    if ((OnOffFilter & FilterCharcoal)!=0)                      Image=Blitz::charcoal(Image);
    if ((OnOffFilter & FilterOil)!=0)                           Image=Blitz::oilPaint(Image);
    if (Desat!=0)                                               Blitz::desaturate(Image,Desat);
    if (Swirl!=0)                                               Image=Blitz::swirl(Image,-Swirl);
    if (Implode!=0)                                             Image=Blitz::implode(Image,Implode);
    if ((WaveAmp!=0)&&(WaveFreq!=0))                            Image=Blitz::wave(Image,Image.height()*(WaveAmp/100),Image.width()/(WaveFreq/10));
    return Image;
}

//====================================================================================================================

QBrush *cBrushDefinition::GetLibraryBrush(QRectF Rect) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetLibraryBrush");

    if (!BackgroundList) return NULL;
    int BackgroundImageNumber=BackgroundList->SearchImage(BrushImage);
    if ((BackgroundImageNumber>=0)&&(BackgroundImageNumber<BackgroundList->List.count())) {
        double Ratio=double(BackgroundList->List[BackgroundImageNumber].BackgroundImage.height())/double(BackgroundList->List[BackgroundImageNumber].BackgroundImage.width());
        double H    =Rect.height()+1;
        double W    =H/Ratio;
        QImage NewImg1;
        if (W<(Rect.width()+1)) {
            NewImg1=QImage(BackgroundList->List[BackgroundImageNumber].BackgroundImage.scaledToWidth(Rect.width()+1,Qt::SmoothTransformation));
        } else {
            NewImg1=QImage(BackgroundList->List[BackgroundImageNumber].BackgroundImage.scaledToHeight(Rect.height()+1,Qt::SmoothTransformation));
        }
        W=NewImg1.width();
        H=GetHeightForWidth(W,Rect);
        if (H<NewImg1.height()) {
            H=NewImg1.height();
            W=GetWidthForHeight(H,Rect);
        }
        if ((W!=NewImg1.width())||(H!=NewImg1.height())) return new QBrush(QImage(NewImg1.copy(0,0,W,H))); else return new QBrush(NewImg1);
    } else return new QBrush(Qt::NoBrush);
}

//====================================================================================================================
// Return height for width depending on Rect geometry
int cBrushDefinition::GetHeightForWidth(int WantedWith,QRectF Rect) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetHeightForWidth");

    double   Ratio=Rect.width()/Rect.height();
    return int(double(double(WantedWith)/Ratio));
}

//====================================================================================================================
// Return width for height depending on Rect geometry
int cBrushDefinition::GetWidthForHeight(int WantedHeight,QRectF Rect) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetWidthForHeight");

    double   Ratio=Rect.height()/Rect.width();
    return int(double(double(WantedHeight)/Ratio));
}

//====================================================================================================================
// create a COMPOSITIONTYPE_SHOT brush as a copy of a given brush

void cBrushDefinition::CopyFromBrushDefinition(cBrushDefinition *BrushToCopy) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::CopyFromBrushDefinition");

    TypeComposition         =COMPOSITIONTYPE_SHOT;
    BrushType               =BrushToCopy->BrushType;
    PatternType             =BrushToCopy->PatternType;
    GradientOrientation     =BrushToCopy->GradientOrientation;
    ColorD                  =BrushToCopy->ColorD;
    ColorF                  =BrushToCopy->ColorF;
    ColorIntermed           =BrushToCopy->ColorIntermed;
    Intermediate            =BrushToCopy->Intermediate;
    BrushImage              =BrushToCopy->BrushImage;
    Image                   =BrushToCopy->Image;
    Video                   =BrushToCopy->Video;
    SoundVolume             =BrushToCopy->SoundVolume;
    Deinterlace             =BrushToCopy->Deinterlace;

    // Image correction part
    ImageRotation           =BrushToCopy->ImageRotation;
    X                       =BrushToCopy->X;
    Y                       =BrushToCopy->Y;
    ZoomFactor              =BrushToCopy->ZoomFactor;
    Brightness              =BrushToCopy->Brightness;
    Contrast                =BrushToCopy->Contrast;
    Gamma                   =BrushToCopy->Gamma;
    Red                     =BrushToCopy->Red;
    Green                   =BrushToCopy->Green;
    Blue                    =BrushToCopy->Blue;
    LockGeometry            =BrushToCopy->LockGeometry;
    FullFilling             =BrushToCopy->FullFilling;
    AspectRatio             =BrushToCopy->AspectRatio;
    GaussBlurSharpenSigma   =BrushToCopy->GaussBlurSharpenSigma;
    BlurSharpenRadius       =BrushToCopy->BlurSharpenRadius;
    QuickBlurSharpenSigma   =BrushToCopy->QuickBlurSharpenSigma;
    TypeBlurSharpen         =BrushToCopy->TypeBlurSharpen;
    Desat                   =BrushToCopy->Desat;
    Swirl                   =BrushToCopy->Swirl;
    Implode                 =BrushToCopy->Implode;
    WaveAmp                 =BrushToCopy->WaveAmp;
    WaveFreq                =BrushToCopy->WaveFreq;
    OnOffFilter             =BrushToCopy->OnOffFilter;
    ImageSpeedWave          =BrushToCopy->ImageSpeedWave;
}

//====================================================================================================================

void cBrushDefinition::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         BrushFileName=(Image?Image->FileName:Video?Video->FileName:"");

    if ((PathForRelativPath!="")&&((Image?Image->FileName:Video?Video->FileName:"")!="")) {
        if (ForceAbsolutPath)
            BrushFileName=QDir::cleanPath(QDir(QFileInfo(PathForRelativPath).absolutePath()).absoluteFilePath(BrushFileName));
        else
            BrushFileName=QDir::cleanPath(QDir(QFileInfo(PathForRelativPath).absolutePath()).relativeFilePath(BrushFileName));
    }
    // Attribut of the object
    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("BrushType",BrushType);                                                            // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    switch (BrushType) {
        case BRUSHTYPE_PATTERN      :
            Element.setAttribute("PatternType",PatternType);                                                // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
            Element.setAttribute("ColorD",ColorD);                                                          // First Color
            break;
        case BRUSHTYPE_GRADIENT3    :
            Element.setAttribute("ColorIntermed",ColorIntermed);                                            // Intermediate Color
            Element.setAttribute("Intermediate",Intermediate);                                              // Intermediate position of 2nd color (in %)
        case BRUSHTYPE_GRADIENT2    :
            Element.setAttribute("ColorF",ColorF);                                                          // Last Color
            Element.setAttribute("GradientOrientation",GradientOrientation);                                // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
        case BRUSHTYPE_SOLID        :
            Element.setAttribute("ColorD",ColorD);                                                          // First Color
            break;
        case BRUSHTYPE_IMAGELIBRARY :
            Element.setAttribute("BrushImage",BrushImage);                                                  // Image name if image from library
            break;
        case BRUSHTYPE_IMAGEDISK :
            if (Video!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Element.setAttribute("StartPos",Video->StartPos.toString("HH:mm:ss.zzz"));              // Start position (video only)
                    Element.setAttribute("EndPos",Video->EndPos.toString("HH:mm:ss.zzz"));                  // End position (video only)
                } else {
                    Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));               // Volume of soundtrack (for video only)
                    Element.setAttribute("Deinterlace",Deinterlace?"1":0);                                  // Add a YADIF filter to deinterlace video (on/off) (for video only)
                }
            } else if (Image!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Element.setAttribute("ImageOrientation",Image->ImageOrientation);
                }
            }
            break;
    }

    // Image correction part
    QDomElement CorrectElement=DomDocument.createElement("ImageCorrection");
    CorrectElement.setAttribute("X",                    X);                 // X position (in %) relative to up/left corner
    CorrectElement.setAttribute("Y",                    Y);                 // Y position (in %) relative to up/left corner
    CorrectElement.setAttribute("ZoomFactor",           ZoomFactor);        // Zoom factor (in %)
    CorrectElement.setAttribute("ImageRotation",        ImageRotation);     // Image rotation (in °)
    CorrectElement.setAttribute("Brightness",           Brightness);
    CorrectElement.setAttribute("Contrast",             Contrast);
    CorrectElement.setAttribute("Gamma",                Gamma);
    CorrectElement.setAttribute("Red",                  Red);
    CorrectElement.setAttribute("Green",                Green);
    CorrectElement.setAttribute("Blue",                 Blue);
    CorrectElement.setAttribute("LockGeometry",         LockGeometry?1:0);
    CorrectElement.setAttribute("AspectRatio",          AspectRatio);
    CorrectElement.setAttribute("FullFilling",          FullFilling?1:0);
    CorrectElement.setAttribute("TypeBlurSharpen",      TypeBlurSharpen);
    CorrectElement.setAttribute("GaussBlurSharpenSigma",GaussBlurSharpenSigma);
    CorrectElement.setAttribute("BlurSharpenRadius",    BlurSharpenRadius);
    CorrectElement.setAttribute("QuickBlurSharpenSigma",QuickBlurSharpenSigma);
    CorrectElement.setAttribute("Desat",                Desat);
    CorrectElement.setAttribute("Swirl",                Swirl);
    CorrectElement.setAttribute("Implode",              Implode);
    CorrectElement.setAttribute("WaveAmp",              WaveAmp);
    CorrectElement.setAttribute("WaveFreq",             WaveFreq);
    CorrectElement.setAttribute("OnOffFilter",          OnOffFilter);
    CorrectElement.setAttribute("ImageSpeedWave",       ImageSpeedWave);
    Element.appendChild(CorrectElement);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::LoadFromXML");

    if (ModifyFlag) *ModifyFlag=false;
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Attribut of the object
        TypeComposition =Element.attribute("TypeComposition").toInt();
        BrushType       =Element.attribute("BrushType").toInt();                         // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
        switch (BrushType) {
            case BRUSHTYPE_PATTERN      :
                PatternType         =Element.attribute("PatternType").toInt();          // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
                ColorD              =Element.attribute("ColorD");                       // First Color
                break;
            case BRUSHTYPE_GRADIENT3    :
                ColorIntermed       =Element.attribute("ColorIntermed");                // Intermediate Color
                Intermediate        =Element.attribute("Intermediate").toDouble();      // Intermediate position of 2nd color (in %)
            case BRUSHTYPE_GRADIENT2    :
                ColorF              =Element.attribute("ColorF");                       // Last Color
                GradientOrientation =Element.attribute("GradientOrientation").toInt();  // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
            case BRUSHTYPE_SOLID        :
                ColorD=Element.attribute("ColorD");                                    // First Color
                break;
            case BRUSHTYPE_IMAGELIBRARY :
                BrushImage=Element.attribute("BrushImage");  // Image name if image from library
                break;
            case BRUSHTYPE_IMAGEDISK :
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {
                    QString BrushFileName=Element.attribute("BrushFileName");                       // File name if image from disk
                    if ((PathForRelativPath!="")&&(BrushFileName!=""))
                        BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(BrushFileName));

                    bool IsValide=false;
                    QString Extension=QFileInfo(BrushFileName).suffix().toLower();
                    for (int i=0;i<ApplicationConfig->AllowImageExtension.count();i++) if (ApplicationConfig->AllowImageExtension[i]==Extension) {
                        Image=new cImageFile(ApplicationConfig);
                        Image->ImageOrientation=Element.attribute("ImageOrientation").toInt();
                        IsValide=Image->GetInformationFromFile(BrushFileName,AliasList,ModifyFlag);
                        if (!IsValide) {
                            delete Image;
                            Image=NULL;
                        }
                        break;
                    }
                    if (Image==NULL) for (int i=0;i<ApplicationConfig->AllowVideoExtension.count();i++) if (ApplicationConfig->AllowVideoExtension[i]==Extension) {
                        Video=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);
                        IsValide=Video->GetInformationFromFile(BrushFileName,AliasList,ModifyFlag);
                        if (!IsValide) {
                            delete Video;
                            Video=NULL;
                        }
                        if (Video!=NULL) Video->OpenCodecAndFile();
                        break;
                    }
                }
                if (Video!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        Video->StartPos =QTime().fromString(Element.attribute("StartPos"));                         // Start position (video only)
                        Video->EndPos   =QTime().fromString(Element.attribute("EndPos"));                           // End position (video only)
                    } else {
                        SoundVolume=Element.attribute("SoundVolume").toDouble();                                    // Volume of soundtrack (for video only)
                        Deinterlace=Element.attribute("Deinterlace")=="1";                                          // Add a YADIF filter to deinterlace video (on/off) (for video only)
                    }
                } else if (Image!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        // Old Image transformation (for compatibility with version prio to 1.5)
                        if ((Element.elementsByTagName("ImageTransformation").length()>0)&&(Element.elementsByTagName("ImageTransformation").item(0).isElement()==true)) {
                            QDomElement SubElement=Element.elementsByTagName("ImageTransformation").item(0).toElement();
                            if (SubElement.hasAttribute("BlurSigma"))      GaussBlurSharpenSigma=  SubElement.attribute("BlurSigma").toDouble();
                            if (SubElement.hasAttribute("BlurRadius"))     BlurSharpenRadius= SubElement.attribute("BlurRadius").toDouble();
                            if (SubElement.hasAttribute("OnOffFilter"))    OnOffFilter=SubElement.attribute("OnOffFilter").toInt();
                            if (GaussBlurSharpenSigma!=0) TypeBlurSharpen=1;
                        }
                    }
                }
                break;
        }

        // Image correction part
        if ((Element.elementsByTagName("ImageCorrection").length()>0)&&(Element.elementsByTagName("ImageCorrection").item(0).isElement()==true)) {
            QDomElement CorrectElement=Element.elementsByTagName("ImageCorrection").item(0).toElement();

            if (CorrectElement.hasAttribute("X"))                       X                       =CorrectElement.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
            if (CorrectElement.hasAttribute("Y"))                       Y                       =CorrectElement.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
            if (CorrectElement.hasAttribute("ZoomFactor"))              ZoomFactor              =CorrectElement.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
            if (CorrectElement.hasAttribute("ImageRotation"))           ImageRotation           =CorrectElement.attribute("ImageRotation").toDouble();          // Image rotation (in °)
            if (CorrectElement.hasAttribute("Brightness"))              Brightness              =CorrectElement.attribute("Brightness").toInt();
            if (CorrectElement.hasAttribute("Contrast"))                Contrast                =CorrectElement.attribute("Contrast").toInt();
            if (CorrectElement.hasAttribute("Gamma"))                   Gamma                   =CorrectElement.attribute("Gamma").toDouble();
            if (CorrectElement.hasAttribute("Red"))                     Red                     =CorrectElement.attribute("Red").toInt();
            if (CorrectElement.hasAttribute("Green"))                   Green                   =CorrectElement.attribute("Green").toInt();
            if (CorrectElement.hasAttribute("Blue"))                    Blue                    =CorrectElement.attribute("Blue").toInt();
            if (CorrectElement.hasAttribute("AspectRatio"))             AspectRatio             =CorrectElement.attribute("AspectRatio").toDouble();
            if (CorrectElement.hasAttribute("FullFilling"))             FullFilling             =CorrectElement.attribute("FullFilling").toInt()==1;
            if (CorrectElement.hasAttribute("TypeBlurSharpen"))         TypeBlurSharpen         =CorrectElement.attribute("TypeBlurSharpen").toDouble();
            if (CorrectElement.hasAttribute("GaussBlurSharpenSigma"))   GaussBlurSharpenSigma   =CorrectElement.attribute("GaussBlurSharpenSigma").toDouble();
            if (CorrectElement.hasAttribute("BlurSharpenRadius"))       BlurSharpenRadius       =CorrectElement.attribute("BlurSharpenRadius").toDouble();
            if (CorrectElement.hasAttribute("QuickBlurSharpenSigma"))   QuickBlurSharpenSigma   =CorrectElement.attribute("QuickBlurSharpenSigma").toInt();
            if (CorrectElement.hasAttribute("Desat"))                   Desat                   =CorrectElement.attribute("Desat").toDouble();
            if (CorrectElement.hasAttribute("Swirl"))                   Swirl                   =CorrectElement.attribute("Swirl").toDouble();
            if (CorrectElement.hasAttribute("Implode"))                 Implode                 =CorrectElement.attribute("Implode").toDouble();
            if (CorrectElement.hasAttribute("WaveAmp"))                 WaveAmp                 =CorrectElement.attribute("WaveAmp").toDouble();
            if (CorrectElement.hasAttribute("WaveFreq"))                WaveFreq                =CorrectElement.attribute("WaveFreq").toDouble();
            if (CorrectElement.hasAttribute("OnOffFilter"))             OnOffFilter             =CorrectElement.attribute("OnOffFilter").toInt();
            if (CorrectElement.hasAttribute("ImageSpeedWave"))          ImageSpeedWave          =CorrectElement.attribute("ImageSpeedWave").toInt();

            // If old ImageGeometry value in project file then compute LockGeometry
            if (CorrectElement.hasAttribute("ImageGeometry"))           LockGeometry=(CorrectElement.attribute("ImageGeometry").toInt()!=2);
                else if (CorrectElement.hasAttribute("LockGeometry"))   LockGeometry=(CorrectElement.attribute("LockGeometry").toInt()==1); // Else load saved value

        }

        return (BrushType==BRUSHTYPE_IMAGEDISK)?(Image!=NULL)||(Video!=NULL):true;
    }
    return false;
}

//====================================================================================================================

int cBrushDefinition::GetImageType() {
    int ImageType=IMAGETYPE_UNKNOWN;
    if (Image!=NULL) {
        if ((Image->ImageWidth>1080)&&(Image->ImageHeight>1080)) ImageType=IMAGETYPE_PHOTOLANDSCAPE; else ImageType=IMAGETYPE_CLIPARTLANDSCAPE;
        if ((qreal(Image->ImageWidth)/qreal(Image->ImageHeight))<1) ImageType++;
    } else if (Video!=NULL) {
        ImageType=IMAGETYPE_VIDEOLANDSCAPE;
        if ((qreal(Video->ImageWidth)/qreal(Video->ImageHeight))<1) ImageType++;
    }
    return ImageType;
}

//====================================================================================================================

void cBrushDefinition::ApplyMaskToImageToWorkspace(QImage *SrcImage,QRectF CurSelRect,int BackgroundForm,int AutoFramingStyle) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyMaskToImageToWorkspace");
    // Create shape mask
    int RowHeight=SrcImage->width();
    QImage Image(RowHeight,RowHeight,QImage::Format_ARGB32_Premultiplied);
    QPainter PainterImg;
    PainterImg.begin(&Image);
    PainterImg.setPen(Qt::NoPen);
    PainterImg.fillRect(QRect(0,0,RowHeight,RowHeight),QBrush(0x555555));
    PainterImg.setBrush(Qt::transparent);
    PainterImg.setCompositionMode(QPainter::CompositionMode_Source);
    QList<QPolygonF> List=ComputePolygon(BackgroundForm,CurSelRect.left(),CurSelRect.top(),CurSelRect.width(),CurSelRect.height());
    for (int i=0;i<List.count();i++) PainterImg.drawPolygon(List.at(i));
    PainterImg.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PainterImg.end();

    // Apply mask to workspace image
    PainterImg.begin(SrcImage);
    PainterImg.setOpacity(0.75);
    PainterImg.drawImage(0,0,Image);
    PainterImg.setOpacity(1);
    PainterImg.end();

    // Add Icon (if wanted)
    if ((AutoFramingStyle>=0)&&(AutoFramingStyle<NBR_AUTOFRAMING)) {
        QImage IconGeoImage;
        switch (AUTOFRAMINGDEF[AutoFramingStyle].GeometryType) {
            case AUTOFRAMING_GEOMETRY_CUSTOM :  IconGeoImage=AutoFramingStyle==AUTOFRAMING_CUSTOMUNLOCK?QImage(AUTOFRAMING_ICON_GEOMETRY_UNLOCKED):
                                                                                                        QImage(AUTOFRAMING_ICON_GEOMETRY_LOCKED);       break;
            case AUTOFRAMING_GEOMETRY_PROJECT : IconGeoImage=QImage(AUTOFRAMING_ICON_GEOMETRY_PROJECT);                                                 break;
            case AUTOFRAMING_GEOMETRY_IMAGE :   IconGeoImage=QImage(AUTOFRAMING_ICON_GEOMETRY_IMAGE);                                                   break;
        }
        QPainter P;
        P.begin(SrcImage);
        P.drawImage(SrcImage->width()-IconGeoImage.width()-2,SrcImage->height()-IconGeoImage.height()-2,IconGeoImage);
        P.end();
    }
}

void cBrushDefinition::ApplyMaskToImageToWorkspace(QImage *SrcImage,int AutoFramingStyle,int BackgroundForm,int WantedSize,qreal maxw,qreal maxh,qreal minw,qreal minh,qreal X,qreal Y,qreal ZoomFactor,qreal AspectRatio,qreal ProjectGeometry) {
    QRectF CurSelRect;
    switch (AutoFramingStyle) {
        case AUTOFRAMING_CUSTOMUNLOCK   :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*AspectRatio-1);                   break;
        case AUTOFRAMING_CUSTOMLOCK     :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*AspectRatio-1);                   break;
        case AUTOFRAMING_CUSTOMIMGLOCK  :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*(maxh/maxw)-1);                   break;
        case AUTOFRAMING_CUSTOMPRJLOCK  :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*ProjectGeometry-1);               break;
        case AUTOFRAMING_FULLMAX        :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxw-1,maxh-1);                                                   break;
        case AUTOFRAMING_FULLMIN        :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minw-1,minh-1);                                                   break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1);                                   break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1);                                   break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   CurSelRect=QRectF((WantedSize-(maxh/ProjectGeometry))/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1);                 break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   CurSelRect=QRectF((WantedSize-(minh/ProjectGeometry))/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1);                 break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   CurSelRect=QRectF(WantedSize-(maxh/ProjectGeometry)-(WantedSize-maxw)/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1); break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   CurSelRect=QRectF(WantedSize-(minh/ProjectGeometry)-(WantedSize-minw)/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1); break;
        case AUTOFRAMING_WIDTHTOPMAX    :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxw-1,maxw*ProjectGeometry-1);                                   break;
        case AUTOFRAMING_WIDTHTOPMIN    :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minw-1,minw*ProjectGeometry-1);                                   break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-(maxw*ProjectGeometry))/2,maxw-1,maxw*ProjectGeometry-1);                 break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-(minw*ProjectGeometry))/2,minw-1,minw*ProjectGeometry-1);                 break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   CurSelRect=QRectF((WantedSize-maxw)/2,WantedSize-(maxw*ProjectGeometry)-(WantedSize-maxh)/2,maxw-1,maxw*ProjectGeometry-1); break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   CurSelRect=QRectF((WantedSize-minw)/2,WantedSize-(minw*ProjectGeometry)-(WantedSize-minh)/2,minw-1,minw*ProjectGeometry-1); break;
        default : return;
    }
    ApplyMaskToImageToWorkspace(SrcImage,CurSelRect,BackgroundForm,AutoFramingStyle);
}

int cBrushDefinition::GetCurrentFramingStyle(qreal ProjectGeometry) {
    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return -1;

    int aX=X*dmax;
    int aY=Y*dmax;
    int aW=dmax*ZoomFactor;
    int aH=dmax*ZoomFactor*AspectRatio;

    if (!LockGeometry)                                                                                                                                                      return AUTOFRAMING_CUSTOMUNLOCK;
    if      ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxw))                &&(aH==int(maxh)))                   return AUTOFRAMING_FULLMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minw))                &&(aH==int(minh)))                   return AUTOFRAMING_FULLMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTLEFTMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTLEFTMIN;
    else if ((aX==int((dmax-(maxh/ProjectGeometry))/2))          &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTMIDLEMAX;
    else if ((aX==int((dmax-(minh/ProjectGeometry))/2))          &&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTMIDLEMIN;
    else if ((aX==int(dmax-(maxh/ProjectGeometry)-(dmax-maxw)/2))&&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTRIGHTMAX;
    else if ((aX==int(dmax-(minh/ProjectGeometry)-(dmax-minw)/2))&&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTRIGHTMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHTOPMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHTOPMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-(maxw*ProjectGeometry))/2))             &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHMIDLEMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-(minw*ProjectGeometry))/2))             &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHMIDLEMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int(dmax-(maxw*ProjectGeometry)-(dmax-maxh)/2))   &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHBOTTOMMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int(dmax-(minw*ProjectGeometry)-(dmax-minh)/2))   &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHBOTTOMMIN;
    else if (AspectRatio==(maxh/maxw))                                                                                                                                                          return AUTOFRAMING_CUSTOMIMGLOCK;
    else if (AspectRatio==ProjectGeometry)                                                                                                                                                      return AUTOFRAMING_CUSTOMPRJLOCK;
    else                                                                                                                                                                                        return AUTOFRAMING_CUSTOMLOCK;
}

//====================================================================================================================

QImage *cBrushDefinition::ImageToWorkspace(QImage *SrcImage,int WantedSize,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ImageToWorkspace");
    QImage  *RetImage   =NULL;
    QImage  *SourceImage=NULL;
    qreal   Hyp         =sqrt(qreal(SrcImage->width())*qreal(SrcImage->width())+qreal(SrcImage->height())*qreal(SrcImage->height()));   // Calc hypothenuse of the image to define full canvas
    qreal   DstX,DstY,DstW,DstH;

    // calc rectangle before rotation
    qreal  rx=qreal(SrcImage->width())*(WantedSize/Hyp)/2;
    qreal  ry=qreal(SrcImage->height())*(WantedSize/Hyp)/2;

    //RotatePoint.X = ((Pt.X - Centre.X) * Cos(AngCrad) - (Pt.Y - Centre.Y) * Sin(AngCrad) + Centre.X)
    //RotatePoint.Y = ((Pt.X - Centre.X) * Sin(AngCrad) + (Pt.Y - Centre.Y) * Cos(AngCrad) + Centre.Y)

    qreal  xtab[4],ytab[4];
    xtab[0]=-rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[1]=+rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[2]=-rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[3]=+rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+WantedSize/2;
    ytab[0]=-rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[1]=+rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[2]=-rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[3]=+rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+WantedSize/2;

    // Sort xtab and ytab
    for (int i=0;i<4;i++) for (int j=0;j<3;j++) {
        if (xtab[j]>xtab[j+1]) {    qreal a=xtab[j+1];  xtab[j+1]=xtab[j];  xtab[j]=a;  }
        if (ytab[j]>ytab[j+1]) {    qreal a=ytab[j+1];  ytab[j+1]=ytab[j];  ytab[j]=a;  }
    }
    maxw=xtab[3]-xtab[0];   minw=xtab[2]-xtab[1];
    maxh=ytab[3]-ytab[0];   minh=ytab[2]-ytab[1];

    // Rotate image if needed and create a SourceImage
    if (ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(SrcImage->transformed(matrix,Qt::SmoothTransformation));

    // If no rotation then SourceImage=SrcImage
    } else SourceImage=SrcImage;

    // Calc coordinates of the part in the source image
    qreal  RealImageW=qreal(SourceImage->width());                  // Get real image widht
    qreal  RealImageH=qreal(SourceImage->height());                 // Get real image height

    DstX=((Hyp-RealImageW)/2)*(WantedSize/Hyp);
    DstY=((Hyp-RealImageH)/2)*(WantedSize/Hyp);
    DstW=RealImageW*(WantedSize/Hyp);
    DstH=RealImageH*(WantedSize/Hyp);

    QImage  ToUseImage=SourceImage->scaled(DstW,DstH);
    if (SourceImage!=SrcImage) delete SourceImage;

    if (ToUseImage.format()!=QImage::Format_ARGB32_Premultiplied) ToUseImage=ToUseImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    ToUseImage=ApplyFilter(ToUseImage);

    RetImage=new QImage(WantedSize,WantedSize,QImage::Format_ARGB32_Premultiplied);
    QPainter P;
    P.begin(RetImage);
    P.fillRect(QRectF(0,0,WantedSize,WantedSize),Transparent);
    P.drawImage(QRectF(DstX,DstY,DstW,DstH),ToUseImage,QRectF(0,0,DstW,DstH));
    P.end();

    x1=xtab[0]; y1=ytab[0];
    x2=xtab[1]; y2=ytab[1];
    x3=xtab[2]; y3=ytab[2];
    x4=xtab[3]; y4=ytab[3];

    return RetImage;
}

//====================================================================================================================

bool cBrushDefinition::CalcWorkspace(qreal &dmax,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::CalcWorkspace");

    int     ImgWidth =Image?Image->ImageWidth:Video?Video->ImageWidth:0;
    int     ImgHeight=Image?Image->ImageHeight:Video?Video->ImageHeight:0;

    if ((Video!=NULL)&&(ImgWidth==1920)&&(ImgHeight=1088)&&(Video->ApplicationConfig->Crop1088To1080)) ImgHeight=1080;

    if ((ImgWidth==0)||(ImgHeight==0)) return false;

    dmax=sqrt(qreal(ImgWidth)*qreal(ImgWidth)+qreal(ImgHeight)*qreal(ImgHeight));   // Calc hypothenuse of the image to define full canvas

    // calc rectangle before rotation
    qreal  rx=qreal(ImgWidth)/2;
    qreal  ry=qreal(ImgHeight)/2;

    qreal  xtab[4],ytab[4];
    xtab[0]=-rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[1]=+rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[2]=-rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[3]=+rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+dmax/2;
    ytab[0]=-rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[1]=+rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[2]=-rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[3]=+rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+dmax/2;

    // Sort xtab and ytab
    for (int i=0;i<4;i++) for (int j=0;j<3;j++) {
        if (xtab[j]>xtab[j+1]) {    qreal a=xtab[j+1];  xtab[j+1]=xtab[j];  xtab[j]=a;  }
        if (ytab[j]>ytab[j+1]) {    qreal a=ytab[j+1];  ytab[j+1]=ytab[j];  ytab[j]=a;  }
    }
    maxw=xtab[3]-xtab[0];   minw=xtab[2]-xtab[1];
    maxh=ytab[3]-ytab[0];   minh=ytab[2]-ytab[1];
    x1=xtab[0];             y1=ytab[0];
    x2=xtab[1];             y2=ytab[1];
    x3=xtab[2];             y3=ytab[2];
    x4=xtab[3];             y4=ytab[3];

    return true;
}

//====================================================================================================================

void cBrushDefinition::ApplyAutoFraming(int AutoAdjust,qreal ProjectGeometry) {
    switch (AutoAdjust) {
        case AUTOFRAMING_FULLMAX        :   s_AdjustWH();                           break;
        case AUTOFRAMING_FULLMIN        :   s_AdjustMinWH();                        break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   s_AdjustHLeft(ProjectGeometry);         break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   s_AdjustMinHLeft(ProjectGeometry);      break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   s_AdjustHMidle(ProjectGeometry);        break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   s_AdjustMinHMidle(ProjectGeometry);     break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   s_AdjustHRight(ProjectGeometry);        break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   s_AdjustMinHRight(ProjectGeometry);     break;
        case AUTOFRAMING_WIDTHTOPMAX    :   s_AdjustWTop(ProjectGeometry);          break;
        case AUTOFRAMING_WIDTHTOPMIN    :   s_AdjustMinWTop(ProjectGeometry);       break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   s_AdjustWMidle(ProjectGeometry);        break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   s_AdjustMinWMidle(ProjectGeometry);     break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   s_AdjustWBottom(ProjectGeometry);       break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   s_AdjustMinWBottom(ProjectGeometry);    break;
    }
}

void cBrushDefinition::s_AdjustWTop(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWTop");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    //qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-maxh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWTop(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWTop");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    //qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-minh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustWMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustWBottom(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWBottom");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=(dmax-H-(dmax-maxh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWBottom(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWBottom");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=(dmax-H-(dmax-minh)/2)/dmax;
    ZoomFactor=W/dmax;
}

//====================================================================================================================

void cBrushDefinition::s_AdjustHLeft(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHLeft");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=((dmax-maxw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHLeft(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHLeft");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=((dmax-minw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustHMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustHRight(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHRight");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=(dmax-W-(dmax-maxw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHRight(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHRight");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=(dmax-W-(dmax-minw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

//====================================================================================================================

void cBrushDefinition::s_AdjustWH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWH");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =maxh/maxw;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWH");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =minh/minw;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}
