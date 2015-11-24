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

#ifndef QNETMAP_UNIVERSALTILEMAPADAPTER_H
#define QNETMAP_UNIVERSALTILEMAPADAPTER_H

#include "qnetmap_global.h"
#include "qnetmap_tilemapadapter.h"

namespace qnetmap
{
   //! \class TUniversalTileMapAdapter
   //! \brief Universal adapter for tiles map
   class QNETMAP_EXPORT TUniversalTileMapAdapter : public TTileMapAdapter
   {
      Q_OBJECT

   public:
      typedef TTileMapAdapter TParent;

      TUniversalTileMapAdapter();
      TUniversalTileMapAdapter(const TUniversalTileMapAdapter& Source_) : TTileMapAdapter(Source_) {}
      virtual ~TUniversalTileMapAdapter() {};

   private:
      QPointF wgs84CoordinateToMercator(QPointF Point) const;
      QPointF wgs84MercatorToCoordinate(QPointF Point) const;
      QPoint wgs84MercatorToTile(QPointF Point_) const;
      QPoint wgs84MercatorToTileLocal(QPointF Point_) const;

   protected:
      //! \brief Смещение относительно Google Maps по y в тайлах
      //! \param int Y_ - номер тайла по y в Google Maps
      //! \return int - смещение по y в тайлах
      virtual int yoffset(int Y_) const;
      //! \brief Перевод географических координат в тайловые
      //! \param QPointF - географические координаты
      //! \return QPoint - тайловые координаты
      QPoint coordinateToTile(const QPointF& Point_) const;
      //! \brief Перевод географических координат в локальные пиксельные на тайле
      //! \param QPointF - географические координаты
      //! \return QPoint - локальные пиксельные координаты на тайле
      QPoint coordinateToTileLocal(const QPointF& Point_) const;
   };
}
#endif
