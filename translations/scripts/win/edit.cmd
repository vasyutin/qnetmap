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

start %QTBIN%\linguist.exe ..\..\qnetmap_ru.ts