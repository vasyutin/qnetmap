TEMPLATE = subdirs
SUBDIRS += lib rastermap

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
rastermap.depends = lib
