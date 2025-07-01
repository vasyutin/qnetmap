TEMPLATE = subdirs
SUBDIRS += lib universaltilesmap

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
universaltilesmap.depends = lib
