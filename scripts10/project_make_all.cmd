@echo off
@rem Сборка проекта из сольюшена для заданной или всех конфигураций.
@set PROJECT_NAME=%2
@set SOLUTION_NAME=%1

rem For Visual Studio 2010
set VISUAL_STUDIO_VERSION=10

rem Определяем путь к выполняемому файлу Visual Studio
set REGISTRY_PATH=HKLM\SOFTWARE\Microsoft\VisualStudio
if "%ProgramFiles(x86)%_"=="_" goto FIND_VS_PATH
set REGISTRY_PATH=HKLM\SOFTWARE\Wow6432Node\Microsoft\VisualStudio

:FIND_VS_PATH
set TEMP_FILE=%TEMP%\VCID%RANDOM%.txt
reg QUERY %REGISTRY_PATH%\%VISUAL_STUDIO_VERSION%.0 /v InstallDir | find "InstallDir" > %TEMP_FILE%
set /p INSTALLDIRHELPER= <%TEMP_FILE% > nul
del %TEMP_FILE%
set INSTALLDIRHELPER=%INSTALLDIRHELPER:    =	%
for /F "tokens=3 delims=	" %%P in ("%INSTALLDIRHELPER%") do set DEVENV=%%Pdevenv.exe

if _%PROJECT_NAME%==_ goto LABEL_BAD_PARAMETERS

if _%SOLUTION_NAME%==_ goto LABEL_BAD_PARAMETERS

if NOT EXIST %SOLUTION_NAME% goto LABEL_SOLUTION_NOT_EXISTS

set BUILD_WIN32_DEBUG=Y
set BUILD_WIN32_RELEASE=Y
set BUILD_X64_DEBUG=Y
set BUILD_X64_RELEASE=Y

if %3_==debug-win32-only_ (
   set BUILD_WIN32_RELEASE=N
   set BUILD_X64_DEBUG=N
   set BUILD_X64_RELEASE=N
   )

if %3_==release-win32-only_ (
   set BUILD_WIN32_DEBUG=N
   set BUILD_X64_DEBUG=N
   set BUILD_X64_RELEASE=N
   )

if %3_==debug-x64-only_ (
   set BUILD_WIN32_DEBUG=N
   set BUILD_WIN32_RELEASE=N
   set BUILD_X64_RELEASE=N
   )

if %3_==release-x64-only_ (
   set BUILD_WIN32_DEBUG=N
   set BUILD_WIN32_RELEASE=N
   set BUILD_X64_DEBUG=N
   )

if %BUILD_WIN32_DEBUG%==Y (
   echo Making %SOLUTION_NAME%: %PROJECT_NAME% - "Debug|Win32"
   "%DEVENV%" %SOLUTION_NAME% /Build "Debug|Win32" /Project %PROJECT_NAME%
   if ERRORLEVEL 1 goto LABEL_ERROR
   )

if %BUILD_WIN32_RELEASE%==Y (
   echo Making %SOLUTION_NAME%: %PROJECT_NAME% - "Release|Win32"
   "%DEVENV%" %SOLUTION_NAME% /Build "Release|Win32" /Project %PROJECT_NAME%
   if ERRORLEVEL 1 goto LABEL_ERROR
   )

if %BUILD_X64_DEBUG%==Y (
   echo Making %SOLUTION_NAME%: %PROJECT_NAME% - "Debug|x64"
   "%DEVENV%" %SOLUTION_NAME% /Build "Debug|x64" /Project %PROJECT_NAME%
   if ERRORLEVEL 1 goto LABEL_ERROR
   )

if %BUILD_X64_RELEASE%==Y (
   echo Making %SOLUTION_NAME%: %PROJECT_NAME% - "Release|x64"
   "%DEVENV%" %SOLUTION_NAME% /Build "Release|x64" /Project %PROJECT_NAME%
   if ERRORLEVEL 1 goto LABEL_ERROR
   )

:OK

echo Ok.
exit /B 0

:LABEL_BAD_PARAMETERS
echo Usage:
echo    project_make_all.cmd solution_name project_name
echo Example:
echo    project_make_all.cmd solution.sln project
exit 1

:LABEL_SOLUTION_NOT_EXISTS
echo Error! Can't find solution file: %SOLUTION_NAME%
exit 1

:LABEL_ERROR
echo Error making project!
exit 1