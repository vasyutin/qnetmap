The QNetMap can be build in different ways depending of what build tools you are using. The standard distribution contains projects files for QMake/Qt Creator (for both Windows and Linux) and Visual Studio.

1. How to build with QMake

1.1. Simple way

Just navigate to the folder /project/qmake and run script build_linux.sh for Linux or build_windows.sh for Windows. To prevent spoiling the system all the compiled executables and libraries will be placed in the /build/release folder. This will not cause problems running apps under Windows. But under Linux having necessary shared libraries is not a common way. In that case you can run apps simply showing the system the path to the shared libraries in the command line, ex:

Or you can put all the shared libraries to the system folders used to store such libraries (see the ld man for more info).

We assum that you have Qt4 or Qt5 installed and all the necessary build tools like compiler, qmake, moc, uic and rcc are avaliable from the command line. this way of building qnetmap was tested under Debian with Qt4 and Qt5 and Qt5 community edition with minigw for Windows.

1.2. More complicated way

You can run qmake and make by yourself. Just navigate to the folder where you want the binaries and temporary files to be stored (/build is recomended) and qmake the main. pro file from withing the desired folder, ex:

qmake path to qnetmap.pro/qnetmap.pro

You will get generated Makefile in the desired folder. Then you can run make (or nmake if you are using Visual Studio) to initiate the build. If you want the binaries for the 'debug' configuration you can run make debug. The binaries for debug configuration will be stored into the /yorr folder/debug and for the release into the /your folder/release.

2. How to build with Qt Creator

Just open the main project file /projects/qmake/qnetmap.pro and start building it using Qt Creator controls. As mentioned above all the result binaries are placed in the /build/release or /build/debug folder depending on configuration. 

3. How to build with Visual Studio

By now we supply solutions for the following versions of Visual Studio. Assumed that you defined environment variable QTDIR for the 32-bit version of Qt. If yiu want to build with 64-bit version of Qt you have to define the variable QTDIR64 pointing to the root folder.

The Qt VS integrator is not necessaryto be installed.

Just open the right project for yours VS and Qt version.

The solution/projects for the newer/older versions of Qt and Visual Studio can be derived from the supplied.

5. Building with Visual Studio 2010 and Qt5

6. Building with Visual Studio 2013 and Qt5

7. Building Russian translation

If you want to build a Russian translation for the QNetMap you can make it by using the scripts in the folder 


run the script which is placed in the folder /translations. The script /translations/win_compile_translations.cmd is user for Windows and script /translations/lin_compile_translations for Linux. These scripts compile the translation file named qnetmap_ru.ts and save it in the binaries folder.


 (2008 with Qt4, 2010 with Qt5, 2013 with Qt5)