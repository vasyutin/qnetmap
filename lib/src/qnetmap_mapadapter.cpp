/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl MapAdapter code by Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#include "qnetmap_mapadapter.h"
#include "qnetmap_imagemanager.h"

#include <QObject>
#include <QUuid>
#include <QLocale>
#include <QDebug>
#include <cmath> 
#include <QSettings>

namespace qnetmap
{
   qreal TMapAdapter::PI = acos(-1.0);

   QSet<TMapAdapter*> TMapAdapter::m_ActiveMapAdapters;
   QMutex             TMapAdapter::m_ActiveMapAdaptersMutex;

   /////////////////////////////////////////////////////////////////////////////////////////
   TMapAdapter::TMapAdapter(int TileSizeX_, int TileSizeY_, int MinZoom_, int MaxZoom_):
        m_Transparency(Consts::OpaqueMap),
        m_SourceType(Consts::InternetMap),
        m_IsInitialized(false),
        m_Description(QNetMapTranslator::tr("Description is not set.")),
        m_MapControl(NULL),
        m_PluginName(QNetMapTranslator::tr("Name is not set.")),
        m_TemplateMappingStoragePath("%1/notnamed"),
        m_MappingStoragePath("."),
        m_ProjectionType(Consts::ProjectionTypeMercator),
        m_Basis(Consts::ProjectionBasisEPSG3857),
        m_TilesNumbering(Consts::TilesNumbering00TopLeft),
        m_ZoomShift(0),
        m_Updatability(Consts::QueryUpdatabilityStatic),
        m_MinZoom(MinZoom_),
        m_MaxZoom(MaxZoom_),
        m_TileSizeX(TileSizeX_),
        m_TileSizeY(TileSizeY_),
        m_CurrentZoom(-1),
        m_NumberOfXTiles(1),
        m_NumberOfYTiles(1),
        m_Locale(QLocale(QLocale::English))
   {
   // save the pointer to the object in the list of "active"
   m_ActiveMapAdapters.insert(this);
   }

   /////////////////////////////////////////////////////////////////////////////////////////
   TMapAdapter::TMapAdapter(const TMapAdapter& Source_): TBase()
   {
   m_ProjectionType = Source_.m_ProjectionType;
   m_Basis = Source_.m_Basis;
   m_TilesNumbering = Source_.m_TilesNumbering;
   m_ZoomShift = Source_.m_ZoomShift;
   m_StorageType = Source_.m_StorageType;
   m_Transparency = Source_.m_Transparency;
   m_SourceType = Source_.m_SourceType;
   m_MapType = Source_.m_MapType;
   m_Description = Source_.m_Description;
   m_MapControl = Source_.m_MapControl;
   m_PluginName = Source_.m_PluginName;
   m_TileSizeX = Source_.m_TileSizeX;
   m_TileSizeY = Source_.m_TileSizeY;
   m_MinZoom = Source_.m_MinZoom;
   m_MaxZoom = Source_.m_MaxZoom;
   m_CurrentZoom = Source_.m_CurrentZoom;
   m_NumberOfXTiles = Source_.m_NumberOfXTiles;
   m_NumberOfYTiles = Source_.m_NumberOfYTiles;
   m_Locale = Source_.m_Locale;
   m_FolderName = Source_.m_FolderName;
   m_MappingStoragePath = Source_.m_MappingStoragePath;
   m_TemplateMappingStoragePath = Source_.m_TemplateMappingStoragePath;
   m_Updatability = Source_.m_Updatability;
   //
   m_IsInitialized = Source_.m_IsInitialized;
   m_UUID = m_PluginName + QUuid::createUuid().toString();
   }

   /////////////////////////////////////////////////////////////////////////////////////////
   TMapAdapter::~TMapAdapter()
   {
   // delete a pointer to an object from the list of "active"
   m_ActiveMapAdapters.remove(this);
   }

   //---------------------------------------------------------------------------
   void TMapAdapter::setParameters(TMapAdapterParameters* Parameters_)
   {
   setPluginName(Parameters_->name());
   setDescription(Parameters_->description());
   setMapType(Parameters_->type());
   setTransparency(Parameters_->transparency());
   m_TileSizeX = Parameters_->tileSizeX();
   m_TileSizeY = Parameters_->tileSizeY();
   m_MaxZoom = Parameters_->zoomMaximum();
   m_MinZoom = Parameters_->zoomMinimum();
   m_ProjectionType = Parameters_->projectionType();
   m_Basis = Parameters_->basis();
   m_TilesNumbering = Parameters_->tilesNumbering();
   m_ZoomShift = Parameters_->zoomShift();
   m_FolderName = Parameters_->folderName();
   m_Updatability = Parameters_->updatability();
   if(!m_FolderName.isEmpty()) m_TemplateMappingStoragePath = "%1/" + m_FolderName;
   }

