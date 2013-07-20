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

#include "../DlgffDPjrProperties/DlgffDPjrProperties.h"

#include "DlgRenderVideo.h"
#include "ui_DlgRenderVideo.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFutureWatcher>

//====================================================================================================================
// Because of MSVC ....
//====================================================================================================================

AVRational MakeAVRational(int num,int den) {
    AVRational AVR;
    AVR.num=num;
    AVR.den=den;
    return AVR;
}

//====================================================================================================================

DlgRenderVideo::DlgRenderVideo(cDiaporama &TheDiaporama,int TheExportMode,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgRenderVideo) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DlgRenderVideo");

    ui->setupUi(this);
    OkBt                =ui->OkBt;
    CancelBt            =ui->CancelBt;
    HelpTT              =ui->HelpTT;
    Diaporama           =&TheDiaporama;
    ExportMode          =TheExportMode;
    IsDestFileOpen      =false;
    StopSpinboxRecursion=false;
    Language            =Diaporama->ProjectInfo->DefaultLanguage;
}

//====================================================================================================================

DlgRenderVideo::~DlgRenderVideo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::~DlgRenderVideo");

    delete ui;
}

//====================================================================================================================

void DlgRenderVideo::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DoInitDialog");

    switch (ExportMode) {
        case EXPORTMODE_ADVANCED :  setWindowTitle(QApplication::translate("DlgRenderVideo","Advanced render video"));                          break;
        case MODE_LOSSLESS       :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render lossless video"));                          break;
        case MODE_SMARTPHONE     :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for mobil player and smartphone"));   break;
        case MODE_MULTIMEDIASYS  :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for multimedia system"));             break;
        case MODE_FORTHEWEB      :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for the WEB"));                       break;
        case MODE_SOUNDTRACK     :  setWindowTitle(QApplication::translate("DlgRenderVideo","Export soundtrack"));                              break;
    }

    // For the first step, only SelectDestWidget is display and enable
    ui->SelectDestScroll->setEnabled(true);     ui->SelectDestScroll->setVisible(true);
    ui->ProcessWidget->setEnabled(false);       ui->ProcessWidget->setVisible(false);

    QString FolderProject=Diaporama->ProjectFileName;
    if (FolderProject!="") {
        FolderProject=QFileInfo(FolderProject).absolutePath();
        if (FolderProject.endsWith(QDir::separator())) {
            FolderProject=QFileInfo(Diaporama->ProjectFileName).baseName();
        } else if (FolderProject.lastIndexOf(QDir::separator())>0) FolderProject=FolderProject.mid(FolderProject.lastIndexOf(QDir::separator())+1);
    }

    // Output file
    VideoCodec          = Diaporama->ApplicationConfig->DefaultVideoCodec;
    VideoFrameRate      = 25;
    VideoBitRate        = Diaporama->ApplicationConfig->DefaultVideoBitRate;
    ImageSize           = Diaporama->ApplicationConfig->DefaultImageSize;
    Standard            = Diaporama->ApplicationConfig->DefaultStandard;

    if (ExportMode==MODE_SOUNDTRACK) {
        OutputFileName=Diaporama->ApplicationConfig->LastMusicPath+
            (Diaporama->ApplicationConfig->LastMusicPath.endsWith(QDir::separator())?"":QString(QDir::separator()))+
            (Diaporama->ProjectFileName!=""?
                 (Diaporama->ApplicationConfig->DefaultNameProjectName==1?QFileInfo(Diaporama->ProjectFileName).baseName():
                 (((Diaporama->ApplicationConfig->DefaultNameProjectName==2)&&(Diaporama->ProjectInfo->Title!=""))?Diaporama->ProjectInfo->Title:FolderProject)):
                 QApplication::translate("DlgRenderVideo","soundtrack","Default name for rendering"));
        OutputFileFormat    = Diaporama->ApplicationConfig->DefaultSoundtrackFormat;
        AudioCodec          = Diaporama->ApplicationConfig->DefaultSoundtrackAudioCodec;
        AudioFrequency      = Diaporama->ApplicationConfig->DefaultSoundtrackFreq;
        AudioBitRate        = Diaporama->ApplicationConfig->DefaultSoundtrackBitRate;
    } else {
        OutputFileName=Diaporama->ApplicationConfig->LastRenderVideoPath+
            (Diaporama->ApplicationConfig->LastRenderVideoPath.endsWith(QDir::separator())?"":QString(QDir::separator()))+
            (Diaporama->ProjectFileName!=""?
                 (Diaporama->ApplicationConfig->DefaultNameProjectName==1?QFileInfo(Diaporama->ProjectFileName).baseName():
                 (((Diaporama->ApplicationConfig->DefaultNameProjectName==2)&&(Diaporama->ProjectInfo->Title!=""))?Diaporama->ProjectInfo->Title:FolderProject)):
                 QApplication::translate("DlgRenderVideo","movie","Default name for rendering"));
        OutputFileFormat    = Diaporama->ApplicationConfig->DefaultFormat;
        AudioCodec          = Diaporama->ApplicationConfig->DefaultAudioCodec;
        AudioFrequency      = Diaporama->ApplicationConfig->DefaultFreq;
        AudioBitRate        = Diaporama->ApplicationConfig->DefaultAudioBitRate;
    }

    ui->IncludeSoundCB->setChecked(true);
    connect(ui->IncludeSoundCB,SIGNAL(clicked()),this,SLOT(s_IncludeSound()));

    ui->ExportThumbCB->setChecked(Diaporama->ApplicationConfig->DefaultExportThumbnail);

    if (ExportMode==EXPORTMODE_ADVANCED) {

        ui->DeviceTypeLabel->setVisible(false);
        ui->DeviceTypeCB->setVisible(false);
        ui->DeviceModelCB->setVisible(false);
        ui->DeviceModelLabel->setVisible(false);
        ui->RenderFormatText->setVisible(false);
        //ui->VideoFormatCB->view()->setFixedWidth(400);
        //ui->AudioFormatCB->view()->setFixedWidth(400);

        // Init format container combo
        for (int i=0;i<VFORMAT_NBR;i++) if (FORMATDEF[i].IsFind) {
            ui->FileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
            if (i==OutputFileFormat) {
                ui->FileFormatCB->setCurrentIndex(ui->FileFormatCB->count()-1);
                FileFormatCombo(i);
            }
        }

        connect(ui->FileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));

        // Video standard & geometry
        ui->GeometryCombo->setEnabled(false);
        ui->GeometryCombo->setCurrentIndex(Diaporama->ImageGeometry);
        ui->StandardCombo->setCurrentIndex(Diaporama->ApplicationConfig->DefaultStandard);
        connect(ui->GeometryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));
        connect(ui->StandardCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));

        // Image size
        InitImageSizeCombo(0);
        ui->ImageSizeCombo->setCurrentIndex(ui->ImageSizeCombo->findText(DefImageFormat[Diaporama->ApplicationConfig->DefaultStandard][Diaporama->ImageGeometry][Diaporama->ApplicationConfig->DefaultImageSize].Name));
        connect(ui->ImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));

        // codec(s) & bitrate(s)
        FileFormatCombo(0);
        InitVideoBitRateCB(-1);
        InitAudioBitRateCB(-1);

        ui->AudioFormatLabel->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioFormatCB->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioBitRateLabel->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioBitRateCB->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioFreqLabel->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioFreqLabel2->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioFreqCB->setEnabled(ui->IncludeSoundCB->isChecked());

        connect(ui->VideoFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
        connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitAudioBitRateCB(int)));

    } else if (ExportMode==MODE_SOUNDTRACK) {

        ui->IncludeSoundCB->setVisible(false);
        ui->DeviceTypeLabel->setVisible(false);                         ui->DeviceTypeCB->setVisible(false);
        ui->DeviceModelLabel->setVisible(false);                        ui->DeviceModelCB->setVisible(false);
        ui->RenderFormatText->setVisible(false);                        ui->SoundLabel->setVisible(false);
        ui->GeometryLabel->setVisible(false);                           ui->GeometryCombo->setVisible(false);
        ui->StandardLabel->setVisible(false);                           ui->StandardCombo->setVisible(false);
        ui->VideoFormatLabel->setVisible(false);                        ui->VideoFormatCB->setVisible(false);
        ui->VideoBitRateLabel->setVisible(false);                       ui->VideoBitRateCB->setVisible(false);
        ui->ImageSizeLabel->setVisible(false);                          ui->ImageSizeCombo->setVisible(false);

        // Init format container combo
        for (int i=0;i<NBR_AFORMAT;i++) if (AUDIOFORMATDEF[i].IsFind) {
            ui->FileFormatCB->addItem(AUDIOFORMATDEF[i].LongName,QVariant(i));
            if (i==OutputFileFormat) {
                ui->FileFormatCB->setCurrentIndex(ui->FileFormatCB->count()-1);
                FileFormatCombo(i);
            }
        }

        connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitAudioBitRateCB(int)));
        connect(ui->FileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));

        // codec(s) & bitrate(s)
        FileFormatCombo(0);
        InitAudioBitRateCB(-1);

    } else {
        ui->FileFormatLabel->setVisible(false);                         ui->FileFormatCB->setVisible(false);
        ui->GeometryLabel->setVisible(false);                           ui->GeometryCombo->setVisible(false);
        ui->StandardLabel->setVisible(false);                           ui->StandardCombo->setVisible(false);
        ui->VideoFormatLabel->setVisible(false);                        ui->VideoFormatCB->setVisible(false);
        ui->VideoBitRateLabel->setVisible(false);                       ui->VideoBitRateCB->setVisible(false);

        ui->AudioFormatLabel->setVisible(false);                        ui->AudioFormatLabel->setEnabled(false);
        ui->AudioFormatCB->setVisible(false);                           ui->AudioFormatCB->setEnabled(false);
        ui->AudioBitRateLabel->setVisible(false);                       ui->AudioBitRateLabel->setEnabled(false);
        ui->AudioBitRateCB->setVisible(false);                          ui->AudioBitRateCB->setEnabled(false);
        ui->AudioFreqLabel->setVisible(false);                          ui->AudioFreqLabel->setEnabled(false);
        ui->AudioFreqLabel2->setVisible(false);                         ui->AudioFreqLabel2->setEnabled(false);
        ui->AudioFreqCB->setVisible(false);                             ui->AudioFreqCB->setEnabled(false);

        QStringList List;
        int         Default=ExportMode==MODE_SMARTPHONE?Diaporama->ApplicationConfig->DefaultSmartphoneType:
                            ExportMode==MODE_MULTIMEDIASYS?Diaporama->ApplicationConfig->DefaultMultimediaType:
                            ExportMode==MODE_FORTHEWEB?Diaporama->ApplicationConfig->DefaultForTheWEBType:
                            Diaporama->ApplicationConfig->DefaultLossLess;
        if (ExportMode!=MODE_LOSSLESS) {
            ui->ImageSizeLabel->setVisible(false);
            ui->ImageSizeCombo->setVisible(false);
            ui->DeviceTypeLabel->setVisible(true);
            ui->DeviceTypeCB->setVisible(true);
            ui->DeviceModelCB->setVisible(true);
            ui->DeviceModelLabel->setVisible(true);
            for (int i=0;i<Diaporama->ApplicationConfig->DeviceModelList.TranslatedRenderSubtype[ExportMode].count();i++) List.append(Diaporama->ApplicationConfig->DeviceModelList.TranslatedRenderSubtype[ExportMode][i]+"#"+QString("%1").arg(i));
            List.sort();
            for (int i=0;i<List.count();i++) {
                QString Item=List[i];
                int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
                Item=Item.left(Item.lastIndexOf("#"));
                ui->DeviceTypeCB->addItem(Item,QVariant(ItemData));
                if (Default==ItemData)  ui->DeviceTypeCB->setCurrentIndex(i);
            }

            s_DeviceTypeCB(0);
            s_DeviceModelCB(0);
            connect(ui->DeviceTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceTypeCB(int)));
            connect(ui->DeviceModelCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceModelCB(int)));
        } else {
            ui->ImageSizeLabel->setVisible(true);
            ui->ImageSizeCombo->setVisible(true);
            ui->DeviceTypeLabel->setVisible(false);
            ui->DeviceTypeCB->setVisible(false);
            ui->DeviceModelCB->setVisible(false);
            ui->DeviceModelLabel->setVisible(false);
            InitImageSizeCombo(0);
            ui->ImageSizeCombo->setCurrentIndex(ui->ImageSizeCombo->findText(DefImageFormat[Diaporama->ApplicationConfig->DefaultStandard][Diaporama->ImageGeometry][Diaporama->ApplicationConfig->DefaultLossLess].Name));
            InitVideoBitRateCB(0);
            connect(ui->ImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
        }
    }

    ui->LanguageED->setText(Language);
    ui->RenderZoneAllBt->setChecked(true);
    ui->RenderZoneFromBt->setChecked(false);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneFromED->setRange(1,Diaporama->List.count());
    ui->RenderZoneFromED->setValue(1);
    ui->RenderZoneToED->setRange(1,Diaporama->List.count());
    ui->RenderZoneToED->setValue(Diaporama->List.count());
    connect(ui->RenderZoneAllBt,SIGNAL(clicked()),this,SLOT(SetZoneToAll()));
    connect(ui->RenderZoneFromBt,SIGNAL(clicked()),this,SLOT(SetZoneToPartial()));

    ui->DestinationFilePath->setText(OutputFileName);
    AdjustDestinationFile();
    connect(ui->DestinationFilePathBT,SIGNAL(clicked()),this,SLOT(SelectDestinationFile()));
    connect(ui->DestinationFilePath,SIGNAL(editingFinished()),this,SLOT(AdjustDestinationFile()));

    connect(ui->ProjectPropertiesBt,SIGNAL(clicked()),this,SLOT(ProjectProperties()));
}

