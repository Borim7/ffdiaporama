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

#ifndef _MODEL_H
#define _MODEL_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"
#include <QImage>

enum ffd_MODELTYPE {
    ffd_MODELTYPE_THUMBNAIL,
    ffd_MODELTYPE_PROJECTTITLE,
    ffd_MODELTYPE_CHAPTERTITLE,
    ffd_MODELTYPE_CREDITTITLE
};

// Number of categorie by type
#define MODELTYPE_PROJECTTITLE_CATNUMBER    3
#define MODELTYPE_CHAPTERTITLE_CATNUMBER    3
#define MODELTYPE_CREDITTITLE_CATNUMBER     2

class cBaseApplicationConfig;
class cDiaporama;
class cDiaporamaObject;
class cModelList;
class cModelListItem {
public:
    cModelList      *Parent;
    QDomDocument    Model;
    QString         Name;
    QString         FileName;
    QSize           ThumbnailSize;
    bool            IsCustom;
    int64_t         Duration;

    cModelListItem(cModelList *Parent,QString FileName,QSize ThumbnailSize);
    ~cModelListItem();

    QDomDocument            LoadModelFile(ffd_MODELTYPE TypeModel,QString ModelFileName);
    QImage                  PrepareImage(int64_t Position,cDiaporama *Diaporama,cDiaporamaObject *DiaporamaObjectToUse,QSize *ForcedThumbnailSize=NULL);
};

class cModelList {
public:
    cBaseApplicationConfig  *ApplicationConfig;
    ffd_MODELTYPE           ModelType;
    ffd_GEOMETRY            ProjectGeometry;
    QString                 StandardModelPath;
    QString                 CustomModelPath;
    QString                 ModelSuffix;
    QList<cModelListItem>   List;
    QSize                   ThumbnailSize;
    int64_t                 *NextNumber;
    int                     DigitCategorie;
    QString                 NameCategorie;

    cModelList(cBaseApplicationConfig *ApplicationConfig,ffd_MODELTYPE ModelType,int64_t *NextNumber,ffd_GEOMETRY ProjectGeometry,int DigitCategorie,QString NameCategorie);
    ~cModelList();

    int             SearchModel(QString ModelName);
    void            FillModelType(ffd_MODELTYPE ModelType);
    cModelListItem  *AppendCustomModel();
    void            RemoveCustomModel();
    QDomDocument    GetModelDocument(QString ModelName);
};

#endif // _MODEL_H