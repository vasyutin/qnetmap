TEMPLATE = app
TARGET = mapsspider

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += NOTRAYICON

CONFIG(release, debug|release) {
	DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/$$QT_ARCH/$$QT_VERSION/release
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/obj
	MOC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/moc
	UI_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/ui
	RCC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/rcc
	DEFINES += NDEBUG
	}
CONFIG(debug, debug|release) {
	DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/$$QT_ARCH/$$QT_VERSION/debug
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/obj
	MOC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/moc
	UI_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/ui
	RCC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/rcc
	}

LIBS += -L"$${DESTDIR}" -lqnetmap
INCLUDEPATH += ../../../../lib/src ../../../../lib

win32:RC_FILE += ../../../../mapsspider/icon/mapsspider_icon.rc

include(mapsspider.pri)
