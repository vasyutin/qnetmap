The QNetMap can be build in different ways depending of what build tools you are using. The standard distribution contains projects files for QMake/Qt Creator (for both Windows and Linux) and Visual Studio 2008/2013.

1. How to build with QMake

1.1. Simple way

Just navigate to the folder /projects/qmake and run script build_linux.sh for Linux or build_windows.sh for Windows. To prevent soiling the system all the compiled executables and libraries will be placed in the /build/release folder. This will not cause problems running sample applications under Windows. But under Linux having all the required shared libraries in the application folder is not a common way. In that case you can run the applications simply showing the system the path to the shared libraries in the command line, ex:

LD_LIBRARY_PATH=. ./mapsviewer

Or you can put all the shared libraries to the system folders used to store such libraries (see the 'man ld.so' for more info).

We assume that you have Qt4 or Qt5 installed and all the necessary build tools like compiler, 'qmake', 'moc', 'uic' and 'rcc' are available from the command line. This way of building QNetMap was tested under Debian 8 (Jessie) with Qt4 and Qt5 and with Qt Open Source 5.5.1 (with MinGW 4.9.2) for Windows.

1.2. More complicated way

You can run 'qmake' and 'make' by yourself. Just navigate to the folder where you want the binaries and temporary files to be stored (/build is recommended) and 'qmake' the main .pro file from withing the desired folder, ex:

qmake <path to /projects/qnetmap/qnetmap.pro>/qnetmap.pro

You will get generated 'Makefile' in the desired folder. Then you can run 'make' (or 'nmake' if you are using Visual Studio) to initiate the build. If you want the binaries for the 'debug' configuration you can run 'make debug'. The binaries for debug configuration will be stored into the <your folder>/debug and for the release into the <your folder>/release.

2. How to build with Qt Creator

Just open the main project file /projects/qmake/qnetmap.pro and start building it using Qt Creator controls. As mentioned above all the result binaries are placed in the /build/release or /build/debug folder depending on configuration. 

3. How to build with Visual Studio

By now we supply solution (.sln) with the project files (.vcproj) files for the Visual Studio 2008 with Qt4 and for the Visual Studio 2008 with Qt5. We assume that the environment variable QTDIR is defined for the 32-bit version of Qt. And it contains the path for the Qt root folder (thus the path to the Qt binaries is '%QTDIR%\bin'). If you want to build QNetMap against the 64-bit version of Qt you have to define the variable QTDIR64 pointing to the root folder of the 64-bit Qt distribution.

You don't have to install 'Visual Studio Add-in for Qt' to build QNetMap. More than that this add-in can unexpectedly change the solution/project's settings before the build process. Thus the usage of 'Visual Studio Add-in for Qt' is not recommended.

To build QNetMap open the proper 'solution' file (/projects/VS2008_Qt4/qnetmap.sln for Visual Studio 2008 with Qt4, /projects/VS2013_Qt5/qnetmap.sln for Visual Studio 2013 with Qt5 or /projects/VS2015_Qt5/qnetmap.sln for Visual Studio 2015 with Qt5) and start the build process.

The compiled binaries will be placed in the folder /build/<Platform>/<Configuration>, where the 'Platform' is 'Win32' or 'x64' and the 'Configuration' is 'Debug' or 'Release'.

4. How to build Russian translation

If you want to build a Russian translation for the QNetMap you can make it by using the scripts in the folder /translations/scripts. Under Windows use script /translations/scripts/win/compile_vs.cmd for the version build with Visual Studio and script /translations/scripts/win/compile_qmake.cmd for the version build with 'qmake'. Under Linux run script /translations/scripts/win/compile.sh. These scripts will compile the translation file named 'qnetmap_ru.ts' and save it in the binaries folder.