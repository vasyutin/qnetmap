TEMPLATE = app
TARGET = mapsspider

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += NOTRAYICON

CONFIG(release, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/release
    OBJECTS_DIR = $${OUT_PWD}/release/obj
    MOC_DIR = $${OUT_PWD}/release/moc
    UI_DIR = $${OUT_PWD}/release/ui
    RCC_DIR = $${OUT_PWD}/release/rcc
    }
CONFIG(debug, debug|release) {
    DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/debug
    OBJECTS_DIR = $${OUT_PWD}/debug/obj
    MOC_DIR = $${OUT_PWD}/debug/moc
    UI_DIR = $${OUT_PWD}/debug/ui
    RCC_DIR = $${OUT_PWD}/debug/rcc
    }

LIBS += -L"$${DESTDIR}" -lqnetmap
INCLUDEPATH += ../../../../lib/src ../../../../lib

win32:RC_FILE += ../../../../mapsspider/icon/mapsspider_icon.rc

include(mapsspider.pri)
