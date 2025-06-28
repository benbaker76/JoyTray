; -- JoyTray.iss --

[Setup]
AppName=JoyTray
AppVerName=JoyTray {#AppVersion}
AppPublisher=Ben Baker
DefaultDirName={pf}\JoyTray
DefaultGroupName=JoyTray
UninstallDisplayIcon={app}\JoyTray.exe
Compression=lzma
SolidCompression=yes
OutputDir=Setup
OutputBaseFilename=JoyTraySetup
WizardImageFile=WizardImage.bmp
WizardSmallImageFile=WizardSmallImage.bmp

[Files]
Source: "JoyTray.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "JoyTray.ini"; DestDir: "{app}"; Flags: ignoreversion
Source: "UltraStik.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libusb0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "PacDrive.dll"; DestDir: "{app}"; Flags: ignoreversion

[UninstallDelete]
Type: files; Name: "{app}\JoyTray.exe"
Type: files; Name: "{app}\JoyTray.ini"
Type: files; Name: "{app}\UltraStik.dll"
Type: files; Name: "{app}\libusb0.dll"

[Run]
Filename: "{app}\JoyTray.exe"; Description: "Launch JoyTray"; Flags: postinstall nowait skipifsilent

[UninstallRun]
Filename: "{app}\JoyTray.exe"; Parameters: "-exit"

[Icons]
Name: "{group}\JoyTray"; Filename: "{app}\JoyTray.exe"
Name: "{group}\Uninstall JoyTray"; Filename: "{app}\unins000.exe"