   //---------------------------------------------------------------------------
   void TMapAdapter::setZoomRestrictions(const int Min_, const int Max_)
   {
   m_MinZoom = Min_;
   m_MaxZoom = Max_;
   }

   //---------------------------------------------------------------------------
   int TMapAdapter::tileSizeX() const
   {
   return m_TileSizeX;
   }

   //---------------------------------------------------------------------------
   int TMapAdapter::tileSizeY() const
   {
   return m_TileSizeY;
   }

   //---------------------------------------------------------------------------
   QRect TMapAdapter::tileRect(int X_, int Y_) const
   {
   int TileSizeX = tileSizeX();
   int TileSizeY = tileSizeY();
   return QRect(QPoint(X_ * TileSizeX + 1, Y_ * TileSizeY + 1), QSize(TileSizeX, TileSizeY));
   }

   //---------------------------------------------------------------------------
   int TMapAdapter::minZoom() const
   {
   return m_MinZoom;
   }

   //---------------------------------------------------------------------------
   int TMapAdapter::maxZoom() const
   {
   return m_MaxZoom;
   }

   //---------------------------------------------------------------------------
   int TMapAdapter::currentZoom() const
   {
   return m_CurrentZoom;
   }

   //------------------------------------------------------------------
   qreal TMapAdapter::deg_rad(qreal x) const
   {
   return x * (PI / 180.0);
   }

   //------------------------------------------------------------------
   qreal TMapAdapter::rad_deg(qreal x) const
   {
   return x * (180.0 / PI);
   }

   //----------------------------------------------------------------------
   QSize TMapAdapter::mapSize(void) const
   {
   return QSize((1 << m_CurrentZoom) * tileSizeX(), (1 << m_CurrentZoom) * tileSizeY());
   }

   //--------------------------------------------------------------------------------------
   void TMapAdapter::setPluginName(const QString& Name_) 
   { 
   m_PluginName = Name_; 
   // unique object identifier
   // add to the UUID of a name for the correct sorting lists
   m_UUID = Name_ + QUuid::createUuid().toString();
   }

   //--------------------------------------------------------------------------------------
   bool TMapAdapter::mapAdapterIsActive(TMapAdapter* MapAdapter_)
   {
   QMutexLocker Locker(&m_ActiveMapAdaptersMutex);
   return m_ActiveMapAdapters.find(MapAdapter_) != m_ActiveMapAdapters.end();
   }

   //--------------------------------------------------------------------------------------
   void TMapAdapter::setTransparency(const uint Transparency_)
   {
   if(Transparency_ == Consts::OpaqueMap || Transparency_ == Consts::TransparentMap)
      m_Transparency = Transparency_; 
   else
      assert(!"Transparency card set incorrectly");
   }

   //--------------------------------------------------------------------------------------
   void TMapAdapter::setSourceType(const uint SourceType_)
   { 
   if(SourceType_ == Consts::EmptyMap    ||
      SourceType_ == Consts::InternetMap ||
      SourceType_ == Consts::RasterMap   ||
      SourceType_ == Consts::UniversalTileMap)
      m_SourceType = SourceType_; 
   else
      assert(!"Type of source maps is incorrect");
   }

   //--------------------------------------------------------------------------------------
   void TMapAdapter::setMapType(const uint MapType_)
   { 
   if(MapType_ == Consts::TypeEmptyMap  ||
      MapType_ == Consts::TypeRasterMap ||
      MapType_ == Consts::TypeRoadMap   ||
      MapType_ == Consts::TypeSchemeMap ||
      MapType_ == Consts::TypeInfoMap   ||
      MapType_ == Consts::TypeSatelliteMap) m_MapType = MapType_; 
   else
      assert(!"Map type is incorrect");
   }

   //--------------------------------------------------------------------------------------
   qreal TMapAdapter::pixelXScale(int Zoom_) const
   {
   if(Zoom_ < 0) return -1;
   // world wide in meters divided by the total number of pixels
   return (2. * PI * QNetMapConsts::GoogleMapsSphereRadius) / (xtilesonzoomlevel(Zoom_) * tileSizeX());
   }

