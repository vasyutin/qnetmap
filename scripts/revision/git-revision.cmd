rem This script gets the the commit count across all branches in the given repo.
rem Usage: git-revision <revision file>
rem The script runs from the repo's root

set CURRENT_VERSION_FILE=%TEMP%\qnetmap_version.%RANDOM%
git rev-list --all --count > %CURRENT_VERSION_FILE% 2> nul
set CURRENT_VERSION_TEXT=###
set /p CURRENT_VERSION_TEXT= < %CURRENT_VERSION_FILE%
del %CURRENT_VERSION_FILE%
set /a CURRENT_VERSION=%CURRENT_VERSION_TEXT%

if %CURRENT_VERSION% == 0 goto NO_GIT

if NOT EXIST %1 (
   echo %CURRENT_VERSION% > %1
   goto END
)

set FILE_VERSION_TEXT=###
set /p FILE_VERSION_TEXT= < %1
set /a FILE_VERSION=%FILE_VERSION_TEXT%

if NOT %CURRENT_VERSION% == %FILE_VERSION% echo %CURRENT_VERSION% > %1
goto END

:NO_GIT
rem If there's no file with the version number, we have to create it.
if NOT EXIST %1 (echo 0 > %1)

:END
