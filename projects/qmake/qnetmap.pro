TEMPLATE = subdirs
SUBDIRS += \
    lib \
    emptymap \
    rastermap \
    universaltilesmap \
    mapsviewer \
    spider_task_creator \
    mapsspider

lib.file = lib/lib.pro
emptymap.file = plugins/emptymap/emptymap.pro
emptymap.depends = lib
rastermap.file = plugins/rastermap/rastermap.pro
rastermap.depends = lib
universaltilesmap.file = plugins/universaltilesmap/universaltilesmap.pro
universaltilesmap.depends = lib
mapsviewer.file = apps/mapsviewer/mapsviewer.pro
mapsviewer.depends = lib
spider_task_creator.file = apps/spider_task_creator/spider_task_creator.pro
spider_task_creator.depends = lib
mapsspider.file = apps/mapsspider/mapsspider.pro
mapsspider.depends = lib
