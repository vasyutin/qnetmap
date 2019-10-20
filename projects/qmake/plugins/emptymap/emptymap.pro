TEMPLATE = lib
TARGET = qnetmap_empty_map

# Исправление кодировки 866 -> 1251 для компилятора VS
windows: {
	QMAKE_EXTRA_TARGETS += before_build makefilehook

	makefilehook.target = $(MAKEFILE)
	makefilehook.depends = .beforebuild

	PRE_TARGETDEPS += .beforebuild

	before_build.target = .beforebuild
	before_build.commands = chcp 1251 1> nul
}

CONFIG(release, debug|release) {
	DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/$$QT_ARCH/$$QT_VERSION/release
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/obj
	MOC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/moc
	UI_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/ui
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/release/obj
	DEFINES += NDEBUG
	}
CONFIG(debug, debug|release) {
	DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/$$QT_ARCH/$$QT_VERSION/debug
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/obj
	MOC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/moc
	UI_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/ui
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


