@echo off
if %QTDIR%_==_ (
   echo Error! The environment variable QTDIR is not defined.
   exit /B 1
   )

@echo off
if NOT EXIST ..\..\qnetmap_ru.ts (
   echo Error! File qnetmap_ru.ts does not exist.
   exit 1
   )

mkdir ..\..\..\build > nul 2> nul
mkdir ..\..\..\build\release > nul 2> nul
mkdir ..\..\..\build\debug > nul 2> nul

%QTDIR%\bin\lrelease.exe -compress ..\..\qnetmap_ru.ts -qm ..\..\..\build\debug\qnetmap_ru.qm
copy ..\..\..\build\debug\qnetmap_ru.qm ..\..\..\build\release\qnetmap_ru.qm > nul
