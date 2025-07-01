TEMPLATE = subdirs
SUBDIRS += lib emptymap

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
emptymap.depends = lib
