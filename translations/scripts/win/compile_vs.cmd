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
mkdir ..\..\..\build\Win32 > nul 2> nul
mkdir ..\..\..\build\Win32\Debug > nul 2> nul
mkdir ..\..\..\build\Win32\Release > nul 2> nul
mkdir ..\..\..\build\x64 > nul 2> nul
mkdir ..\..\..\build\x64\Debug > nul 2> nul
mkdir ..\..\..\build\x64\Release > nul 2> nul

%QTDIR%\bin\lrelease.exe -compress ..\..\qnetmap_ru.ts -qm ..\..\..\build\Win32\Debug\qnetmap_ru.qm
copy ..\..\..\build\Win32\Debug\qnetmap_ru.qm ..\..\..\build\Win32\Release\qnetmap_ru.qm > nul
copy ..\..\..\build\Win32\Debug\qnetmap_ru.qm ..\..\..\build\x64\Debug\qnetmap_ru.qm > nul
copy ..\..\..\build\Win32\Debug\qnetmap_ru.qm ..\..\..\build\x64\Release\qnetmap_ru.qm > nul
