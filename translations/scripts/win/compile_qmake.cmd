@echo off
set QTBIN=%QTDIR%\bin

if %QTDIR%_ == _ (
   if %QTDIR64%_ == _ (
      echo Neither QTDIR of QTDIR64 are defined.
      exit 1
   ) else (
      set QTBIN=%QTDIR64%\bin
   )
)

@echo off
if NOT EXIST ..\..\qnetmap_ru.ts (
   echo Error! File qnetmap_ru.ts does not exist.
   exit 1
   )

mkdir ..\..\..\build > nul 2> nul
mkdir ..\..\..\build\release > nul 2> nul
mkdir ..\..\..\build\debug > nul 2> nul

%QTBIN%\lrelease.exe -compress ..\..\qnetmap_ru.ts -qm ..\..\..\build\debug\qnetmap_ru.qm
copy ..\..\..\build\debug\qnetmap_ru.qm ..\..\..\build\release\qnetmap_ru.qm > nul