//====================================================================================================================

void DlgRenderVideo::ProjectProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::ProjectProperties");

    DlgffDPjrProperties Dlg(false,Diaporama,BaseApplicationConfig,BaseApplicationConfig->DlgffDPjrPropertiesWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) emit SetModifyFlag();
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SetZoneToAll");

    ui->RenderZoneAllBt->setChecked(true);
    ui->RenderZoneFromBt->setChecked(false);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToPartial() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SetZoneToPartial");

    ui->RenderZoneAllBt->setChecked(false);
    ui->RenderZoneFromBt->setChecked(true);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================
// Create and sort List

QStringList DlgRenderVideo::StringToSortedStringList(QString String) {
    QStringList StringList;
    QString     SubString;
    while (String.length()>0) {
        int Index=String.indexOf("#");
        if (Index>0) {
            SubString=String.left(Index);
            String=String.right(String.length()-Index-1);
        } else {
            SubString=String;
            String="";
        }
        StringList.append(SubString);
    }
    for (int i=0;i<StringList.count();i++) for (int j=0;j<StringList.count()-1;j++) {
        QString NameA=StringList[j];      if (NameA.endsWith("k")) NameA=NameA.left(NameA.length()-1);
        double  NumA=NameA.toDouble();
        QString NameB=StringList[j+1];    if (NameB.endsWith("k")) NameB=NameB.left(NameB.length()-1);
        double  NumB=NameB.toDouble();
        if (NumA>NumB) StringList.swap(j,j+1);
    }
    return StringList;
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceTypeCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_DeviceTypeCB");

    ui->DeviceModelCB->clear();
    int ItemData=ui->DeviceTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->DeviceTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count();i++)
        if ((Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceType==ExportMode)&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceSubtype==ItemData))
            List.append(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName);
    List.sort();
    ui->DeviceModelCB->addItems(List);
    int i=0;
    int ToFind=(ExportMode==MODE_SMARTPHONE)?Diaporama->ApplicationConfig->DefaultSmartphoneModel:
               (ExportMode==MODE_MULTIMEDIASYS)?Diaporama->ApplicationConfig->DefaultMultimediaModel:
               Diaporama->ApplicationConfig->DefaultForTheWEBModel;
    while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceIndex!=ToFind)) i++;
    if (Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceIndex==ToFind)
        ui->DeviceModelCB->setCurrentIndex(ui->DeviceModelCB->findText(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName));
}

