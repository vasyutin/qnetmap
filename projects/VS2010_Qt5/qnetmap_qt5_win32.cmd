@echo off
call ..\..\setvars_qt5_VS2010.cmd
set PATH=%QTDIR%\bin.win32;%QTDIR%\plugins.win32;%PATH%
rem set QT_DEBUG_PLUGINS=1
call qnetmap_qt5.sln
