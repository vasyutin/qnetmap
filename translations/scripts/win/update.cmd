@echo off
if %QTDIR%_==_ (
   echo Error! The environment variable QTDIR is not defined.
   exit /B 1
   )
%QTDIR%\bin\lupdate.exe ..\..\..\lib ..\..\..\mapsspider ..\..\..\mapsviewer -no-obsolete -codecfortr UTF-8 -ts ..\..\qnetmap_ru.ts