/*======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray<levray.dominique@bbox.fr>

    This program is free software;you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation;either version 2 of the License,or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY;without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program;if not,write to the Free Software Foundation,Inc.,
    51 Franklin Street,Fifth Floor,Boston,MA 02110-1301 USA.
  ======================================================================*/

#include "cDatabase.h"
#include "cBaseApplicationConfig.h"

#define DATABASEVERSION 2       // Current database version

void DisplayLastSQLError(QSqlQuery *Query) {
    ToLog(LOGMSG_CRITICAL,Query->lastQuery());
    ToLog(LOGMSG_CRITICAL,QString("Error %1:%2").arg(Query->lastError().number()).arg(Query->lastError().text()));
}

//**************************************************************************************************************************
// cDatabase : encapsulate a SQLite3 database
//**************************************************************************************************************************

cDatabase::cDatabase(QString DBFNAME) {
    dbPath=DBFNAME;
    ApplicationConfig=NULL;
}

//=====================================================================================================

cDatabase::~cDatabase() {
    while (!Tables.isEmpty()) delete Tables.takeLast();
    CloseDB();
}

//=====================================================================================================

bool cDatabase::OpenDB() {
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    QDir ApplicationFolder(QFileInfo(dbPath).absolutePath());
    if (!ApplicationFolder.exists()) ApplicationFolder.mkpath(ApplicationFolder.path());
    db.setDatabaseName(dbPath);

    // Open databasee
    if (!db.open()) return false;

    // Disable journalisation to speed query
    QSqlQuery Query(db);
    if (!Query.exec("PRAGMA journal_mode=OFF")) {
        DisplayLastSQLError(&Query);
        return false;
    }
    return true;
}

//=====================================================================================================
// Close database

bool cDatabase::CloseDB() {
    if (db.isOpen()) db.close();
    return true;
}

//=====================================================================================================
// Reset database and recreate it

bool cDatabase::ResetDB() {
    bool Ret=CloseDB();
    if (QFileInfo(dbPath).exists()) Ret=Ret && QFile::remove(dbPath);
    Ret=Ret && OpenDB();
    if (db.isOpen()) foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->CreateTable();
    return db.isOpen();
}

//=====================================================================================================
// Return table object corresponding to TableName

cDatabaseTable *cDatabase::GetTable(eTypeTable TableType) {
    foreach (cDatabaseTable *Table,Tables) if (Table->TypeTable==TableType) return Table;
    return NULL;
}

//=====================================================================================================
// Get database version from setting table and upgrade database if needed

bool cDatabase::CheckDatabaseVersion() {
    cSettingsTable  *SettingsTable=(cSettingsTable *)GetTable(TypeTable_SettingsTable);
    bool            Ret=SettingsTable!=NULL;
    if (Ret) {
        qlonglong DatabaseVersion=SettingsTable->GetIntValue("Version",0);
        if (DatabaseVersion<DATABASEVERSION) {
            foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->DoUpgradeTableVersion(DatabaseVersion);
            SettingsTable->SetIntValue("Version",DATABASEVERSION);
        }
    }
    return Ret;
}

//=====================================================================================================
// Validate each table one by one

bool cDatabase::ValidateTables() {
    bool Ret=db.isOpen();
    foreach (cDatabaseTable *Table,Tables) Ret=Ret && Table->ValidateTable();
    return Ret;
}

//=====================================================================================================

QSqlError cDatabase::LastError() {
    return db.lastError();
}

//**************************************************************************************************************************
// cDatabaseTable : encapsulate a table
//      this class must not been used directly but must be use as ancestor by all table class
//**************************************************************************************************************************

cDatabaseTable::cDatabaseTable(cDatabase *Database) {
    this->Database   =Database;
    NextIndex        =0;
    TypeTable        =TypeTable_Undefined;
}

//=====================================================================================================

cDatabaseTable::~cDatabaseTable() {
}

