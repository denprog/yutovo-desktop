[Setup]
AppId={{bd041914-8896-43bc-b496-7aeb90887fff}}
AppName=Yutovo
AppVerName=Yutovo 1.5.3
AppPublisher=yutovo.com
AppPublisherURL=https://yutovo.com/
AppSupportURL=https://yutovo.com/
AppUpdatesURL=https://yutovo.com/
DefaultDirName={commonpf}\Yutovo
DefaultGroupName=Yutovo
OutputBaseFilename=yutovo-desktop_1.5.3-1_win10_amd64
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
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\yutovo-desktop.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\*.qm"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-library\library\*"; DestDir: "{app}\library"; Flags: ignoreversion recursesubdirs; Attribs: readonly
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\setup\Windows\fonts\*"; DestDir: "{app}\fonts"; Flags: ignoreversion recursesubdirs
Source: "VC_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Icons]
Name: "{group}\Yutovo"; Filename: "{app}\yutovo-desktop.exe"
Name: "{group}\Yutovo online"; Filename: "https://yutovo.com?ref=windows_menu"
Name: "{commondesktop}\Yutovo"; Filename: "{app}\yutovo-desktop.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Yutovo"; Filename: "{app}\yutovo-desktop.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCR; Subkey: ".yut"; ValueType: string; ValueName: ""; ValueData: "YutovoFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "YutovoFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\yutovo-desktop.exe,0"
Root: HKCR; Subkey: "YutovoFile"; ValueType: string; ValueName: ""; ValueData: "Yutovo File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "YutovoFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\yutovo-desktop.exe,0"; Flags: uninsdeletekey
Root: HKCR; Subkey: "YutovoFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\yutovo-desktop.exe"" ""%1"""; Flags: uninsdeletekey

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; Check: not VC2022RedistNeedsInstall
Filename: "{app}\yutovo-desktop.exe"; Description: "{cm:LaunchProgram,Yutovo}"; Flags: nowait postinstall skipifsilent

[Code]
function VC2022RedistNeedsInstall: Boolean;
var 
  Version: String;
begin
  Result := False;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64', 'Version', Version) then
  begin
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.44.35112.01') = 0);
  end
end;