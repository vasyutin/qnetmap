@echo off
set QTDIR=c:\Qt\qt-5.3.1-msvc2008
if NOT EXIST qnetmap_ru.ts (
   echo Error! File qnetmap_ru.ts does not exist.
   exit 1
   )

if %QNETMAP%_==_ (
   echo Error! Environment variable QNETMAP is not set.
   exit 1
   )

mkdir "%QNETMAP_BIN%\Win32\Debug" > nul 2> nul
mkdir "%QNETMAP_BIN%\Win32\Release" > nul 2> nul
mkdir "%QNETMAP_BIN%\x64\Debug" > nul 2> nul
mkdir "%QNETMAP_BIN%\x64\Release" > nul 2> nul

%QTDIR%\bin\lrelease.exe -compress qnetmap_ru.ts -qm %QNETMAP_BIN%\Win32\Debug\qnetmap_ru.qm
copy %QNETMAP_BIN%\Win32\Debug\qnetmap_ru.qm %QNETMAP_BIN%\Win32\Release\qnetmap_ru.qm > nul
copy %QNETMAP_BIN%\Win32\Debug\qnetmap_ru.qm %QNETMAP_BIN%\x64\Debug\qnetmap_ru.qm > nul
copy %QNETMAP_BIN%\Win32\Debug\qnetmap_ru.qm %QNETMAP_BIN%\x64\Release\qnetmap_ru.qm > nul
