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
if NOT EXIST qnetmap_ru.ts (
   echo Error! File qnetmap_ru.ts does not exist.
   exit 1
   )

mkdir ..\build > nul 2> nul
mkdir ..\build\Win32 > nul 2> nul
mkdir ..\build\Win32\Debug > nul 2> nul
mkdir ..\build\Win32\Release > nul 2> nul
mkdir ..\build\x64 > nul 2> nul
mkdir ..\build\x64\Debug > nul 2> nul
mkdir ..\build\x64\Release > nul 2> nul

%QTBIN%\lrelease.exe -compress qnetmap_ru.ts -qm ..\build\Win32\Debug\qnetmap_ru.qm
copy ..\build\Win32\Debug\qnetmap_ru.qm ..\build\Win32\Release\qnetmap_ru.qm > nul
copy ..\build\Win32\Debug\qnetmap_ru.qm ..\build\x64\Debug\qnetmap_ru.qm > nul
copy ..\build\Win32\Debug\qnetmap_ru.qm ..\build\x64\Release\qnetmap_ru.qm > nul
