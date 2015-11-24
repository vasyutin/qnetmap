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

#include "qnetmap_universaltilemapadapter.h"
#include <cmath> 

namespace qnetmap
{
   TUniversalTileMapAdapter::TUniversalTileMapAdapter()
      : TTileMapAdapter("", "", 256, 256, 0, 15)
   {
   setSourceType(TMapAdapter::Consts::UniversalTileMap);
   }

   // -----------------------------------------------------------------------
   int TUniversalTileMapAdapter::yoffset(int Y_) const
   {
   switch(tilesNumbering()) {
   case Consts::TilesNumbering00BottomLeft:
      return (1 << m_CurrentZoom) - Y_ - 1;
      break;
   case Consts::TilesNumberingYahoo:
      return (1 << (m_CurrentZoom - 1)) - Y_ - 1;
      break;
   case Consts::TilesNumbering00TopLeft:
   default:
      return TParent::yoffset(Y_);
      }
   }

   //----------------------------------------------------------------------
   QPoint TUniversalTileMapAdapter::coordinateToTile(const QPointF& Point_) const
   {
   switch(basis()) {
   case Consts::ProjectionBasisEPSG3395:
      return wgs84MercatorToTile(wgs84CoordinateToMercator(Point_));
      break;
   case Consts::ProjectionBasisEPSG3857:
   default:
      return TParent::coordinateToTile(Point_);
      }
   }

   //----------------------------------------------------------------------
   QPoint TUniversalTileMapAdapter::coordinateToTileLocal(const QPointF& Point_) const
   {
   switch(basis()) {
   case Consts::ProjectionBasisEPSG3395:
      return wgs84MercatorToTileLocal(wgs84CoordinateToMercator(Point_));
      break;
   case Consts::ProjectionBasisEPSG3857:
   default:
      return TParent::coordinateToTileLocal(Point_);
      }
   }

   // ---------------------------------------------------------------------
   QPoint TUniversalTileMapAdapter::wgs84MercatorToTile(QPointF Point_) const
   {
   unsigned Scale = 1 << (23 - currentZoom());
	int X = round((20037508.342789 + Point_.x()) * 53.5865938 / Scale);
   int Y = round((20037508.342789 - Point_.y()) * 53.5865938 / Scale);
	return QPoint(X >> 8, Y >> 8);
   }

   // ---------------------------------------------------------------------
   QPoint TUniversalTileMapAdapter::wgs84MercatorToTileLocal(QPointF Point_) const
   {
   unsigned Scale = 1 << (23 - currentZoom());
	int X = round((20037508.342789 + Point_.x()) * 53.5865938 / Scale);
   int Y = round((20037508.342789 - Point_.y()) * 53.5865938 / Scale);
	return QPoint(X % 256, Y % 256);
   }

   // -----------------------------------------------------------------------
   QPointF TUniversalTileMapAdapter::wgs84CoordinateToMercator(QPointF Point_) const
   {
   qreal d = Point_.x() * PI / 180.;
   qreal m = Point_.y() * PI / 180.;
   qreal R = QNetMapConsts::WGS84EquatorialRadius;
   qreal k = 0.0818191908426;
   qreal f = k * sin(m);
	qreal h = tan(PI / 4. + m / 2.);
   qreal j = pow(tan(PI / 4. + asin(f) / 2.), k);
	return QPointF(R * d, R * log(h / j));
   }

   // -----------------------------------------------------------------------
   QPointF TUniversalTileMapAdapter::wgs84MercatorToCoordinate(QPointF Point_) const
   {
   qreal R  = QNetMapConsts::WGS84EquatorialRadius;
   qreal C1 = 0.003356551468879694;
   qreal C2 = 0.00000657187271079536;
   qreal C3 = 1.764564338702e-8;
   qreal C4 = 5.328478445e-11;
	qreal g = PI / 2. - 2. * atan(1. / exp(Point_.y() / R));
	qreal l = g + C1 * sin(2. * g) + C2 * sin(4. * g) + C3 * sin(6. * g) + C4 * sin(8. * g);
	qreal d = Point_.x() / R;
	return QPointF(d * 180. / PI, l * 180. / PI);
   }
}