//=====================================================================================================

bool cDatabaseTable::ValidateTable() {
    QSqlQuery Query(Database->db);
    if (Query.exec(QString("SELECT MAX(%1) FROM %2").arg(IndexKeyName).arg(TableName))) {
        bool Ret=true;
        while (Query.next()) {
            QVariant Value=Query.value(0);
            if (!Value.isNull()) NextIndex=Value.toLongLong(&Ret);
        }
        if (!Ret) DisplayLastSQLError(&Query);
        return Ret;
    } else {
        if (Query.lastError().number()==1) return CreateTable(); else {
            DisplayLastSQLError(&Query);
            return false;
        }
    }
}

//=====================================================================================================

bool cDatabaseTable::CreateTable() {
    if (!CreateTableQuery.isEmpty()) {
        QSqlQuery   Query(Database->db);
        bool        Ret=Query.exec(CreateTableQuery);
        if (!Ret) DisplayLastSQLError(&Query);
        foreach (QString IndexQuery,CreateIndexQuery)
            Ret=Ret && Query.exec(IndexQuery);
        if (!Ret) DisplayLastSQLError(&Query);
        return Ret;
    } else return false;
}

//=====================================================================================================

bool cDatabaseTable::DoUpgradeTableVersion(qlonglong) {
    return true;
}

//**************************************************************************************************************************
// cSettingsTable : encapsulate the settings table
//**************************************************************************************************************************

cSettingsTable::cSettingsTable(cDatabase *Database):cDatabaseTable(Database) {
    TypeTable       =TypeTable_SettingsTable;
    TableName       ="Settings";
    IndexKeyName    ="Key";
    CreateTableQuery="create table Settings ("\
                            "Key                bigint primary key,"\
                            "Name               varchar(100),"\
                            "IntValue           bigint,"\
                            "TextValue          text"
                     ")";
    CreateIndexQuery.append("CREATE INDEX idx_Settings_Key ON Settings (Key)");
    CreateIndexQuery.append("CREATE INDEX idx_Settings_Name ON Settings (Name)");
}

//=====================================================================================================

bool cSettingsTable::CreateTable() {
    return (cDatabaseTable::CreateTable())&&(SetIntValue("Version",DATABASEVERSION));
}

//=====================================================================================================

