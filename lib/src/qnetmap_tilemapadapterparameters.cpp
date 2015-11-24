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

#include "qnetmap_tilemapadapterparameters.h"
#include "qnetmap_xmltagnames.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap 
{
   //////////////////////////////////////////////////////////////////////////
   TTileMapAdapterParameters::TTileMapAdapterParameters()
      : m_HostNumberMinimum(0), m_HostNumberMaximum(0),
        m_CheckMapVersion(false), m_HostIsTemplate(false),
        m_CoordinatesWidth(0), m_ZoomWidth(0)  {}

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setHostTemplate(const QString& HostTemplate_) 
   { 
   m_HostTemplate = HostTemplate_;
   if(!m_HostTemplate.isEmpty()) {
      m_HostIsTemplate = m_HostTemplate.contains("%1");
      return true;
      }
   return false; 
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setHostNumberMinimum(const uint HostNumberMinimum_)
   {
   m_HostNumberMinimum = HostNumberMinimum_;
   return true;
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setHostNumberMaximum(const uint HostNumberMaximum_)
   {
   m_HostNumberMaximum = HostNumberMaximum_;
   return true;
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setQueryTemplate(const QString& QueryTemplate_) 
   { 
   if(!QueryTemplate_.isEmpty() &&
      ((QueryTemplate_.contains("%1") && 
        QueryTemplate_.contains("%2") && 
        QueryTemplate_.contains("%3")) ||
      QueryTemplate_.contains("%5"))) {
      //
      m_QueryTemplate = QueryTemplate_;
      return true;
      }
   return false; 
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setDefaultMapVersion(const QString& DefaultMapVersion_) 
   { 
   m_DefaultMapVersion = DefaultMapVersion_;
   return !DefaultMapVersion_.isEmpty();
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setMapVersionUrl(const QString& MapVersionUrl_) 
   { 
   m_MapVersionUrl = MapVersionUrl_;
   m_CheckMapVersion =
      !(m_MapVersionUrl.isEmpty() || m_MapVersionPrefix.isEmpty() || m_MapVersionPostfix.isEmpty());
   return !MapVersionUrl_.isEmpty();
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setMapVersionPrefix(const QString& MapVersionPrefix_) 
   { 
   m_MapVersionPrefix = MapVersionPrefix_;
   m_CheckMapVersion =
      !(m_MapVersionUrl.isEmpty() || m_MapVersionPrefix.isEmpty() || m_MapVersionPostfix.isEmpty());
   return !MapVersionPrefix_.isEmpty();
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setMapVersionPostfix(const QString& MapVersionPostfix_) 
   { 
   m_MapVersionPostfix = MapVersionPostfix_;
   m_CheckMapVersion =
      !(m_MapVersionUrl.isEmpty() || m_MapVersionPrefix.isEmpty() || m_MapVersionPostfix.isEmpty());
   return !MapVersionPostfix_.isEmpty();
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::isValid(void) const 
   { 
   return TMapAdapterParameters::isValid() &&
          !m_HostTemplate.isEmpty() && !m_QueryTemplate.isEmpty() &&
          (!m_QueryTemplate.contains("%4") || 
           (m_QueryTemplate.contains("%4") && !m_DefaultMapVersion.isEmpty()));
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setCoordinatesWidth(const uint CoordinatesWidth_)
   {
   m_CoordinatesWidth = CoordinatesWidth_;
   return CoordinatesWidth_ > 0;
   }

   //---------------------------------------------------------------------------------
   bool TTileMapAdapterParameters::setZoomWidth(const uint ZoomWidth_)
   {
   m_ZoomWidth = ZoomWidth_;
   return ZoomWidth_ > 0;
   }
}
