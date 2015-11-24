/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will `be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNETMAP_XMLTAGNAMES_H
#define QNETMAP_XMLTAGNAMES_H

#include "qnetmap_global.h"

// XML-файл заданий на загрузку карт
// имена тегов обрабатываемого XML-файла
#define SPIDER_TAG_JOBDOCUMENT              "MapsSpiderJob"
#define SPIDER_TAG_MAP                      "Map"
#define SPIDER_TAG_AREA                     "Area"
#define SPIDER_TAG_POINT                    "Point"
// имена параметров тегов обрабатываемого XML-файла
#define SPIDER_PARAMETER_REQUESTSCOUNT      "SimultaneousRequestsCount"
#define SPIDER_PARAMETER_PLUGINNAME         "PluginName"
#define SPIDER_PARAMETER_ZOOM               "Zoom"
#define SPIDER_PARAMETER_LONGITUDE          "Longitude"
#define SPIDER_PARAMETER_LATITUDE           "Latitude"

// XML-файл параметров плагинов карт
// имена тегов обрабатываемого XML-файла
#define XML_TAG_QNETMAP                    "QNetMap"
#define XML_TAG_TILEMAPADAPTERS            "TileMapAdapters"
#define XML_TAG_TILEMAPADAPTER             "TileMapAdapter"
#define XML_TAG_GEOPARAMETERS              "GeoParameters"
#define XML_TAG_PROJECTION                 "Projection"
#define XML_TAG_TILES                      "Tiles"
#define XML_TAG_ZOOMS                      "Zooms"
#define XML_TAG_URL                        "Url"
#define XML_TAG_HOST                       "Host"
#define XML_TAG_QUERY                      "Query"
#define XML_TAG_MAPVERSION                 "MapVersion"
#define XML_TAG_SOURCE                     "Source"
#define XML_TAG_HTTP                       "Http"
#define XML_TAG_HTTPFIELDS                 "HttpFields"
// имена параметров тегов обрабатываемого XML-файла
#define XML_PARAMETER_ADAPTERNAME          "Name"
#define XML_PARAMETER_ADAPTERDESCRIPTION   "Description"
#define XML_PARAMETER_ADAPTERTYPE          "Type"
#define XML_PARAMETER_ADAPTERTRANSPARENCY  "Transparency"
#define XML_PARAMETER_ADAPTERFOLDERNAME    "FolderName"
#define XML_PARAMETER_PROJECTIONTYPE       "Type"
#define XML_PARAMETER_PROJECTIONBASIS      "Basis"
#define XML_PARAMETER_TILESSIZEX           "SizeX"
#define XML_PARAMETER_TILESSIZEY           "SizeY"
#define XML_PARAMETER_TILESNUMBERING       "Numbering"
#define XML_PARAMETER_ZOOMMINIMUM          "Minimum"
#define XML_PARAMETER_ZOOMMAXIMUM          "Maximum"
#define XML_PARAMETER_ZOOMSHIFT            "Shift"
#define XML_PARAMETER_HOSTTEMPLATE         "Template"
#define XML_PARAMETER_HOSTMINIMUM          "Minimum"
#define XML_PARAMETER_HOSTMAXIMUM          "Maximum"
#define XML_PARAMETER_QUERYTEMPLATE        "Template"
#define XML_PARAMETER_UPDATABILITY         "Updatability"
#define XML_PARAMETER_DEFAULTMAPVERSION    "Default"
#define XML_PARAMETER_SOURCEURL            "Url"
#define XML_PARAMETER_SOURCEPREFIX         "Prefix"
#define XML_PARAMETER_SOURCEPOSTFIX        "Postfix"
#define XML_PARAMETER_COORDINATESWIDTH     "CoordinatesWidth"
#define XML_PARAMETER_ZOOMWIDTH            "ZoomWidth"
#define XML_PARAMETER_USERAGENT            "UserAgent"
// имена параметров адаптеров карт
#define MAPADAPTER_TYPE_SCHEME                 "Scheme"
#define MAPADAPTER_TYPE_SATELLITE              "Satellite"
#define MAPADAPTER_TYPE_ROADMAP                "RoadMap"
#define MAPADAPTER_TYPE_INFORMATION            "Information"
#define MAPADAPTER_TRANSPARENSY_OPAQUE         "Opaque"
#define MAPADAPTER_TRANSPARENSY_TRANSPARENT    "Transparent"
#define MAPADAPTER_PROJECTIONTYPE_MERCATOR     "Mercator"
#define MAPADAPTER_PROJECTIONBASIS_EPSG3857    "EPSG:3857"
#define MAPADAPTER_PROJECTIONBASIS_EPSG3395    "EPSG:3395"
#define MAPADAPTER_PROJECTIONBASIS_EPSG900913  "EPSG:900913"
#define MAPADAPTER_TILESNUMBERING_00TOPLEFT    "00TopLeft"
#define MAPADAPTER_TILESNUMBERING_YAHOO        "Yahoo"
#define MAPADAPTER_TILESNUMBERING_00BOTTOMLEFT "00BottomLeft"
#define MAPADAPTER_QUERYUPDATABILITY_STATIC    "Static"
#define MAPADAPTER_QUERYUPDATABILITY_DYNAMIC   "Dynamic"

#endif //QNETMAP_XMLTAGNAMES_H
