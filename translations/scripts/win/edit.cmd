@echo off
if %QTDIR%_==_ (
   echo Error! The environment variable QTDIR is not defined.
   exit /B 1
   )
start %QTDIR%\bin\linguist.exe ..\..\qnetmap_ru.ts