qlonglong cSettingsTable::GetIntValue(QString SettingName,qlonglong DefaultValue) {
    QSqlQuery   Query(Database->db);
    qlonglong   RetValue=DefaultValue;

    Query.prepare(QString("SELECT IntValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        QVariant Value=Query.value(0);
        if (!Value.isNull()) {
            bool Ret=false;
            RetValue=Value.toLongLong(&Ret);
            if (!Ret) RetValue=DefaultValue;
        }
    }
    return RetValue;
}

//=====================================================================================================

bool cSettingsTable::SetIntValue(QString SettingName,qlonglong Value) {
    QSqlQuery   Query(Database->db);
    bool        Ret=true;

    Query.prepare(QString("UPDATE %1 set IntValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":IntValue",Value,      QSql::In);
    Query.bindValue(":Name",    SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,IntValue) VALUES (:Key,:Name,:IntValue)").arg(TableName));
        Query.bindValue(":Key",     ++NextIndex,QSql::In);
        Query.bindValue(":IntValue",Value,      QSql::In);
        Query.bindValue(":Name",    SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//=====================================================================================================

QString cSettingsTable::GetTextValue(QString SettingName,QString DefaultValue) {
    QSqlQuery   Query(Database->db);
    QString     RetValue=DefaultValue;

    Query.prepare(QString("SELECT TextValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        QVariant Value=Query.value(0);
        if (!Value.isNull()) RetValue=Value.toString();
    }
    return RetValue;
}

//=====================================================================================================

bool cSettingsTable::SetTextValue(QString SettingName,QString Value) {
    QSqlQuery   Query(Database->db);
    bool        Ret=true;

    Query.prepare(QString("UPDATE %1 set TextValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":TextValue",Value,      QSql::In);
    Query.bindValue(":Name",     SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,TextValue) VALUES (:Key,:Name,:TextValue)").arg(TableName));
        Query.bindValue(":Key",      ++NextIndex,QSql::In);
        Query.bindValue(":TextValue",Value,      QSql::In);
        Query.bindValue(":Name",     SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//=====================================================================================================

bool cSettingsTable::GetIntAndTextValue(QString SettingName,qlonglong *IntValue,QString *TextValue) {
    QSqlQuery   Query(Database->db);
    bool        Ret=false;

    Query.prepare(QString("SELECT IntValue,TextValue FROM %1 WHERE Name=:Name").arg(TableName));
    Query.bindValue(":Name",SettingName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        Ret=true;
        if (Query.value(0).isNull()) Ret=false; else *IntValue =Query.value(0).toLongLong(&Ret);
        if (Query.value(1).isNull()) Ret=false; else *TextValue=Query.value(1).toString();
    }
    return Ret;
}

//=====================================================================================================

bool cSettingsTable::SetIntAndTextValue(QString SettingName,qlonglong IntValue,QString TextValue) {
    bool        Ret=true;
    QSqlQuery   Query(Database->db);

    Query.prepare(QString("UPDATE %1 set TextValue=:TextValue,IntValue=:IntValue WHERE Name=:Name").arg(TableName));
    Query.bindValue(":TextValue",TextValue,  QSql::In);
    Query.bindValue(":IntValue", IntValue,   QSql::In);
    Query.bindValue(":Name",     SettingName,QSql::In);
    if ((!Query.exec())||(Query.numRowsAffected()==0)) {
        Query.prepare(QString("INSERT INTO %1 (Key,Name,IntValue,TextValue) VALUES (:Key,:Name,:IntValue,:TextValue)").arg(TableName));
        Query.bindValue(":Key",      ++NextIndex,QSql::In);
        Query.bindValue(":TextValue",TextValue,  QSql::In);
        Query.bindValue(":IntValue", IntValue,   QSql::In);
        Query.bindValue(":Name",     SettingName,QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query);
    }
    return Ret;
}

//**********************************************************************************************
// cFolderTable : encapsulate folders in the table
//**********************************************************************************************

cFolderTable::cFolderTable(cDatabase *Database):cDatabaseTable(Database) {
    TypeTable       =TypeTable_FolderTable;
    TableName       ="Folders";
    IndexKeyName    ="Key";
    CreateTableQuery="create table Folders ("\
                            "Key                bigint primary key,"\
                            "Name               varchar(512),"\
                            "ParentKey          bigint,"\
                            "Timestamp          bigint"
                     ")";
    CreateIndexQuery.append("CREATE INDEX idx_Folders_Key  ON Folders (Key)");
    CreateIndexQuery.append("CREATE INDEX idx_Folders_Name ON Folders (ParentKey,Name)");
}

//=====================================================================================================
// Get the key associated to a folder path
// If folder not found in the database, then create it and all his parents

qlonglong cFolderTable::GetFolderKey(QString FolderPath) {
    if ((FolderPath==".")||(FolderPath=="..")) return -1;
    FolderPath=QDir::toNativeSeparators(QDir(FolderPath).absolutePath());
    #ifdef Q_OS_WIN
    // On windows, network share start with \\,  so keep this information in a boolean and remove this "\"
    bool IsNetworkShare=FolderPath.startsWith("\\\\");
    if (IsNetworkShare) FolderPath=FolderPath.mid(QString("\\\\").length());
    #endif
    QStringList FolderList=FolderPath.split(QDir::separator());
    #ifdef Q_OS_WIN
    // On windows, add previously \\ removed before to create the list
    if ((IsNetworkShare)&&(FolderList.count()>0)) FolderList[0]="\\\\"+FolderList[0];
    #endif
    if (FolderList.last().isEmpty()) FolderList.removeLast();
    qlonglong Key=-1;
    QString   ParentPath;   if (FolderList.count()>1) for (int i=0;i<FolderList.count()-1;i++) ParentPath=ParentPath+FolderList[i]+QDir::separator();
    qlonglong ParentKey=(FolderList.count()>1)?GetFolderKey(ParentPath):-1;
    if (FolderList.count()>0) {
        QSqlQuery Query(Database->db);
        bool      Ret=false;
        Query.prepare(QString("SELECT Key FROM %1 WHERE ParentKey=:ParentKey AND Name=:Name").arg(TableName));
        Query.bindValue(":ParentKey",ParentKey,         QSql::In);
        Query.bindValue(":Name",     FolderList.last(), QSql::In);
        if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) if (!Query.value(0).isNull()) {
            Ret=true;
            Key=Query.value(0).toLongLong(&Ret);
        }
        if (!Ret) {
            // Path not found : then add it to the table
            qlonglong Timestamp=QFileInfo(FolderPath).lastModified().toMSecsSinceEpoch();
            Query.prepare(QString("INSERT INTO %1 (Key,Name,ParentKey,Timestamp) VALUES (:Key,:Name,:ParentKey,:Timestamp)").arg(TableName));
            Query.bindValue(":Key",      ++NextIndex,QSql::In);
            Query.bindValue(":Name",     FolderList.last(), QSql::In);
            Query.bindValue(":ParentKey",ParentKey,         QSql::In);
            Query.bindValue(":Timestamp",Timestamp,         QSql::In);
            Ret=Query.exec();
            if (!Ret) DisplayLastSQLError(&Query); else Key=NextIndex;
        }
    }
    return Key;
}

//=====================================================================================================
// Get the path associated to a folder key
// path are always ended with a QDir::separator()

QString cFolderTable::GetFolderPath(qlonglong FolderKey) {
    QSqlQuery   Query(Database->db);
    QString     Path;
    qlonglong   ParentKey=-1;

    Query.prepare(QString("SELECT ParentKey,Name FROM %1 WHERE Key=:Key").arg(TableName));
    Query.bindValue(":Key",FolderKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        if (!Query.value(0).isNull()) {
            bool Ret;
            ParentKey=Query.value(0).toLongLong(&Ret);
            if (!Ret) ParentKey=-1;
        }
        Path=Query.value(1).toString();
    }
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    if (ParentKey!=-1) Path=GetFolderPath(ParentKey)+Path;
    return Path;
}

//**********************************************************************************************
// cFilesTable : encapsulate media files in the table
//**********************************************************************************************

cFilesTable::cFilesTable(cDatabase *Database):cDatabaseTable(Database) {
    TypeTable       =TypeTable_FileTable;
    TableName       ="MediaFiles";
    IndexKeyName    ="Key";
    CreateTableQuery="create table MediaFiles ("\
                            "Key                bigint primary key,"\
                            "ShortName          varchar(256),"\
                            "FolderKey          bigint,"\
                            "Timestamp          bigint,"\
                            "IsHidden           int,"\
                            "IsDir              int,"\
                            "CreatDateTime      text,"\
                            "ModifDateTime      text,"\
                            "FileSize           bigint,"\
                            "MediaFileType      int,"\
                            "BasicProperties    text,"\
                            "ExtendedProperties text,"\
                            "Thumbnail16        binary,"\
                            "Thumbnail100       binary"\
                     ")";
    CreateIndexQuery.append("CREATE INDEX idx_MediaFiles_Key ON MediaFiles (Key)");
    CreateIndexQuery.append("CREATE INDEX idx_MediaFiles_FolderKey ON MediaFiles (FolderKey,ShortName)");
}

//=====================================================================================================
// Get the key associated to a file
// If file not found in the database, then create it

qlonglong cFilesTable::GetFileKey(qlonglong FolderKey,QString ShortName,int MediaFileType) {
    QSqlQuery   Query(Database->db);
    bool        Ret=false;
    qlonglong   FileKey=-1;
    Query.prepare(QString("SELECT Key FROM %1 WHERE FolderKey=:FolderKey AND ShortName=:ShortName").arg(TableName));
    Query.bindValue(":FolderKey",FolderKey,QSql::In);
    Query.bindValue(":ShortName",ShortName,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) if (!Query.value(0).isNull()) {
        Ret=true;
        FileKey=Query.value(0).toLongLong(&Ret);
    }
    if (!Ret) {
        // File not found : then add it to the table
        QString   FullPath=((cFolderTable *)Database->GetTable(TypeTable_FolderTable))->GetFolderPath(FolderKey)+ShortName;
        QFileInfo FileInfo(FullPath);
        Query.prepare(QString("INSERT INTO %1 (Key,ShortName,FolderKey,Timestamp,IsHidden,IsDir,CreatDateTime,ModifDateTime,FileSize,MediaFileType) "\
                                      "VALUES (:Key,:ShortName,:FolderKey,:Timestamp,:IsHidden,:IsDir,:CreatDateTime,:ModifDateTime,:FileSize,:MediaFileType)").arg(TableName));
        Query.bindValue(":Key",          ++NextIndex,                                                   QSql::In);
        Query.bindValue(":ShortName",    ShortName,                                                     QSql::In);
        Query.bindValue(":FolderKey",    FolderKey,                                                     QSql::In);
        Query.bindValue(":Timestamp",    FileInfo.lastModified().toMSecsSinceEpoch(),                   QSql::In);
        Query.bindValue(":IsHidden",     FileInfo.isHidden()||FileInfo.fileName().startsWith(".")?1:0,  QSql::In);
        Query.bindValue(":IsDir",        FileInfo.isDir()?1:0,                                          QSql::In);
        Query.bindValue(":CreatDateTime",FileInfo.lastModified(),                                       QSql::In);
        Query.bindValue(":ModifDateTime",FileInfo.created(),                                            QSql::In);
        Query.bindValue(":FileSize",     FileInfo.size(),                                               QSql::In);
        Query.bindValue(":MediaFileType",MediaFileType,                                                 QSql::In);
        Ret=Query.exec();
        if (!Ret) DisplayLastSQLError(&Query); else FileKey=NextIndex;
    }
    return FileKey;
}

//=====================================================================================================
// Get the path associated to a folder key
// path are always ended with a QDir::separator()

QString cFilesTable::GetShortName(qlonglong FileKey) {
    QSqlQuery   Query(Database->db);
    QString     ShortName;

    Query.prepare(QString("SELECT ShortName FROM %1 WHERE Key=:Key").arg(TableName));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) ShortName=Query.value(0).toString();
    return ShortName;
}

//=====================================================================================================
// Get the path associated to a folder key
// path are always ended with a QDir::separator()

QString cFilesTable::GetFileName(qlonglong FileKey) {
    QSqlQuery   Query(Database->db);
    QString     FolderPath;
    QString     ShortName;
    qlonglong   FolderKey=-1;

    Query.prepare(QString("SELECT FolderKey,ShortName FROM %1 WHERE Key=:Key").arg(TableName));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        if (!Query.value(0).isNull()) {
            bool Ret=true;
            FolderKey=Query.value(0).toLongLong(&Ret);
            if (Ret) FolderPath=((cFolderTable *)Database->GetTable(TypeTable_FolderTable))->GetFolderPath(FolderKey);
        }
        ShortName=Query.value(1).toString();
    }
    return FolderPath+ShortName;
}

//=====================================================================================================
// scan all files for a given folderkey and:
//  - delete files no longer exist
//  - set data field to null for modified files (different timestamp)

int cFilesTable::CleanTableForFolder(qlonglong FolderKey) {
    int         NbrModif=0;
    QSqlQuery   Query(Database->db);
    QSqlQuery   Query2(Database->db);
    QString     FolderPath=((cFolderTable *)Database->GetTable(TypeTable_FolderTable))->GetFolderPath(FolderKey);
    Query.prepare(QString("SELECT Key,ShortName,Timestamp FROM %1 WHERE FolderKey=:FolderKey").arg(TableName));
    Query.bindValue(":FolderKey",FolderKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        qlonglong FileKey=-1;
        QString   ShortName;
        qlonglong Timestamp=-1;
        bool      Ret=false;

        if (!Query.value(0).isNull()) {
            Ret=true;
            FileKey=Query.value(0).toLongLong(&Ret);
        }
        if (Ret) ShortName=Query.value(1).toString();
        if ((Ret)&&(!Query.value(2).isNull())) {
            Ret=true;
            Timestamp=Query.value(2).toLongLong(&Ret);
        }

        if (Ret) {
            QFileInfo FileInfo(FolderPath+ShortName);
            if (!FileInfo.exists()) {
                Query2.prepare((QString("DELETE FROM %1 WHERE Key=:Key").arg(TableName)));
                Query2.bindValue(":Key",FileKey,QSql::In);
                if (!Query2.exec()) DisplayLastSQLError(&Query2);
                NbrModif++;
            } else {
                qlonglong NewTimestamp=FileInfo.lastModified().toMSecsSinceEpoch();
                if (NewTimestamp!=Timestamp) {
                    Query2.prepare((QString("UPDATE %1 SET Thumbnail16=NULL,Thumbnail100=NULL,BasicProperties=NULL,ExtendedProperties=NULL,Timestamp=:Timestamp,"\
                                            "CreatDateTime=:CreatDateTime,ModifDateTime=:ModifDateTime,FileSize=:FileSize WHERE Key=:Key").arg(TableName)));
                    Query2.bindValue(":Key",          FileKey,                                      QSql::In);
                    Query2.bindValue(":Timestamp",    NewTimestamp,                                 QSql::In);
                    Query2.bindValue(":CreatDateTime",FileInfo.lastModified().toString(Qt::ISODate),QSql::In);
                    Query2.bindValue(":ModifDateTime",FileInfo.created().toString(Qt::ISODate),     QSql::In);
                    Query2.bindValue(":FileSize",     FileInfo.size(),                              QSql::In);
                    if (!Query2.exec()) DisplayLastSQLError(&Query2);
                    NbrModif++;
                }
            }
        }
    }
    return NbrModif;
}

//=====================================================================================================
// scan all files for a given folderkey and add new one to the table

void cFilesTable::UpdateTableForFolder(qlonglong FolderKey) {
    if (CleanTableForFolder(FolderKey)>0) {
    }
        QString         FolderPath=((cFolderTable *)Database->GetTable(TypeTable_FolderTable))->GetFolderPath(FolderKey);
        QDir            Folder(FolderPath);
        QFileInfoList   Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
        foreach(QFileInfo File,Files) {
            QString ShortName=File.fileName();
            if ((ShortName!=".")&&(ShortName!="..")) {
                int ObjectType=OBJECTTYPE_UNMANAGED;
                if (Database->ApplicationConfig->AllowImageExtension.contains(File.suffix().toLower())) {
                    ObjectType=OBJECTTYPE_IMAGEFILE;
                    // Special case for folder Thumbnails
                    if (File.fileName()=="folder.jpg") {
                        ObjectType=OBJECTTYPE_THUMBNAIL;
                    } else if ((File.fileName().toLower().endsWith("-poster.jpg"))||(File.fileName().toLower().endsWith("-poster.png"))) {  // Special case for video xbmc poster Thumbnails
                        // Search if a video with same name exist
                        QString ToSearch=File.fileName().left(File.fileName().toLower().indexOf("-poster."));
                        for (int i=0;i<Files.count();i++)
                            if ((Database->ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&(((QFileInfo)Files[i]).completeBaseName()==ToSearch))
                                ObjectType=OBJECTTYPE_THUMBNAIL;
                    } else if (File.suffix().toLower()=="jpg") {  // Special case for video Thumbnails
                        // Search if a video with same name exist
                        for (int i=0;i<Files.count();i++)
                            if ((Database->ApplicationConfig->AllowVideoExtension.contains(((QFileInfo)Files[i]).suffix().toLower()))&&
                                (((QFileInfo)Files[i]).completeBaseName()==File.completeBaseName())) ObjectType=OBJECTTYPE_THUMBNAIL;
                    }
                }   else if (Database->ApplicationConfig->AllowVideoExtension.contains(File.suffix().toLower()))   ObjectType=OBJECTTYPE_VIDEOFILE;
                    else if (Database->ApplicationConfig->AllowMusicExtension.contains(File.suffix().toLower()))   ObjectType=OBJECTTYPE_MUSICFILE;
                    else if (File.suffix().toLower()=="ffd")                                                       ObjectType=OBJECTTYPE_FFDFILE;
                    else if (File.isDir())                                                                         ObjectType=OBJECTTYPE_FOLDER;

                GetFileKey(FolderKey,File.fileName(),ObjectType);
            }
        }
//    }
}

//=====================================================================================================
// Write basic properties to the database

bool cFilesTable::SetBasicProperties(qlonglong FileKey,QString Properties) {
    QSqlQuery Query(Database->db);
    Query.prepare((QString("UPDATE %1 SET BasicProperties=:BasicProperties WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",            FileKey,   QSql::In);
    Query.bindValue(":BasicProperties",Properties,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    return true;
}

//=====================================================================================================
// Read basic properties from the database

bool cFilesTable::GetBasicProperties(qlonglong FileKey,QString *Properties,QString FileName,int64_t *FileSize,QDateTime *CreatDateTime,QDateTime *ModifDateTime) {
    QSqlQuery Query(Database->db);
    Query.prepare((QString("SELECT BasicProperties,Timestamp,FileSize,CreatDateTime,ModifDateTime FROM %1 WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query); else while (Query.next()) {
        bool        Ret=true;
        qlonglong   Timestamp=0;
        if (!Query.value(0).isNull())           *Properties   =Query.value(0).toString();                                               else Ret=false;
        if ((Ret)&&(!Query.value(1).isNull()))  Timestamp     =Query.value(1).toLongLong(&Ret);                                         else Ret=false;
        if ((Ret)&&(!Query.value(2).isNull()))  *FileSize     =Query.value(2).toLongLong(&Ret);                                         else Ret=false;
        if ((Ret)&&(!Query.value(3).isNull()))  *CreatDateTime=Query.value(3).toDateTime();                                             else Ret=false;
        if ((Ret)&&(!Query.value(4).isNull()))  *ModifDateTime=Query.value(4).toDateTime();                                             else Ret=false;
        Ret=Ret && (Timestamp==QFileInfo(FileName).lastModified().toMSecsSinceEpoch());
        return Ret;
    }
    return false;
}

//=====================================================================================================
// Write extended properties to the database

bool cFilesTable::SetExtendedProperties(qlonglong FileKey,QStringList *PropertiesList) {
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("ExtendedProperties");
    domDocument.appendChild(root);
    for (int i=0;i<PropertiesList->count();i++) {
        QStringList Values=PropertiesList->at(i).split("##");
        QDomElement Element=domDocument.createElement(QString("Item-%1").arg(i));
        Element.setAttribute("Name", Values[0]);
        Element.setAttribute("Value",Values[1]);
        root.appendChild(Element);
    }
    QSqlQuery Query(Database->db);
    Query.prepare((QString("UPDATE %1 SET ExtendedProperties=:ExtendedProperties WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",               FileKey,               QSql::In);
    Query.bindValue(":ExtendedProperties",domDocument.toString(),QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    return true;
}

//=====================================================================================================
// Read extended properties from the database

bool cFilesTable::GetExtendedProperties(qlonglong FileKey,QStringList *PropertiesList) {
    QSqlQuery Query(Database->db);
    Query.prepare((QString("SELECT ExtendedProperties FROM %1 WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    while (Query.next()) {
        QDomDocument    domDocument;
        QString         errorStr;
        int             errorLine,errorColumn;
        if ((domDocument.setContent(Query.value(0).toString(),true,&errorStr,&errorLine,&errorColumn))&&
            (domDocument.elementsByTagName("ExtendedProperties").length()>0)&&
            (domDocument.elementsByTagName("ExtendedProperties").item(0).isElement()==true)) {

            QDomElement root=domDocument.elementsByTagName("ExtendedProperties").item(0).toElement();
            int i=0;
            while ((root.elementsByTagName(QString("Item-%1").arg(i)).length()>0)&&(root.elementsByTagName(QString("Item-%1").arg(i)).item(0).isElement()==true)) {
                QDomElement Element=root.elementsByTagName(QString("Item-%1").arg(i)).item(0).toElement();
                PropertiesList->append(Element.attribute("Name")+"##"+Element.attribute("Value"));
                i++;
            }
        }
        return true;
    }
    return false;
}

//=====================================================================================================
// Write thumbnails to the database

bool cFilesTable::SetThumbs(qlonglong FileKey,QImage *Icon16,QImage *Icon100) {
    QSqlQuery Query(Database->db);
    Query.prepare((QString("UPDATE %1 SET Thumbnail16=:Thumbnail16,Thumbnail100=:Thumbnail100 WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Icon16->isNull()) {
        QByteArray  Data;
        QBuffer     BufData(&Data);
        BufData.open(QIODevice::WriteOnly);
        Icon16->save(&BufData,"PNG");
        Query.bindValue(":Thumbnail16",Data,QSql::In|QSql::Binary);
    } else Query.bindValue(":Thumbnail16",QVariant(QVariant::ByteArray),QSql::In|QSql::Binary);
    if (!Icon100->isNull()) {
        QByteArray  Data;
        QBuffer     BufData(&Data);
        BufData.open(QIODevice::WriteOnly);
        Icon100->save(&BufData,"PNG");
        Query.bindValue(":Thumbnail100",Data,QSql::In|QSql::Binary);
    } else Query.bindValue(":Thumbnail100",QVariant(QVariant::ByteArray),QSql::In|QSql::Binary);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    return true;
}

//=====================================================================================================
// Read thumbnails properties from the database

bool cFilesTable::GetThumbs(qlonglong FileKey,QImage *Icon16,QImage *Icon100) {
    QSqlQuery Query(Database->db);
    Query.prepare((QString("SELECT Thumbnail16,Thumbnail100 FROM %1 WHERE Key=:Key").arg(TableName)));
    Query.bindValue(":Key",FileKey,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    QByteArray Data;
    while (Query.next()) {
        if (!Query.value(0).isNull()) {
            Data=Query.value(0).toByteArray();
            Icon16->loadFromData(Data);
        }
        if (!Query.value(1).isNull()) {
            Data=Query.value(1).toByteArray();
            Icon100->loadFromData(Data);
        }
    }
    return (!Icon16->isNull())&&(!Icon100->isNull());
}

//=====================================================================================================

bool cFilesTable::DoUpgradeTableVersion(qlonglong OldVersion) {
    QSqlQuery Query(Database->db);
    bool Ret=true;
    if (OldVersion==1) Ret=Query.exec("DROP TABLE MediaFiles");
    if (!Ret) {
        if (Query.lastError().number()==1) Ret=true;    // If table not existed then ignore error
            else DisplayLastSQLError(&Query);
    }
    return Ret;
}