TEMPLATE = lib
TARGET = qnetmap_universaltile_map

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
	DEFINES += NDEBUG
	}
CONFIG(debug, debug|release) {
	DESTDIR = $${_PRO_FILE_PWD_}/../../../../build/$$QT_ARCH/$$QT_VERSION/debug
	OBJECTS_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/obj
	MOC_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/moc
	UI_DIR = $${OUT_PWD}/$$QT_ARCH/$$QT_VERSION/debug/ui
	}

LIBS += -L"$${DESTDIR}" -lqnetmap

QT += core gui network xml
INCLUDEPATH += ../../../../lib/src ../../../../lib ../../../../plugins/universaltilemap

include(universaltilesmap.pri)

# Copy qnetmap.xml to the binaries folder.
QNETMAPXML = $${_PRO_FILE_PWD_}/../../../../plugins/universaltilemap/xml/qnetmap.xml
qnetmapxmlcopy.name = Copy qnetmap.xml to bin folder
qnetmapxmlcopy.input = QNETMAPXML
qnetmapxmlcopy.depends = $$QNETMAPXML
qnetmapxmlcopy.output = $${DESTDIR}/qnetmap.xml
# Commands
win32: qnetmapxmlcopy.commands = copy /Y $$quote($$replace(_PRO_FILE_PWD_, /, \ )\..\..\..\..\plugins\universaltilemap\xml\qnetmap.xml) $$quote($$replace(DESTDIR, /, \ )) >nul
!win32: qnetmapxmlcopy.commands = cp -f $$quote($${_PRO_FILE_PWD_}/../../../../plugins/universaltilemap/xml/qnetmap.xml) $$quote($${DESTDIR})
#
qnetmapxmlcopy.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += qnetmapxmlcopy


