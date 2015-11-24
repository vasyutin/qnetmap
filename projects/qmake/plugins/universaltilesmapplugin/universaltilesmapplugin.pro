TEMPLATE = lib
TARGET = qnetmap_universaltile_map
CONFIG(release, debug|release) {
   DESTDIR = $${OUT_PWD}/../bin/Release
   OBJECTS_DIR = $${OUT_PWD}/tmp/Release/obj
   MOC_DIR = $${OUT_PWD}/tmp/Release/moc
   LIBS += -L"$${OUT_PWD}/../../lib/bin/Release" -lqnetmap
   }
CONFIG(debug, debug|release) {
   DESTDIR = $${OUT_PWD}/../bin/Debug
   OBJECTS_DIR = $${OUT_PWD}/tmp/Debug/obj
   MOC_DIR = $${OUT_PWD}/tmp/Debug/moc
   LIBS += -L"$${OUT_PWD}/../../lib/bin/Debug" -lqnetmap
   }
QT += core gui network xml
DEFINES += _WINDOWS QT_LARGEFILE_SUPPORT QT_DLL QNETMAP_LIB
INCLUDEPATH += ../../lib/src
DEPENDPATH += .
UI_DIR += ./uic
RCC_DIR += ./qrc
include(universaltilesmapplugin.pri)
exists( ../../qnetmap.xml ) {
   win32:system(copy /Y ..\..\qnetmap.xml $$replace(DESTDIR, /, \))
   unix:system(cp ../../qnetmap.xml $$DESTDIR)
   }
