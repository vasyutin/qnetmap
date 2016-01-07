TEMPLATE = lib
CONFIG += dll
QT += core gui network xml printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = qnetmap
DEFINES += QNETMAP_LIB

CONFIG(release, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../build/release
    OBJECTS_DIR = $${OUT_PWD}/release/obj
    MOC_DIR = $${OUT_PWD}/release/moc
    UI_DIR = $${OUT_PWD}/release/ui
    }
CONFIG(debug, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../build/debug
    OBJECTS_DIR = $${OUT_PWD}/debug/obj
    MOC_DIR = $${OUT_PWD}/debug/moc
    UI_DIR = $${OUT_PWD}/debug/ui
    }

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



#DEFINES += QT_NETWORK_LIB QNETMAP_LIB QT_NO_OPENSSL

INCLUDEPATH += $${_PRO_FILE_PWD_}/../../../lib/src $${_PRO_FILE_PWD_}/../../../lib $${_PRO_FILE_PWD_}/../../../version

#LIBS += -L"."
#DEPENDPATH += .
#QMAKE_POST_LINK = cp -f $${_PRO_FILE_PWD_}/../qnetmap.xml $${PLUGINSDIR}

include(lib.pri)

# Create revision file if not exists
versionfile.target = $${_PRO_FILE_PWD_}/../../../version/qnetmap_revision.h
win32: versionfile.commands = cd $$quote($$replace(_PRO_FILE_PWD_, /, \ )\..\..\..) & call .\scripts\revision\git-revision.cmd .\version\qnetmap_revision.h
!win32: versionfile.commands = cd ../../.. & ./scripts/revision/git-revision ./version/qnetmap_revision.h
QMAKE_EXTRA_TARGETS += versionfile
PRE_TARGETDEPS += $${_PRO_FILE_PWD_}/../../../version/qnetmap_revision.h
