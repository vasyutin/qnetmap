/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl EmptyMapAdapter code by Kai Winter
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

#ifndef QNETMAP_EMPTYMAPADAPTER_H
#define QNETMAP_EMPTYMAPADAPTER_H

#include "qnetmap_global.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_imagemanager.h"

namespace qnetmap
{
   //! MapAdapter which do not load map tiles.
   /*!
    * The EmptyMapAdapter can be used if QMapControl should not load any map tiles. This is useful if you
    * only want to display an image through a FixedImageOverlay e.g.
    *	@author Kai Winter <sergey.shumeyko@gmail.com>
    */
   class QNETMAP_EXPORT TEmptyMapAdapter : public TMapAdapter
   {
      Q_OBJECT

   public:
      //! Constructor.
      /*!
       * @param tileSize This parameter seems unnecessary for this type of MapAdaper on first sight. But since
       * this parameter defines the size of the offscreen image it could be used for a little performance
       * tuning (larger offscreen-images have to be redrawed less times).
       * @param minZoom the minimum zoom level
       * @param maxZoom the maximum zoom level
       */
      TEmptyMapAdapter(int tileSizeX_ = 256, int tileSizeY_ = 256, int minZoom = 0, int maxZoom = 30);
      virtual ~TEmptyMapAdapter();

   protected:
      virtual QString host(const bool Random_ = false) const { Q_UNUSED(Random_); return QString(); }
      virtual unsigned hostsCount(void) const { return 0; }
      virtual void setParameters(TMapAdapterParameters*) {}
      //! \brief 
      virtual void setImageManagerInstance(TImageManager* Instance_) { TImageManager::setInstance(Instance_); }
      virtual void setURL(const QString&, const QString&) {};
      virtual bool isGeocoded(void) const { return true; };
      virtual QPointF centerMap(void) const { return QPointF(0, 0); }
      virtual QPointF topLeftMap(void) const { return QPointF(-180, 90); }
      virtual QPointF bottomRightMap(void) const { return QPointF(180, -90); }
   };
}
#endif
