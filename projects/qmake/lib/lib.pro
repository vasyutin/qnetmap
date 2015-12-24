TEMPLATE = lib
CONFIG += dll
QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = qnetmap
DEFINES += QNETMAP_LIB

#DESTDIR = ../../../bin

#CONFIG(release, debug|release) {
#   DESTDIR = $${OUT_PWD}/bin/Release
#   OBJECTS_DIR = $${OUT_PWD}/tmp/Release/obj
#   MOC_DIR = $${OUT_PWD}/tmp/Release/moc
#   PLUGINSDIR = $${OUT_PWD}/../plugins/bin/Relese
#   }
#CONFIG(debug, debug|release) {
#   DESTDIR = $${OUT_PWD}/bin/Debug
#   OBJECTS_DIR = $${OUT_PWD}/tmp/Debug/obj
#   MOC_DIR = $${OUT_PWD}/tmp/Debug/moc
#   PLUGINSDIR = $${OUT_PWD}/../plugins/bin/Debug
#   }

#UI_DIR += ./uic
#RCC_DIR += ./qrc




CONFIG(release, debug|release) {
    DESTDIR = $${OUT_PWD}/../bin/release
    OBJECTS_DIR = $${OUT_PWD}/release/obj
    MOC_DIR = $${OUT_PWD}/release/moc
    UI_DIR = $${OUT_PWD}/release/ui
    }

CONFIG(debug, debug|release) {
    DESTDIR = $${OUT_PWD}/../bin/debug
    OBJECTS_DIR = $${OUT_PWD}/debug/obj
    MOC_DIR = $${OUT_PWD}/debug/moc
    UI_DIR = $${OUT_PWD}/debug/ui
    }



#DEFINES += QT_NETWORK_LIB QNETMAP_LIB QT_NO_OPENSSL

INCLUDEPATH += ../../../lib/src\
    ../../../lib

#LIBS += -L"."
#DEPENDPATH += .
#QMAKE_POST_LINK = cp -f $${_PRO_FILE_PWD_}/../qnetmap.xml $${PLUGINSDIR}

include(lib.pri)
