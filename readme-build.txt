The QNetMap can be build in different ways depending of what build tools you are using. The standard distribution contains projects files for QMake/Qt Creator (for both Windows and Linux) and Visual Studio (2008 with Qt4, 2010 with Qt5, 2013 with Qt5).

1. Building with QMake

QMake build project was tested Qt 


2. Building with Qt Creator


3. Building with Visual Studio 2008 and Qt4

5. Building with Visual Studio 2010 and Qt5

6. Building with Visual Studio 2013 and Qt5

7. Building Russian translation

If you want to build a Russian translation for the QNetMap you can run the script which is placed in the folder /translations. The script /translations/win_compile_translations.cmd is user for Windows and script /translations/lin_compile_translations for Linux. These scripts compile the translation file named qnetmap_ru.ts and save it in the binaries folder.