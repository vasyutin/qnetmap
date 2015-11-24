@echo off

set MAKER=..\..\scripts\project_make_all.cmd

call %MAKER% qnetmap.sln qnetmap %1

call %MAKER% qnetmap.sln qnetmap_raster_map %1
call %MAKER% qnetmap.sln qnetmap_empty_map %1
call %MAKER% qnetmap.sln qnetmap_universaltile_map %1

call %MAKER% qnetmap.sln mapsviewer %1
call %MAKER% qnetmap.sln mapsspider %1
call %MAKER% qnetmap.sln spider_task_creator %1
