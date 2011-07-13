; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ffDiaporama"
#define MyAppVersion "0.99.alpha3.3"
#define MyAppPublisher "The ffDiaporama Team"
#define MyAppURL "http://ffdiaporama.tuxfamily.org"
#define MyAppExeName "ffDiaporama.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
LicenseFile=C:\ffDiaporama\licences.txt
OutputDir=Y:\ffDiaporama\SiteWEB-Paquets
OutputBaseFilename={#MyAppName}.{#MyAppVersion}_setup
AppID={{113BC587-C2A9-457F-9022-9DB31ABBDD90}
AppName={#MyAppName}
AppCopyright={#MyAppPublisher}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
Compression=lzma/Max
SolidCompression=true
ShowLanguageDialog=no
VersionInfoDescription={#MyAppName}
VersionInfoCopyright={#MyAppPublisher}
VersionInfoProductName={#MyAppName}
VersionInfoTextVersion={#MyAppName} {#MyAppVersion}
VersionInfoCompany={#MyAppPublisher}
ChangesAssociations=true
VersionInfoVersion=0.99.3.3
VersionInfoProductVersion=0.99.3.3

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: C:\ffDiaporama\ffDiaporama.exe; DestDir: {app}; Flags: ignoreversion; Languages: english french; 
Source: C:\ffDiaporama\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; 
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
;Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent
