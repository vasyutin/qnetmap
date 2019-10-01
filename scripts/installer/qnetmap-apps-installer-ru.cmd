rem Create binary distribution in PRODUCTION_DIR folder

if %QNETMAP%_==_ (
   echo Error! The environment variable QNETMAP is not defined.
   exit /B 1
   )

if %QTDIR%_==_ (
   echo Error! The environment variable QTDIR is not defined.
   exit /B 1
   )

if %PRODUCTION_DIR%_==_ (
   echo Error! The environment variable PRODUCTION_DIR is not defined.
   exit /B 1
   )

set ITEXT=0
set /p ITEXT= < %QNETMAP%\lib\src\qnetmap_version_major.h
set /a QNETMAP_VERSION=%ITEXT%

set ITEXT=0
set /p ITEXT= < %QNETMAP%\lib\src\qnetmap_version_minor.h
set /a ITEXT=%ITEXT%
set QNETMAP_VERSION=%QNETMAP_VERSION%.%ITEXT%

set ITEXT=0
set /p ITEXT= < %QNETMAP%\version\qnetmap_revision.h
set /a ITEXT=%ITEXT%
set QNETMAP_VERSION=%QNETMAP_VERSION%.%ITEXT%

"%ProgramFiles(x86)%\Inno Setup 6\Compil32.exe" /cc qnetmap-apps-installer-ru.iss
