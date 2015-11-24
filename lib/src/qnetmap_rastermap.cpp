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

#include "qnetmap_rastermap.h"
#include "qnetmap_rastermapadapter.h"
#include "qnetmap_utmpoint.h"

namespace qnetmap
{
   qreal TRasterMap::PI = acos(-1.0);

   //------------------------------------------------------------------
   TRasterMap::TRasterMap(
      const QString& RasterFileFullPath_,
      qreal PixelXScale_, qreal PixelXSkew_, qreal PixelYSkew_, qreal PixelYScale_,
      qreal TopLeftX_, qreal TopLeftY_) 
      : m_RasterMapAdapter(NULL), m_Zoom(-1)
   {
   setRasterMap(RasterFileFullPath_, PixelXScale_, PixelXSkew_, PixelYSkew_, PixelYScale_, TopLeftX_, TopLeftY_);
   }

   //------------------------------------------------------------------
   TRasterMap::TRasterMap(TRasterMapAdapter* RasterMapAdapter_, TRasterMap* RasterMap_)
   {
   m_RasterMapAdapter = RasterMapAdapter_;
   if(!RasterMap_->rasterMapIsNull() && m_RasterMapAdapter && m_RasterMapAdapter->currentZoom() >= 0) {
      m_Zoom = m_RasterMapAdapter->currentZoom();
      // необходимая размерность пикселя
      m_PixelXScale =   m_RasterMapAdapter->pixelXScale();
      m_PixelYScale = - m_RasterMapAdapter->pixelYScale();
      // масштабируем
      m_PixmapMap = RasterMap_->transformed(
         QMatrix(RasterMap_->pixelXScale() / m_PixelXScale, 0, 0,
                 RasterMap_->pixelYScale() / m_PixelYScale, 0, 0));
      // устанавливаем остальные свойства
      m_PixelXSkew = RasterMap_->pixelXSkew();
      m_PixelYSkew = RasterMap_->pixelYSkew();
      m_TopLeftX = RasterMap_->topLeftX();
      m_TopLeftY = RasterMap_->topLeftY();
      QPoint CenterMap = m_RasterMapAdapter->coordinateToDisplay(centerMap());
      m_TopLeftPixelX = CenterMap.x() - m_PixmapMap.width()  / 2;
      m_TopLeftPixelY = CenterMap.y() - m_PixmapMap.height() / 2;
      }
   else assert(!"Некорректные параметры TRasterMap");
   }

   //------------------------------------------------------------------
   void TRasterMap::setRasterMap(
      const QString& RasterFileFullPath_,
      qreal PixelXScale_, qreal PixelXSkew_, qreal PixelYSkew_, qreal PixelYScale_,
      qreal TopLeftX_, qreal TopLeftY_) 
   {
   m_PixelXScale = PixelXScale_;
   m_PixelYScale = PixelYScale_;
   m_PixelXSkew  = PixelXSkew_;
   m_PixelYSkew  = PixelYSkew_;
   m_TopLeftX    = TopLeftX_ - PixelXScale_ / 2.; // в world-файле указаны координаты
   m_TopLeftY    = TopLeftY_ - PixelYScale_ / 2.; // центра верхнего левого пиксела
   // а не надо-ли здесь сразу повернуть карту?
   if(PixelXSkew_ != 0 || PixelYSkew_ != 0) {
      // загружаем и поворачиваем
      m_PixmapMap = QPixmap(RasterFileFullPath_).
         transformed(QMatrix(1, PixelXSkew_, PixelYSkew_, 1, 0, 0), Qt::FastTransformation);
      m_RasterMapLoaded = !m_PixmapMap.isNull();
      }
   else {
      // просто загружаем
      m_RasterMapLoaded = m_PixmapMap.load(RasterFileFullPath_);
      }
   }