//====================================================================================================================

void DlgRenderVideo::InitImageSizeCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitImageSizeCombo");
    int Geometry=(ExportMode!=MODE_LOSSLESS?ui->GeometryCombo->currentIndex():Diaporama->ImageGeometry);
    int Standard=(ExportMode!=MODE_LOSSLESS?ui->StandardCombo->currentIndex():Diaporama->ApplicationConfig->DefaultStandard);
    int ImageSize=ui->ImageSizeCombo->currentIndex();
    ui->ImageSizeCombo->clear();
    QStringList List;
    for (int i=0;i<NBR_SIZEDEF;i++) if (ORDERIMAGENAME[Standard][i]!=0) List.append(QString("%1:%2#####%3").arg(DefImageFormat[Standard][Geometry][i].Name).arg(ORDERIMAGENAME[Standard][i]).arg(i));
    // Sort List
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) {
        QString StrA=List[j].mid(List[j].lastIndexOf(":")+1);       StrA=StrA.left(StrA.indexOf("#"));
        QString StrB=List[j+1].mid(List[j+1].lastIndexOf(":")+1);   StrB=StrB.left(StrB.indexOf("#"));
        if (StrA.toInt()>StrB.toInt()) List.swap(j,j+1);
    }
    // Fill combo
    for (int i=0;i<List.count();i++) {
        QString Codec=List[i].left(List[i].indexOf("#####")); Codec=Codec.left(Codec.lastIndexOf(":"));
        int Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
        ui->ImageSizeCombo->addItem(Codec,QVariant(Index));
    }
    ui->ImageSizeCombo->setCurrentIndex(ImageSize);
}

