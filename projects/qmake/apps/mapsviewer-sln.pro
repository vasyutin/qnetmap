TEMPLATE = subdirs
SUBDIRS += lib mapsviewer

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
mapsviewer.depends = lib