   //------------------------------------------------------------------
   QPixmap TRasterMap::tile(int X_, int Y_, int Zoom_)
   {
   QPixmap PixmapTile(m_RasterMapAdapter->tileSizeX(), m_RasterMapAdapter->tileSizeY());
   if((X_ + Y_) % 2) PixmapTile.fill(QColor(230, 230, 230));
   else              PixmapTile.fill(QColor(240, 240, 240));
   //
   if(!m_PixmapMap.isNull() && m_Zoom == Zoom_ &&
      m_RasterMapAdapter && m_RasterMapAdapter->isValid(X_, Y_, Zoom_)) { 
      // глобальные пиксельные координаты тайла на текущем уровне
      QRect TileRect = m_RasterMapAdapter->tileRect(X_, Y_);
      // пиксельные координаты относительно верхнего левого угла карты
      TileRect.translate(-m_TopLeftPixelX, -m_TopLeftPixelY);
      // пересекается с картой?
      if(TileRect.intersects(m_PixmapMap.rect())) {
         // вытаскиваем пересечение и рисуем его на тайле
         QPainter Painter;
         Painter.begin(&PixmapTile);
         int x = TileRect.x() > 0 ? 0 : -TileRect.x();
         int y = TileRect.y() > 0 ? 0 : -TileRect.y();
         Painter.drawPixmap(x, y, m_PixmapMap.copy(TileRect));
         Painter.end();
         }
      }
   return PixmapTile;
   }

   //------------------------------------------------------------------
   QPointF TRasterMap::centerMap(void) const
   {
   if(!m_PixmapMap.isNull()) {
      qreal Longitude = 
         ((m_TopLeftX + qreal(m_PixmapMap.width()) * m_PixelXScale * 0.5) / 
          QNetMapConsts::PopularVisualisationSphereRadius) * (180. / PI);
      qreal Latitude =
         atan(sinh((m_TopLeftY + qreal(m_PixmapMap.height()) * m_PixelYScale * 0.5) / 
         QNetMapConsts::PopularVisualisationSphereRadius)) * (180. / PI);

      return QPointF(Longitude, Latitude);
      }
   else
      return QPointF(0, 0);
   }

   //------------------------------------------------------------------
   QPointF TRasterMap::topLeftMap(void) const
   {
   if(!m_PixmapMap.isNull()) {
      qreal Longitude = (m_TopLeftX / QNetMapConsts::PopularVisualisationSphereRadius) * (180. / PI);
      qreal Latitude =
         atan(sinh(m_TopLeftY / QNetMapConsts::PopularVisualisationSphereRadius)) * (180. / PI);

      return QPointF(Longitude, Latitude);
      }
   else
      return QPointF(0, 0);
   }

   //------------------------------------------------------------------
   QPointF TRasterMap::bottomRightMap(void) const
   {
   if(!m_PixmapMap.isNull()) {
      qreal Longitude = 
         ((m_TopLeftX + qreal(m_PixmapMap.width()) * m_PixelXScale) / 
          QNetMapConsts::PopularVisualisationSphereRadius) * (180. / PI);
      qreal Latitude =
         atan(sinh((m_TopLeftY + qreal(m_PixmapMap.height()) * m_PixelYScale) / 
                   QNetMapConsts::PopularVisualisationSphereRadius)) * (180. / PI);

      return QPointF(Longitude, Latitude);
      }
   else
      return QPointF(0, 0);
   }

   //------------------------------------------------------------------
   QPixmap TRasterMap::transformed(const QMatrix & Matrix_, Qt::TransformationMode Mode_)
   {
   if(!m_PixmapMap.isNull()) return m_PixmapMap.transformed(Matrix_, Mode_);
   else                      return QPixmap();
   }

   //------------------------------------------------------------------
   TRasterMap* TRasterMap::zoomedRasterMap(TRasterMapAdapter* RasterMapAdapter_)
   {
   if(!m_PixmapMap.isNull() && RasterMapAdapter_ && RasterMapAdapter_->currentZoom() >= 0)
      return new TRasterMap(RasterMapAdapter_, this);
   else return NULL;
   }
}