   //--------------------------------------------------------------------------------------
   qreal TMapAdapter::pixelYScale(int Zoom_) const 
   { 
   if(Zoom_ < 0) return -1;
   // world wide in meters divided by the total number of pixels
   return (2. * PI * QNetMapConsts::GoogleMapsSphereRadius) / (ytilesonzoomlevel(Zoom_) * tileSizeY());
   }

   //--------------------------------------------------------------------------------------
   void TMapAdapter::setMappingStoragePath(const QString& Path_, const bool InsertIntoATemplate_)
   {
   if(InsertIntoATemplate_ && m_TemplateMappingStoragePath.contains("%1"))
      m_MappingStoragePath = m_TemplateMappingStoragePath.arg(Path_);
   else  m_MappingStoragePath = Path_;
   }

   //----------------------------------------------------------------------
   QPoint TMapAdapter::coordinateToDisplay( const QPointF& Point_, int Zoom_ /*= -1*/ ) const
   {
   Q_UNUSED(Zoom_)
   qreal x = (Point_.x()+ 180.) * (numberOfXTiles() * tileSizeX()) / 360.; // coord to pixel!
   qreal y = (1. - (log(tan(PI * 0.25 + deg_rad(Point_.y()) * 0.5)) / PI)) * 0.5 * (numberOfYTiles() * tileSizeY());

   return QPoint(int(x), int(y));
   }

   //----------------------------------------------------------------------
   QPointF TMapAdapter::displayToCoordinate(const QPoint& point) const
   {
   qreal longitude = ((qreal)point.x() * (360. / (numberOfXTiles() * tileSizeX()))) - 180.;
   qreal latitude  = rad_deg(atan(sinh((1. - (qreal)point.y() * (2. / (numberOfYTiles() * tileSizeY()))) * PI)));

   return QPointF(longitude, latitude);
   }

   //----------------------------------------------------------------------
   int TMapAdapter::xtilesonzoomlevel(int zoomlevel) const
   {
   int Zoom = zoomlevel;
   if(Zoom < 1) Zoom = 0;
   return 1 << Zoom;
   }

   //----------------------------------------------------------------------
   int TMapAdapter::ytilesonzoomlevel(int zoomlevel) const
   {
   int Zoom = zoomlevel;
   if(Zoom < 1) Zoom = 0;
   return 1 << Zoom;
   }

   //----------------------------------------------------------------------
   QPoint TMapAdapter::coordinateToTile(const QPointF& Point_) const
   {
   QPoint Point = coordinateToDisplay(Point_);
   return QPoint(Point.x() / tileSizeX(), Point.y() / tileSizeY());
   }

   //----------------------------------------------------------------------
   QPoint TMapAdapter::coordinateToTileLocal(const QPointF& Point_) const
   {
   QPoint Point = coordinateToDisplay(Point_);
   return QPoint(Point.x() % tileSizeX(), Point.y() % tileSizeY());
   }

   //------------------------------------------------------------------
   void TMapAdapter::zoom_in()
   {
   if(m_CurrentZoom < m_MaxZoom) m_CurrentZoom = m_CurrentZoom + 1;
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   }

   //------------------------------------------------------------------
   void TMapAdapter::zoom_out()
   {
   if(m_CurrentZoom > m_MinZoom) m_CurrentZoom = m_CurrentZoom - 1;
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   }

   //------------------------------------------------------------------
   QString TMapAdapter::query(int x, int y, int z) const
   {
   Q_UNUSED(x)
   Q_UNUSED(y)
   Q_UNUSED(z)
   return "";
   }

   //------------------------------------------------------------------
   bool TMapAdapter::isValid(int x, int y, int zoom) const
   {
   int MaxPixel = (1 << zoom) - 1;
   return !(x < 0 || x > MaxPixel || y < 0 || y > MaxPixel) &&
          zoom >= m_MinZoom && zoom <= m_MaxZoom;
   }

   //----------------------------------------------------------------------
   void TMapAdapter::setZoom(int ZoomLevel_)
   {
   m_CurrentZoom = ZoomLevel_ > m_MaxZoom ? m_MaxZoom : (ZoomLevel_ < m_MinZoom ? m_MinZoom : ZoomLevel_);
   
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   }
}
