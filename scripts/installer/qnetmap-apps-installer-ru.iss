[Setup]
AppName=QNetMap Applications
AppVersion={#GetEnv('QNETMAP_VERSION')}
DefaultDirName={commonpf}\QNetMapApplications
AppMutex=qnetmap-app
SetupMutex=qnetmap-installer
DefaultGroupName=QNetMap Applications
SetupIconFile={#GetEnv('QNETMAP')}\mapsviewer\icon\mapsviewer.ico
UninstallDisplayIcon={app}\mapsviewer.exe
Compression=lzma2
SolidCompression=yes
OutputBaseFilename=qnetmap-applications-{#GetEnv('QNETMAP_VERSION')}
OutputDir={#GetEnv('PRODUCTION_DIR')}\.installers
PrivilegesRequired=admin
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\mapsviewer.exe"; DestDir: "{app}"; DestName: "mapsviewer.exe"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\mapsspider.exe"; DestDir: "{app}"; DestName: "mapsspider.exe"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap.dll"; DestDir: "{app}"; DestName: "qnetmap.dll"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap.xml"; DestDir: "{app}"; DestName: "qnetmap.xml"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap_empty_map.dll"; DestDir: "{app}"; DestName: "qnetmap_empty_map.dll"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap_raster_map.dll"; DestDir: "{app}"; DestName: "qnetmap_raster_map.dll"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap_ru.qm"; DestDir: "{app}"; DestName: "qnetmap_ru.qm"
Source: "{#GetEnv('QNETMAP')}\build\x64\Release\qnetmap_universaltile_map.dll"; DestDir: "{app}"; DestName: "qnetmap_universaltile_map.dll"

Source: "{#GetEnv('PRODUCTION_DIR')}\.vcredist\vcredist_2015-2019_x64.exe"; DestDir: "{app}\redist"; DestName: "vcredist_2015-2019_x64.exe"

Source: "{#GetEnv('QTDIR')}\bin\Qt5Core.dll"; DestDir: "{app}"; DestName: "Qt5Core.dll"
Source: "{#GetEnv('QTDIR')}\bin\Qt5Gui.dll"; DestDir: "{app}"; DestName: "Qt5Gui.dll"
Source: "{#GetEnv('QTDIR')}\bin\Qt5Widgets.dll"; DestDir: "{app}"; DestName: "Qt5Widgets.dll"
Source: "{#GetEnv('QTDIR')}\bin\Qt5Xml.dll"; DestDir: "{app}"; DestName: "Qt5Xml.dll"
Source: "{#GetEnv('QTDIR')}\bin\Qt5Network.dll"; DestDir: "{app}"; DestName: "Qt5Network.dll"
Source: "{#GetEnv('QTDIR')}\bin\Qt5PrintSupport.dll"; DestDir: "{app}"; DestName: "Qt5PrintSupport.dll"
Source: "{#GetEnv('QTDIR')}\translations\qtbase_ru.qm"; DestDir: "{app}"; DestName: "qtbase_ru.qm"
Source: "{#GetEnv('QTDIR')}\plugins\imageformats\qgif.dll"; DestDir: "{app}\imageformats"; DestName: "qgif.dll"
Source: "{#GetEnv('QTDIR')}\plugins\imageformats\qico.dll"; DestDir: "{app}\imageformats"; DestName: "qico.dll"
Source: "{#GetEnv('QTDIR')}\plugins\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; DestName: "qjpeg.dll"
Source: "{#GetEnv('QTDIR')}\plugins\imageformats\qtiff.dll"; DestDir: "{app}\imageformats"; DestName: "qtiff.dll"
Source: "{#GetEnv('QTDIR')}\plugins\imageformats\qwbmp.dll"; DestDir: "{app}\imageformats"; DestName: "qwbmp.dll"
Source: "{#GetEnv('QTDIR')}\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"; DestName: "qwindows.dll"

[Dirs]
Name: "{commonappdata}\qnetmap\maps"
Name: "{commonappdata}\qnetmap\cache"

[Icons]
Name: "{group}\Maps Viewer"; Filename: "{app}\mapsviewer.exe"; IconFilename: "{app}\mapsviewer.exe"; WorkingDir: "{app}"
Name: "{group}\Maps Spider"; Filename: "{app}\mapsspider.exe"; IconFilename: "{app}\mapsspider.exe"; WorkingDir: "{app}"

[Languages]
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
	ResultCode: Integer;
begin
  if CurStep = ssPostInstall then
  begin

	if not RegKeyExists(HKEY_CLASSES_ROOT, 'Installer\Dependencies\VC,redist.x64,amd64,14.22,bundle\Dependents\{6361b579-2795-4886-b2a8-53d5239b6452}') then
	begin
		WizardForm.StatusLabel.Caption := '”становка библиотеки времени выполнени€ дл€ Visual Studio 2015-2019...';
		Exec(ExpandConstant('{app}') + '\redist\vcredist_2015-2019_x64.exe', '/q /norestart', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
	end;

	if not RegValueExists(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'CachePath_0') then
		begin
			RegWriteStringValue(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'CachePath_0', ExpandConstant('{commonappdata}') + '\qnetmap\cache');
		end;

	if not RegValueExists(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'StoragePath_0') then
		begin
		RegWriteStringValue(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'StoragePath_0', ExpandConstant('{commonappdata}') + '\qnetmap\maps');
		end;

	if not RegValueExists(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'StorageType') then
		begin
		RegWriteDWordValue(HKEY_CURRENT_USER, 'Software\QNetMap\MapsViewer', 'StorageType', 3);
		end;

	end;
end;
