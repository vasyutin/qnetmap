TEMPLATE = lib
TARGET = qnetmap_empty_map

CONFIG(release, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/release
    OBJECTS_DIR = $${OUT_PWD}/release/obj
    MOC_DIR = $${OUT_PWD}/release/moc
    UI_DIR = $${OUT_PWD}/release/ui
    }
CONFIG(debug, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/debug
    OBJECTS_DIR = $${OUT_PWD}/debug/obj
    MOC_DIR = $${OUT_PWD}/debug/moc
    UI_DIR = $${OUT_PWD}/debug/ui
    }

LIBS += -L"$${DESTDIR}" -lqnetmap

#CONFIG(release, debug|release) {
#   DESTDIR = $${OUT_PWD}/../bin/Release
#   OBJECTS_DIR = $${OUT_PWD}/tmp/Release/obj
#   MOC_DIR = $${OUT_PWD}/tmp/Release/moc
#   LIBS += -L"$${OUT_PWD}/../../lib/bin/Release" -lqnetmap
#   }
#CONFIG(debug, debug|release) {
#   DESTDIR = $${OUT_PWD}/../bin/Debug
#   OBJECTS_DIR = $${OUT_PWD}/tmp/Debug/obj
#   MOC_DIR = $${OUT_PWD}/tmp/Debug/moc
#   LIBS += -L"$${OUT_PWD}/../../lib/bin/Debug" -lqnetmap
#   }

QT += core gui network xml
INCLUDEPATH += ../../../../lib/src ../../../../lib ../../../../plugins/emptymap

#DEFINES += _WINDOWS QT_LARGEFILE_SUPPORT QT_DLL QNETMAP_LIB
#INCLUDEPATH += ../../lib/src
#DEPENDPATH += .
#UI_DIR += ./uic
#RCC_DIR += ./rcc
include(emptymap.pri)


