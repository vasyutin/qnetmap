@echo off
set MAKE_BIN=mingw32-make.exe

mkdir ..\..\build > nul 2> nul
cd ..\..\build
qmake ..\projects\qmake\qnetmap.pro
%MAKE_BIN%