//====================================================================================================================

void DlgRenderVideo::SelectDestinationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SelectDestinationFile");

    QString FileFormat;
    if (ExportMode==EXPORTMODE_ADVANCED) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=QString(FORMATDEF[OutputFileFormat].LongName)+" (*."+QString(FORMATDEF[OutputFileFormat].FileExtension)+")";
    } else if (ExportMode==MODE_SOUNDTRACK) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=QString(AUDIOFORMATDEF[OutputFileFormat].LongName)+" (*."+QString(AUDIOFORMATDEF[OutputFileFormat].FileExtension)+")";
    } else {
        if (ExportMode==MODE_LOSSLESS) {
            // For Lossless, OutputFileFormat is 2 : matroska
            FileFormat=QString(FORMATDEF[2].LongName)+" (*."+QString(FORMATDEF[2].FileExtension)+")";
        } else {
            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].LongName)+" (*."+
                        QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].FileExtension)+")";
            }
        }
    }
    QString OutputFileName  =QFileDialog::getSaveFileName(this,QApplication::translate("DlgRenderVideo","Select destination file"),ui->DestinationFilePath->text(),FileFormat);
    if (OutputFileName!="") {
        if (ExportMode==MODE_SOUNDTRACK)    Diaporama->ApplicationConfig->LastMusicPath=QFileInfo(OutputFileName).dir().absolutePath();
            else                            Diaporama->ApplicationConfig->LastRenderVideoPath=QFileInfo(OutputFileName).dir().absolutePath();
        ui->DestinationFilePath->setText(OutputFileName);
        AdjustDestinationFile();
    }
}

//====================================================================================================================

void DlgRenderVideo::FileFormatCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::FileFormatCombo");

    AdjustDestinationFile();

    ui->VideoFormatCB->clear();
    ui->AudioFormatCB->clear();

    //********* Video codec part
    int         CurrentFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
    QString     AllowedCodec;
    QString     Codec="";
    QString     ToSelect="";
    QStringList List;
    int         Index=0;

    if (ExportMode==EXPORTMODE_ADVANCED) {
        AllowedCodec=FORMATDEF[CurrentFormat].PossibleVideoCodec;
        while (AllowedCodec.length()>0) {
            Index=AllowedCodec.indexOf("#");
            if (Index>0) {
                Codec=AllowedCodec.left(Index);
                AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
            } else {
                Codec=AllowedCodec;
                AllowedCodec="";
            }
            // Now find index of this codec in the VIDEOCODECDEF
            Index=0;
            while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].FFD_VCODECST))) Index++;
            if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) {
                List.append(QString("%1#####%2").arg(VIDEOCODECDEF[Index].LongName).arg(Index));
                if (Codec==QString(VideoCodec)) ToSelect=QString(VIDEOCODECDEF[Index].LongName);
            }
        }
        // Sort List
        for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) if (List[j]>List[j+1]) List.swap(j,j+1);
        // Fill combo
        for (int i=0;i<List.count();i++) {
            Codec=List[i].left(List[i].indexOf("#####"));
            Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
            ui->VideoFormatCB->addItem(Codec,QVariant(Index));
        }
        // Set current selection
        if (ui->VideoFormatCB->findText(ToSelect)!=-1) ui->VideoFormatCB->setCurrentIndex(ui->VideoFormatCB->findText(ToSelect));
            else ui->VideoFormatCB->setCurrentIndex(0);
        ui->VideoFormatCB->setEnabled(ui->VideoFormatCB->count()>1);

        AllowedCodec=FORMATDEF[CurrentFormat].PossibleAudioCodec;

    } else if (ExportMode==MODE_SOUNDTRACK) {

        AllowedCodec=AUDIOFORMATDEF[CurrentFormat].PossibleAudioCodec;
    }

    //********* Audio codec part
    Codec="";
    Index=0;
    ToSelect="";
    List.clear();
    while (AllowedCodec.length()>0) {
        Index=AllowedCodec.indexOf("#");
        if (Index>0) {
            Codec=AllowedCodec.left(Index);
            AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
        } else {
            Codec=AllowedCodec;
            AllowedCodec="";
        }
        // Now find index of this codec in the AUDIOCODECDEF
        Index=0;
        while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
        if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) {
            List.append(QString("%1#####%2").arg(AUDIOCODECDEF[Index].LongName).arg(Index));
            if (Codec==QString(AudioCodec)) ToSelect=QString(AUDIOCODECDEF[Index].LongName);
        }
    }

    // Sort List
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) if (List[j]>List[j+1]) List.swap(j,j+1);
    // Fill combo
    for (int i=0;i<List.count();i++) {
        Codec=List[i].left(List[i].indexOf("#####"));
        Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
        ui->AudioFormatCB->addItem(Codec,QVariant(Index));
    }
    // Set current selection
    if (ui->AudioFormatCB->findText(ToSelect)!=-1) ui->AudioFormatCB->setCurrentIndex(ui->AudioFormatCB->findText(ToSelect));
        else ui->AudioFormatCB->setCurrentIndex(0);
    ui->AudioFormatCB->setEnabled((ui->AudioFormatCB->count()>1)&&(ui->IncludeSoundCB->isChecked()));
}

