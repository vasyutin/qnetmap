TEMPLATE = subdirs
SUBDIRS += lib \
    plugins/emptymap \
    plugins/rastermap \
    plugins/universaltilesmap \
    apps/mapsviewer \
    apps/spider_task_creator \
    apps/mapsspider

emptymap.depends = lib
rastermap.depends = lib
universaltilesmap.depends = lib
mapsviewer.depends = lib
spider_task_creator.depends = lib
mapsspider.depends = lib
