TEMPLATE = app
TARGET = mapsspider
CONFIG(release, debug|release) {
   DESTDIR = $${OUT_PWD}/bin/Release
   OBJECTS_DIR = $${OUT_PWD}/tmp/Release/obj
   MOC_DIR = $${OUT_PWD}/tmp/Release/moc
   RCC_DIR = $${OUT_PWD}/tmp/Release/qrc
   LIBS += -L"$${OUT_PWD}/../lib/bin/Release" -lqnetmap
   }
CONFIG(debug, debug|release) {
   DESTDIR = $${OUT_PWD}/bin/Debug
   OBJECTS_DIR = $${OUT_PWD}/tmp/Debug/obj
   MOC_DIR = $${OUT_PWD}/tmp/Debug/moc
   RCC_DIR = $${OUT_PWD}/tmp/Debug/qrc
   LIBS += -L"$${OUT_PWD}/../lib/bin/Debug" -lqnetmap
   }
QT += core gui network xml svg
DEFINES += NOTRAYICON _WINDOWS QT_LARGEFILE_SUPPORT QT_DLL QT_SVG_LIB QT_XML_LIB QNETMAP_LIB
INCLUDEPATH += ./src ../lib/src
DEPENDPATH += .
UI_DIR += ./uic
include(mapsspider.pri)
