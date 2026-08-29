[Setup]
AppId={{bd041914-8896-43bc-b496-7aeb90887fff}}
AppName=Yutovo
AppVersion=1.7.1
AppVerName=Yutovo 1.7.1
AppPublisher=yutovo.com
AppPublisherURL=https://yutovo.com/
AppSupportURL=https://yutovo.com/
AppUpdatesURL=https://yutovo.com/
DefaultDirName={commonpf}\Yutovo
DefaultGroupName=Yutovo
OutputDir=Output
OutputBaseFilename=yutovo-desktop_1.7.1-1_win10_amd64
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
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\yutovo-solver-calculator-worker.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\qt.conf"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\*.dll"; DestDir: "{app}"; Excludes: "gtest*.dll,gmock*.dll,boost_unit_test_framework*.dll"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\*.qm"; DestDir: "{app}"; Flags: ignoreversion
Source: "c:\Lang\Programs\yutovo\yutovo-desktop\build\release\src\Release\library\*"; DestDir: "{app}\library"; Flags: ignoreversion recursesubdirs overwritereadonly; Attribs: readonly
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
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Visual C++ 2022 Redistributable"; Check: VC2022RedistNeedsInstall
Filename: "{app}\yutovo-desktop.exe"; Description: "{cm:LaunchProgram,Yutovo}"; Flags: nowait postinstall skipifsilent

[Code]
function CompareVersion(A, B: String): Integer;
var
  PartA, PartB: String;
  NumA, NumB: Integer;
begin
  if (Length(A) > 1) and (A[1] = 'v') then
    A := Copy(A, 2, Length(A) - 1);
  if (Length(B) > 1) and (B[1] = 'v') then
    B := Copy(B, 2, Length(B) - 1);
  Result := 0;
  while ((Length(A) > 0) or (Length(B) > 0)) and (Result = 0) do
  begin
    if Pos('.', A) > 0 then
    begin
      PartA := Copy(A, 1, Pos('.', A) - 1);
      A := Copy(A, Pos('.', A) + 1, Length(A));
    end
    else
    begin
      PartA := A;
      A := '';
    end;
    if Pos('.', B) > 0 then
    begin
      PartB := Copy(B, 1, Pos('.', B) - 1);
      B := Copy(B, Pos('.', B) + 1, Length(B));
    end
    else
    begin
      PartB := B;
      B := '';
    end;
    NumA := StrToIntDef(PartA, 0);
    NumB := StrToIntDef(PartB, 0);
    if NumA > NumB then
      Result := 1
    else if NumA < NumB then
      Result := -1;
  end;
end;

function VC2022RedistNeedsInstall: Boolean;
var
  Version: String;
begin
  Result := True;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64', 'Version', Version) then
  begin
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareVersion(Version, 'v14.44.35112.01') < 0);
  end
end;