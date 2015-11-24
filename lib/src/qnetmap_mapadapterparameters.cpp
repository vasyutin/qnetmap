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

#include "qnetmap_mapadapterparameters.h"
#include "qnetmap_xmltagnames.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap 
{
   //---------------------------------------------------------------------------------
   TMapAdapterParameters::TMapAdapterParameters()
      : m_Transparency(TMapAdapter::Consts::OpaqueMap),
        m_ProjectionType(TMapAdapter::Consts::ProjectionTypeMercator),
        m_Basis(TMapAdapter::Consts::ProjectionBasisEPSG3857), 
        m_TileSizeX(256), m_TileSizeY(256),
        m_TilesNumbering(TMapAdapter::Consts::TilesNumbering00TopLeft),
        m_ZoomMinimum(0), m_ZoomMaximum(15), m_ZoomShift(0),
        m_ZoomMinimumSet(false), m_ZoomMaximumSet(false),
        m_Updatability(TMapAdapter::Consts::QueryUpdatabilityStatic) {}

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setName( const QString& Name_ )
   { 
   m_Name = Name_; 
   return !m_Name.isEmpty(); 
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setDescription( const QString& Description_ )
   { 
   m_Description = Description_;
   return !m_Description.isEmpty(); 
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setType(const uint Type_)
   {
   if(Type_ == TMapAdapter::Consts::TypeRoadMap   ||
      Type_ == TMapAdapter::Consts::TypeSchemeMap ||
      Type_ == TMapAdapter::Consts::TypeInfoMap   ||
      Type_ == TMapAdapter::Consts::TypeSatelliteMap) {
      //
      m_Type = Type_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setType(const QString& Type_)
   {
   if(Type_ == MAPADAPTER_TYPE_ROADMAP)          return setType(TMapAdapter::Consts::TypeRoadMap);
   else if(Type_ == MAPADAPTER_TYPE_SCHEME)      return setType(TMapAdapter::Consts::TypeSchemeMap);
   else if(Type_ == MAPADAPTER_TYPE_SATELLITE)   return setType(TMapAdapter::Consts::TypeSatelliteMap);
   else if(Type_ == MAPADAPTER_TYPE_INFORMATION) return setType(TMapAdapter::Consts::TypeInfoMap);
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTransparency(const uint Transparency_)
   {
   if(Transparency_ == TMapAdapter::Consts::TransparentMap ||
      Transparency_ == TMapAdapter::Consts::OpaqueMap) {
      //
      m_Transparency = Transparency_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTransparency(const QString& Transparency_)
   {
   if(Transparency_ == MAPADAPTER_TRANSPARENSY_TRANSPARENT) 
      return setTransparency(TMapAdapter::Consts::TransparentMap);
   else if(Transparency_ == MAPADAPTER_TRANSPARENSY_OPAQUE) 
      return setTransparency(TMapAdapter::Consts::OpaqueMap);
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setProjectionType(const uint ProjectionType_)
   {
   if(ProjectionType_ == TMapAdapter::Consts::ProjectionTypeMercator) {
      m_ProjectionType = ProjectionType_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setProjectionType(const QString& ProjectionType_)
   {
   if(ProjectionType_ == MAPADAPTER_PROJECTIONTYPE_MERCATOR)
      return setProjectionType(TMapAdapter::Consts::ProjectionTypeMercator);
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setBasis(const uint Basis_)
   {
   if(Basis_ == TMapAdapter::Consts::ProjectionBasisEPSG3395   ||
      Basis_ == TMapAdapter::Consts::ProjectionBasisEPSG3857) {
      //
      m_Basis = Basis_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setBasis(const QString& Basis_)
   {
   if(Basis_ == MAPADAPTER_PROJECTIONBASIS_EPSG3395)
      return setBasis(TMapAdapter::Consts::ProjectionBasisEPSG3395);
   else if(Basis_ == MAPADAPTER_PROJECTIONBASIS_EPSG900913)
      return setBasis(TMapAdapter::Consts::ProjectionBasisEPSG3857);
   else if(Basis_ == MAPADAPTER_PROJECTIONBASIS_EPSG3857)
      return setBasis(TMapAdapter::Consts::ProjectionBasisEPSG3857);
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTileSizeX(const uint TileSizeX_)
   {
   if(TileSizeX_ > 0) {
      m_TileSizeX = TileSizeX_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTileSizeY(const uint TileSizeY_)
   {
   if(TileSizeY_ > 0) {
      m_TileSizeY = TileSizeY_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTilesNumbering(const uint TilesNumbering_)
   {
   if(TilesNumbering_ == TMapAdapter::Consts::TilesNumbering00TopLeft    ||
      TilesNumbering_ == TMapAdapter::Consts::TilesNumbering00BottomLeft ||
      TilesNumbering_ == TMapAdapter::Consts::TilesNumberingYahoo) {
      //
      m_TilesNumbering = TilesNumbering_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setTilesNumbering(const QString& TilesNumbering_)
   {
   if(TilesNumbering_ == MAPADAPTER_TILESNUMBERING_00TOPLEFT)
      return setTilesNumbering(TMapAdapter::Consts::TilesNumbering00TopLeft);
   else if(TilesNumbering_ == MAPADAPTER_TILESNUMBERING_00BOTTOMLEFT)
      return setTilesNumbering(TMapAdapter::Consts::TilesNumbering00BottomLeft);
   else if(TilesNumbering_ == MAPADAPTER_TILESNUMBERING_YAHOO)
      return setTilesNumbering(TMapAdapter::Consts::TilesNumberingYahoo);
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setZoomMinimum(const int ZoomMinimum_)
   {
   int MaximumLimit = m_ZoomMaximumSet ? m_ZoomMaximum : 50;
   if(ZoomMinimum_ >= 0 && ZoomMinimum_ < MaximumLimit) {
      m_ZoomMinimum = ZoomMinimum_;
      m_ZoomMinimumSet = true;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setZoomMaximum(const int ZoomMaximum_)
   {
   int MinimumLimit = m_ZoomMinimumSet ? m_ZoomMinimum : 0;
   if(ZoomMaximum_ >= MinimumLimit) {
      m_ZoomMaximum = ZoomMaximum_;
      m_ZoomMaximumSet = true;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setZoomShift(const int ZoomShift_)
   {
   if(ZoomShift_ >= 0) {
      m_ZoomShift = ZoomShift_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::isValid(void) const 
   { 
   return !m_Name.isEmpty() && !m_Description.isEmpty() &&
          (!m_FolderName.isEmpty() || (m_Updatability == TMapAdapter::Consts::QueryUpdatabilityDynamic)) &&
          m_Type > 0 && m_Transparency > 0 &&
          m_ProjectionType > 0 && m_Basis > 0 &&
          m_TileSizeX > 0 && m_TileSizeY > 0 && m_TilesNumbering > 0 &&
          m_ZoomMinimum >= 0 && m_ZoomMaximum >= m_ZoomMinimum && m_ZoomShift >= 0;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setFolderName(const QString& FolderName_)
   { 
   m_FolderName = FolderName_; 
   return !m_FolderName.isEmpty(); 
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setUpdatability(const uint Updatability_)
   {
   if(Updatability_ == TMapAdapter::Consts::QueryUpdatabilityDynamic ||
      Updatability_ == TMapAdapter::Consts::QueryUpdatabilityStatic) {
      //
      m_Updatability = Updatability_;
      return true;
      }
   return false;
   }

   //---------------------------------------------------------------------------------
   bool TMapAdapterParameters::setUpdatability(const QString& Updatability_)
   {
   if(Updatability_ == MAPADAPTER_QUERYUPDATABILITY_DYNAMIC) 
      return setUpdatability(TMapAdapter::Consts::QueryUpdatabilityDynamic);
   else if(Updatability_ == MAPADAPTER_QUERYUPDATABILITY_STATIC) 
      return setUpdatability(TMapAdapter::Consts::QueryUpdatabilityStatic);
   return false;
   }
}
