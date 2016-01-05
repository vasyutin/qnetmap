TEMPLATE = subdirs
SUBDIRS += lib \
    plugins/emptymap \
    plugins/rastermap \
    plugins/universaltilesmap \
    apps/mapsviewer

emptymap.depends = lib
rastermap.depends = lib
universaltilesmap.depends = lib
mapsviewer.depends = lib