//====================================================================================================================

void DlgRenderVideo::InitVideoBitRateCB(int ChangeIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitVideoBitRateCB");

    if (ExportMode==MODE_LOSSLESS) {
        QString Text="Format=\tMKV\nVideo=\tx264 lossless";
        int ImgSize=ImageSize=ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
        int ExtendH =0;
        int ExtendV =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Extend;
        int W       =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Width;
        int H       =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Height;
        Text=Text+QString("-%1").arg(W)+"x"+QString("%1").arg(H);
        if (ExtendH>0) Text=Text+"+PADLEFT:"+QString("%1").arg(ExtendH/2)+"+PADRIGHT:"+QString("%1").arg(ExtendH-ExtendH/2);
        if (ExtendV>0) Text=Text+"+PADTOP:"+QString("%1").arg(ExtendV/2)+"+PADBOTTOM:"+QString("%1").arg(ExtendV-ExtendV/2);
        if (ui->IncludeSoundCB->isChecked()) Text=Text+"\nAudio=\tFLAC";
        ui->RenderFormatText->setText(Text);
        AdjustDestinationFile();
    } else {
        ui->VideoBitRateCB->clear();
        int Standard    =ui->StandardCombo->currentIndex();
        int CurrentCodec=ui->VideoFormatCB->currentIndex();
        int CurrentSize =ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
        if (CurrentCodec>=0) {
            CurrentCodec=ui->VideoFormatCB->itemData(CurrentCodec).toInt();

            bool        IsFindBitRate=false;
            QStringList List=StringToSortedStringList(VIDEOCODECDEF[CurrentCodec].PossibleBitrate);

            for (int i=0;i<List.count();i++) {
                ui->VideoBitRateCB->addItem(List[i]);
                if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(VideoBitRate))) {
                    ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->count()-1);
                    IsFindBitRate=true;
                }
            }
            if (!IsFindBitRate) ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->findText(VIDEOCODECDEF[CurrentCodec].DefaultBitrate[Standard][CurrentSize]));
            ui->VideoBitRateCB->setEnabled(ui->VideoBitRateCB->count()>1);
        } else ui->VideoBitRateCB->setEnabled(false);
    }
}

//====================================================================================================================

void DlgRenderVideo::InitAudioBitRateCB(int ChangeIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitAudioBitRateCB");

    ui->AudioBitRateCB->clear();
    ui->AudioFreqCB->clear();

    int CurrentCodec=ui->AudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->AudioFormatCB->itemData(CurrentCodec).toInt();

        int         CurrentFormat    =ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        sFormatDef  *CurrentContainer=(ExportMode==MODE_SOUNDTRACK?&AUDIOFORMATDEF[CurrentFormat]:&FORMATDEF[CurrentFormat]);
        QStringList ListBitRate      =StringToSortedStringList(AUDIOCODECDEF[CurrentCodec].PossibleBitrate2CH);
        QStringList ListFreq         =StringToSortedStringList(AUDIOCODECDEF[CurrentCodec].PossibleFrequency);
        QStringList ListAllowedFreq  =StringToSortedStringList(CurrentContainer->PossibleFrequency);

        bool        IsFindFreq=false;
        bool        IsFindBitRate=false;

        // Fill AudioBitRateCB
        for (int i=0;i<ListBitRate.count();i++) {
            ui->AudioBitRateCB->addItem(ListBitRate[i]);
            if ((ChangeIndex==-1)&&(ListBitRate[i]==QString("%1k").arg(AudioBitRate))) {
                ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->count()-1);
                IsFindBitRate=true;
            }
        }
        if (!IsFindBitRate) ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(AUDIOCODECDEF[CurrentCodec].Default));
        ui->AudioBitRateCB->setEnabled((ui->AudioBitRateCB->count()>1)&&(ui->IncludeSoundCB->isChecked()));

        // Fill AudioFreqCB (only if freq in ListFreq and in ListFreqAllowed)
        for (int i=0;i<ListFreq.count();i++) if (ListAllowedFreq.indexOf(ListFreq[i])!=-1) {
            ui->AudioFreqCB->addItem(ListFreq[i]);
            if ((ChangeIndex==-1)&&(ListFreq[i]==QString("%1").arg(AudioFrequency))) {
                ui->AudioFreqCB->setCurrentIndex(ui->AudioFreqCB->count()-1);
                IsFindFreq=true;
            }
        }
        if (!IsFindFreq) ui->AudioFreqCB->setCurrentIndex(ui->AudioFreqCB->findText(CurrentContainer->DefaultAudioFreq));
        if ((ui->AudioFreqCB->currentIndex()==-1)&&(ui->AudioFreqCB->count()>0)) ui->AudioFreqCB->setCurrentIndex(0);
        ui->AudioFreqCB->setEnabled((ui->AudioFreqCB->count()>0)&&(ui->IncludeSoundCB->isChecked()));
    } else {
        ui->AudioBitRateCB->setEnabled(false);
        ui->AudioFreqCB->setEnabled(false);
    }
}

//====================================================================================================================

