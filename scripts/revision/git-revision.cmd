rem This script gets the the commit count across all branches in the given repo.
rem Usage: git-revision <revision file>
rem The script runs from the repo's root

set CURRENT_VERSION_FILE=%TEMP%\qnetmap_version.%RANDOM%
git rev-list --all --count > %CURRENT_VERSION_FILE% 2> nul
set /p CURRENT_VERSION_TEXT= < %CURRENT_VERSION_FILE%
del %CURRENT_VERSION_FILE%
set /a CURRENT_VERSION=%CURRENT_VERSION_TEXT%

if %CURRENT_VERSION% == 0 goto NO_GIT

echo %CURRENT_VERSION%


:NO_GIT
