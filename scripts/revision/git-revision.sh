# /bin/sh
#
rem This script gets the the commit count across all branches in the given repo.
rem Usage: git-revision <revision file>
rem The script runs from the repo's root

REVISION_FILE=$1
REVISION_HASH_FILE=${REVISION_FILE}ash

rem Create ./version folder if absent
if NOT EXIST version (
   mkdir version
)

set CURRENT_VERSION_FILE=%TEMP%\qnetmap_version.%RANDOM%

git rev-list --all --count > %CURRENT_VERSION_FILE% 2> nul
set CURRENT_VERSION_TEXT=###
set /p CURRENT_VERSION_TEXT= < %CURRENT_VERSION_FILE%
del %CURRENT_VERSION_FILE%
set /a CURRENT_VERSION=%CURRENT_VERSION_TEXT%

if %CURRENT_VERSION% == 0 goto NO_GIT

if NOT EXIST %REVISION_FILE% (
   echo %CURRENT_VERSION% > %REVISION_FILE%
   echo | set /p"=0x" > %REVISION_HASH_FILE%
   git rev-parse --short=10 HEAD >> %REVISION_HASH_FILE%
   goto END
)

set FILE_VERSION_TEXT=###
set /p FILE_VERSION_TEXT= < %REVISION_FILE%
set /a FILE_VERSION=%FILE_VERSION_TEXT%

if NOT %CURRENT_VERSION% == %FILE_VERSION% (
   echo %CURRENT_VERSION% > %REVISION_FILE%
   echo | set /p"=0x" > %REVISION_HASH_FILE%
   git rev-parse --short=10 HEAD >> %REVISION_HASH_FILE%
)
goto END

:NO_GIT
rem If there's no file with the version number, we have to create it.
if NOT EXIST %REVISION_FILE% (
   echo 0 > %REVISION_FILE%
   echo 0x0000000000 > %REVISION_HASH_FILE%
)

:END