void DlgRenderVideo::AdjustDestinationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::AdjustDestinationFile");

    QString FileFormat;
    if (ExportMode==EXPORTMODE_ADVANCED) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=FORMATDEF[OutputFileFormat].FileExtension;
    } else if (ExportMode==MODE_SOUNDTRACK) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=AUDIOFORMATDEF[OutputFileFormat].FileExtension;
    } else {
        if (ExportMode!=MODE_LOSSLESS) {
            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].FileExtension);
            }
        } else {
            FileFormat="mkv";
        }
    }
    QString OutputFileName   =ui->DestinationFilePath->text();

    OutputFileName=QFileInfo(OutputFileName).absoluteFilePath();
    if (QFileInfo(OutputFileName).suffix().length()>0) OutputFileName=OutputFileName.left(OutputFileName.length()-QFileInfo(OutputFileName).suffix().length()-1);
    if (OutputFileName.endsWith(".")) OutputFileName=OutputFileName.left(OutputFileName.length()-1);
    OutputFileName=AdjustDirForOS(OutputFileName+"."+FileFormat);
    ui->DestinationFilePath->setText(OutputFileName);
}

//====================================================================================================================

void DlgRenderVideo::reject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::reject");

    if (IsDestFileOpen) {
        Encoder.StopProcessWanted=true;
        ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Stop rendering"));
    } else QCustomDialog::reject();
}

//====================================================================================================================

void DlgRenderVideo::s_IncludeSound() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_IncludeSound");

    ui->AudioFormatLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFormatCB->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateCB->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFreqLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFreqLabel2->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFreqCB->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioLanguageLabel1->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioLanguageLabel2->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->LanguageED->setEnabled(ui->IncludeSoundCB->isChecked());
    if (ExportMode==MODE_LOSSLESS) InitVideoBitRateCB(0);
        else if (ExportMode!=EXPORTMODE_ADVANCED) s_DeviceModelCB(ui->DeviceModelCB->currentIndex());
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceModelCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_DeviceModelCB");

    QString Device=ui->DeviceModelCB->currentText();
    int i=0;
    while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
    if ((Device!="")&&(i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())) {
        int Standard=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->Standard;
        QString Text="Format=\t"+QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].LongName)+"\nVideo=\t";
        Text=Text+VIDEOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoCodec].LongName;
        int ImgSize=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->ImageSize;
        int ExtendV   =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Extend*2;
        int W         =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Width;
        int H         =DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Height;
        Text=Text+QString("-%1").arg(W)+"x"+QString("%1").arg(H);
        if (ExtendV>0) Text=Text+"+PADTOP:"+QString("%1").arg(ExtendV/2)+"+PADBOTTOM:"+QString("%1").arg(ExtendV-ExtendV/2);

        QString FPS=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==24?"24 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==25?"25 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==24000L/1001L?"23.98 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==30000L/1001L?"29.97 FPS":
                    "";

        QString VideoBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        Text=Text+"-"+FPS+"-"+VideoBitRateStr+"b/s";
        if (ui->IncludeSoundCB->isChecked()) {
            int AudioFreq=48000;
            int AudioChannels=2;
            if (AUDIOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec].Codec_id==AV_CODEC_ID_AMR_WB) {
                AudioChannels=1;
                AudioFreq=16000;
            } else if (AUDIOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec].Codec_id==AV_CODEC_ID_AMR_NB) {
                AudioChannels=1;
                AudioFreq=8000;
            } else if (QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].ShortName)==QString("flv")) {
                AudioFreq=44100;   // Special case for FLV
            }

            Text=Text+"\nAudio=\t"+AUDIOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec].LongName+"-"+AudioBitRateStr+"b/s"
                    +QString(" - %1 Hz ").arg(AudioFreq)+(AudioChannels==2?" - Stereo":" - Mono");
        }
        ui->RenderFormatText->setText(Text);

    } else ui->RenderFormatText->setText("");
    AdjustDestinationFile();
}

//====================================================================================================================

