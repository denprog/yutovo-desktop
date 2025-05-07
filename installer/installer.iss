[Setup]
AppId={{bd041914-8896-43bc-b496-7aeb90887fff}}
AppName=Yutovo
AppVerName=Yutovo 1.0.2
AppPublisher=yutovo.com
AppPublisherURL=http://www.yutovo.com/
AppSupportURL=http://www.yutovo.com/
AppUpdatesURL=http://www.yutovo.com/
DefaultDirName={commonpf}\Yutovo
DefaultGroupName=Yutovo
OutputBaseFilename=yutovo-desktop_1.0.2-1_win10_amd64
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}";
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}";

[Files]
Source: "c:\Lang\Programs\yutovo\yutovo_desktop\build\RelWithDebugInfo\src\RelWithDebInfo\yutovo_desktop.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo_desktop\build\RelWithDebugInfo\src\RelWithDebInfo\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo_desktop\build\RelWithDebugInfo\src\RelWithDebInfo\*.qm"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo_desktop\build\RelWithDebugInfo\src\RelWithDebInfo\library\*"; DestDir: "{app}\library"; Flags: ignoreversion recursesubdirs
Source: "c:\Lang\Programs\yutovo\yutovo_desktop\build\RelWithDebugInfo\src\RelWithDebInfo\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\Yutovo"; Filename: "{app}\yutovo_desktop.exe"
Name: "{group}\Yutovo online"; Filename: "https://yutovo.com?ref=windows_menu"
Name: "{commondesktop}\Yutovo"; Filename: "{app}\yutovo_desktop.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Yutovo"; Filename: "{app}\yutovo_desktop.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCR; Subkey: ".yut"; ValueType: string; ValueName: ""; ValueData: "Yutovo"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Yutovo"; ValueType: string; ValueName: ""; ValueData: "Yutovo"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Yutovo\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\yutovo_desktop.exe,0"
Root: HKCR; Subkey: "Yutovo\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\yutovo_desktop.exe"" ""%1"""

Root: HKLM; Subkey: "Software\Yutovo"; ValueType: string; ValueName: "InstallDir"; ValueData: "{app}"

[Run]
Filename: "{app}\yutovo_desktop.exe"; Description: "{cm:LaunchProgram,Yutovo}"; Flags: nowait postinstall skipifsilent
