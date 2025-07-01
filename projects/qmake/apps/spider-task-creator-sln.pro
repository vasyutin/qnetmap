TEMPLATE = subdirs
SUBDIRS += lib spider_task_creator

lib.subdirs = ../lib
lib.file = ../lib/lib.pro
spider_task_creator.depends = lib
