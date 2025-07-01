TEMPLATE = subdirs
SUBDIRS += lib mapsspider

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
mapsspider.depends = lib