bool DlgRenderVideo::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DoAccept");

    if (IsDestFileOpen) {
        Encoder.StopProcessWanted=true;
        ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Stop rendering"));
    } else {
        int FromSlide=(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneFromED->value()-1:0;
        int ToSlide  =(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneToED->value()-1:Diaporama->List.count()-1;
        if (FromSlide>ToSlide) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Range selection"),
                QApplication::translate("DlgRenderVideo","Slide range is defined to incorrect values"));
            ui->RenderZoneToED->setFocus();
            return false;
        }

        Language=ui->LanguageED->text();
        if (Language=="") Language="und";
        if (Language.length()!=3) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Language selection"),
                QApplication::translate("DlgRenderVideo","Language must be empty or an ISO 639 language code (3 characters)\nSee help for more details!"));
            ui->LanguageED->setFocus();
            return false;
        }

        if (OutputFileName !=ui->DestinationFilePath->text()) OutputFileName=ui->DestinationFilePath->text();

        if ((QFileInfo(OutputFileName).exists())&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgRenderVideo","Overwrite file ?"),
            QApplication::translate("DlgRenderVideo","The file you selected already exist.\nDo you want to overwrite it ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)) return false;

        // Get values from controls
        AudioChannels    =2;
        AudioFrequency   =48000;
        VideoCodecIndex =-1;
        AudioCodecIndex =-1;

        if (ExportMode==EXPORTMODE_ADVANCED) {
            Standard =ui->StandardCombo->currentIndex();
            ImageSize=ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();

            OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
            VideoFrameRate  =DefImageFormat[ui->StandardCombo->currentIndex()][ui->GeometryCombo->currentIndex()][ImageSize].dFPS;
            // Video codec
            VideoCodecIndex=ui->VideoFormatCB->currentIndex();
            if (VideoCodecIndex<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Video codec error!");
                return false;
            }
            VideoCodecIndex=ui->VideoFormatCB->itemData(VideoCodecIndex).toInt();
            QString BitRate=ui->VideoBitRateCB->currentText();
            if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1)+"000";
            VideoBitRate=BitRate.toInt();

            // Audio codec
            AudioCodecIndex=ui->AudioFormatCB->currentIndex();
            if (AudioCodecIndex<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec error!");
                return false;
            }
            AudioCodecIndex=ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
            BitRate=ui->AudioBitRateCB->currentText();
            if (BitRate.endsWith("k")) {
                if (BitRate.contains(".")) {
                    BitRate=BitRate.left(BitRate.length()-1);
                    double Value=BitRate.toDouble()*1000;
                    BitRate=QString("%1").arg(int(Value));
                } else BitRate=BitRate.left(BitRate.length()-1)+"000";
            }
            AudioBitRate=BitRate.toInt();
            if (ui->AudioFreqCB->currentText()!="") AudioFrequency=ui->AudioFreqCB->currentText().toInt();

        } else if (ExportMode==MODE_SOUNDTRACK) {

            OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
            // Audio codec
            AudioCodecIndex=ui->AudioFormatCB->currentIndex();
            if (AudioCodecIndex<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec error!");
                return false;
            }
            AudioCodecIndex=ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
            QString BitRate=ui->AudioBitRateCB->currentText();
            if (BitRate.endsWith("k")) {
                if (BitRate.contains(".")) {
                    BitRate=BitRate.left(BitRate.length()-1);
                    double Value=BitRate.toDouble()*1000;
                    BitRate=QString("%1").arg(int(Value));
                } else BitRate=BitRate.left(BitRate.length()-1)+"000";
            }
            AudioBitRate=BitRate.toInt();
            if (ui->AudioFreqCB->currentText()!="") AudioFrequency=ui->AudioFreqCB->currentText().toInt();

        } else if (ExportMode==MODE_LOSSLESS) {

            Standard        =Diaporama->ApplicationConfig->DefaultStandard;
            ImageSize       =ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
            VideoFrameRate  =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].dFPS;
            OutputFileFormat=2;             // MKV
            VideoCodecIndex =8;             // x264 lossless
            AudioCodecIndex =7;             // FLAC
            VideoBitRate    =0;             // Not use
            AudioBitRate    =0;             // Not use

        } else {

            OutputFileFormat=0;
            QString Device=ui->DeviceModelCB->currentText();
            if (Device=="") {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Device model selection"),
                    QApplication::translate("DlgRenderVideo","A device model is require!"));
                ui->DeviceModelCB->setFocus();
                return false;
            }
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                Standard        =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->Standard;
                OutputFileFormat=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat;
                VideoCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoCodec;
                AudioCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec;
                ImageSize       =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->ImageSize;
                VideoFrameRate  =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].dFPS;
                VideoBitRate    =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate;
                AudioBitRate    =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate;
                AudioFrequency  =48000;
                AudioChannels   =2;
                if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==AV_CODEC_ID_AMR_WB) {
                    AudioChannels =1;
                    AudioFrequency=16000;
                } else if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==AV_CODEC_ID_AMR_NB) {
                    AudioChannels =1;
                    AudioFrequency=8000;
                } else if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("flv")) {
                    AudioFrequency=44100;   // Special case for FLV
                }
            }
        }

        if ((VideoFrameRate>=29.96)&&(VideoFrameRate<=29.98))    VideoFrameRate=29.97;              // Manual rounded
        if (VideoBitRate==0) VideoBitRate=DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Width*DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Height*100/2;

        if (ExportMode==MODE_SOUNDTRACK) {

            //QString FileFormat  =FORMATDEF[OutputFileFormat].FileExtension;
            AudioCodec=AUDIOCODECDEF[AudioCodecIndex].ShortName;

            // Special case adjustment
            if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==AV_CODEC_ID_PCM_S16LE) AudioBitRate=AudioFrequency*16*AudioChannels;      // Special case for WAV

        } else {

            //QString FileFormat  =FORMATDEF[OutputFileFormat].FileExtension;
            VideoCodec=VIDEOCODECDEF[VideoCodecIndex].FFD_VCODECST;
            AudioCodec=AUDIOCODECDEF[AudioCodecIndex].ShortName;

            // Special case adjustment
            if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("flv")) AudioFrequency=44100;   // Special case for FLV
            if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==AV_CODEC_ID_PCM_S16LE) AudioBitRate=AudioFrequency*16*AudioChannels;      // Special case for WAV
        }

        // Ensure codec combination is correct
        int  Capabilities=(ExportMode==MODE_SOUNDTRACK)?SUPPORTED_COMBINATION:CheckEncoderCapabilities((VFORMAT_ID)OutputFileFormat,(CodecID)VIDEOCODECDEF[VideoCodecIndex].Codec_id,(CodecID)AUDIOCODECDEF[AudioCodecIndex].Codec_id);
        if (Capabilities!=SUPPORTED_COMBINATION) {
            CustomMessageBox(this,QMessageBox::Information,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Incorrect codec combination!"));
            return false;
        }

        //============================================================================================
        // Render
        //============================================================================================

        bool Continue   =true;
        IsDestFileOpen  =true;

        // switch visible part of the dialog
        ui->SelectDestScroll->setEnabled(false);    ui->SelectDestScroll->setVisible(false);
        ui->ProcessWidget->setEnabled(true);        ui->ProcessWidget->setVisible(true);

        // Disable unwanted buttons
        ui->OkBt->setEnabled(false);
        ui->ProjectPropertiesBt->setEnabled(false);

        // Switch cancel button mode
        ui->CancelBt->setToolTip(QApplication::translate("DlgRenderVideo","Stop rendering process and exit dialog box"));

        // Set encoder display control
        Encoder.InfoLabelA1          =ui->InfoLabelA1;
        Encoder.InfoLabelA2          =ui->InfoLabelA2;
        Encoder.InfoLabelA3          =ui->InfoLabelA3;
        Encoder.InfoLabelA4          =ui->InfoLabelA4;
        Encoder.InfoLabelB1          =ui->InfoLabelB1;
        Encoder.InfoLabelB2          =ui->InfoLabelB2;
        Encoder.InfoLabelB3          =ui->InfoLabelB3;
        Encoder.InfoLabelB4          =ui->InfoLabelB4;
        Encoder.SlideProgressBarLabel=ui->SlideProgressBarLabel;
        Encoder.SlideNumberLabel     =ui->SlideNumberLabel;
        Encoder.SlideProgressBar     =ui->SlideProgressBar;
        Encoder.FrameNumberLabel     =ui->FrameNumberLabel;
        Encoder.TotalProgressBar     =ui->TotalProgressBar;
        Encoder.FPSLabel             =ui->FPSLabel;
        Encoder.ElapsedTimeLabel     =ui->ElapsedTimeLabel;

        if (ExportMode==MODE_SOUNDTRACK) {

            ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Encoding sound"));

            Mutex.lock();
            Continue=Encoder.OpenEncoder(Diaporama,OutputFileName,FromSlide,ToSlide,
                                    false,AV_CODEC_ID_NONE,NULL,0,0,0,0,0,MakeAVRational(1,1),0,
                                    true,AudioCodecIndex,2,AudioBitRate,AudioFrequency,Language);
            Mutex.unlock();

            Continue=Continue && Encoder.DoEncode();
            Encoder.CloseEncoder();

        } else {

            ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Encoding video"));

            int         Final_W,Final_H,Internal_W,Internal_H,Ext_H;
            AVRational  PixelAspectRatio;

            // Special case for SD-DVD format (anamorphous)
            if (ImageSize==SIZE_DVD) {
                switch (Standard) {
                    /* SD-DVD Anamorphous
                    SD-4/3  PAL:    I:768x576 -D:720x576-PAR:16:15  -DAR:4:3
                    SD-16/9 PAL:    I:1024x576-D:720x576-PAR:64:45  -DAR:16:9
                    SD-CIN  PAL:    I:1024x436-D:720x436-PAR:218:153-DAR:40:17
                    SD-4/3  NTSC:   I:640x480 -D:720x480-PAR:8:9    -DAR:4:3
                    SD-16/9 NTSC:   I:854x480 -D:720x480-PAR:32:27  -DAR:16:9
                    SD-CIN  NTSC:   I:854x436 -D:720x362-PAR:181:153-DAR:40:17
                    */

                    case STANDARD_PAL :
                        switch (Diaporama->ImageGeometry) {
                            case GEOMETRY_4_3:      Final_W=720;  Final_H=576;  Internal_W=768;  Internal_H=576;  PixelAspectRatio=MakeAVRational(16,15);     break;
                            case GEOMETRY_16_9:     Final_W=720;  Final_H=576;  Internal_W=1024; Internal_H=576;  PixelAspectRatio=MakeAVRational(64,45);     break;
                            case GEOMETRY_40_17:    Final_W=720;  Final_H=436;  Internal_W=1024; Internal_H=436;  PixelAspectRatio=MakeAVRational(64,45);     break;
                        }
                        Ext_H=576-Final_H;
                        break;
                    case STANDARD_NTSC:
                        switch (Diaporama->ImageGeometry) {
                            case GEOMETRY_4_3:      Final_W=720;  Final_H=480;  Internal_W=640;  Internal_H=480;  PixelAspectRatio=MakeAVRational(8,9);       break;
                            case GEOMETRY_16_9:     Final_W=720;  Final_H=480;  Internal_W=854;  Internal_H=480;  PixelAspectRatio=MakeAVRational(32,27);     break;
                            case GEOMETRY_40_17:    Final_W=720;  Final_H=306;  Internal_W=854;  Internal_H=306;  PixelAspectRatio=MakeAVRational(32,27);     break;
                        }
                        Ext_H=480-Final_H;
                        break;
                }
            } else {
                Ext_H       =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Extend*2;
                Final_W     =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Width;
                Final_H     =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Height;
                Internal_W  =Final_W;
                Internal_H  =Final_H;
                PixelAspectRatio=MakeAVRational(1,1);
            }

            Mutex.lock();
            Continue=Encoder.OpenEncoder(Diaporama,OutputFileName,FromSlide,ToSlide,
                                    true,VideoCodecIndex,&DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize],Final_W,Final_H,Ext_H,Internal_W,Internal_H,PixelAspectRatio,VideoBitRate,
                                    ui->IncludeSoundCB->isChecked(),AudioCodecIndex,2,AudioBitRate,AudioFrequency,Language);
            Mutex.unlock();
            Continue=Continue && Encoder.DoEncode();
            Encoder.CloseEncoder();

        }

        if ((Continue)&&(ui->ExportThumbCB->isChecked())) {
            QString              ThumbFileName      =OutputFileName.left(OutputFileName.lastIndexOf("."))+".jpg";
            cDiaporamaObjectInfo *Frame=            new cDiaporamaObjectInfo(NULL,0,Diaporama,1,false);
            Frame->CurrentObject                    =Diaporama->ProjectThumbnail;
            Frame->CurrentObject_CurrentShot        =Diaporama->ProjectThumbnail->List[0];
            Frame->CurrentObject_BackgroundBrush    =new QBrush(Qt::black,Qt::SolidPattern);
            Frame->CurrentObject_FreeBackgroundBrush=true;
            Diaporama->PrepareImage(Frame,THUMBWITH,THUMBHEIGHT,true,false,false);
            Frame->CurrentObject_PreparedImage->save(ThumbFileName,0,100);
            delete Frame;
        }

        // Inform user of success
        if (Continue)                           CustomMessageBox(this,QMessageBox::Information,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job completed successfully!"));
            else if (Encoder.StopProcessWanted) CustomMessageBox(this,QMessageBox::Information,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job canceled!"));
            else                                CustomMessageBox(this,QMessageBox::Information,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job error!\nPlease contact ffDiaporama team"));

        return true;
    }
    return false;
}