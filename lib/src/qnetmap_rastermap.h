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

#ifndef QNETMAP_RASTERMAP_H
#define QNETMAP_RASTERMAP_H

#include "qnetmap_global.h"

#include <QPixmap>
#include <QPainter>
#include <cmath> 

namespace qnetmap
{
   class TRasterMapAdapter;

   class QNETMAP_EXPORT TRasterMap
   {
   public:
      //! \brief конструктор
      //!        Сохраняет внутри класса оригинальную карту
      //! \param QString RasterFileFullPath_ - путь к файлу с растровой картой
      //! \param qreal PixelXScale_ - размер пиксела по оси X в метрах
      //! \param qreal PixelXSkew_  - x-skew
      //! \param qreal PixelYSkew_  - y-skew
      //! \param qreal PixelYScale_ - размер пиксела по оси Y в метрах
      //! \param qreal TopLeftX_ - x-координата центра верхнего левого пиксела в метрах
      //! \param qreal TopLeftY_ - y-координата центра верхнего левого пиксела в метрах
      TRasterMap(const QString& RasterFileFullPath_,
                 qreal PixelXScale_, qreal PixelXSkew_, qreal PixelYSkew_, qreal PixelYScale_,
                 qreal TopLeftX_, qreal TopLeftY_); 
      virtual ~TRasterMap() {}

      //! \brief Изменяет внутри класса оригинальную карту
      //! \param QString RasterFileFullPath_ - путь к файлу с растровой картой
      //! \param qreal PixelXScale_ - размер пиксела по оси X в метрах
      //! \param qreal PixelXSkew_  - x-skew
      //! \param qreal PixelYSkew_  - y-skew
      //! \param qreal PixelYScale_ - размер пиксела по оси Y в метрах
      //! \param qreal TopLeftX_ - x-координата центра верхнего левого пиксела в метрах
      //! \param qreal TopLeftY_ - y-координата центра верхнего левого пиксела в метрах
      void setRasterMap(
         const QString& RasterFileFullPath_,
         qreal PixelXScale_, qreal PixelXSkew_, qreal PixelYSkew_, qreal PixelYScale_,
         qreal TopLeftX_, qreal TopLeftY_); 
      //! \brief размер пиксела по оси X в метрах
      qreal pixelXScale(void) const { return m_PixelXScale; }
      //! \brief размер пиксела по оси Y в метрах 
      qreal pixelYScale(void) const { return m_PixelYScale; }
      //! \brief x-skew
      qreal pixelXSkew(void)  const { return m_PixelXSkew; }
      //! \brief y-skew
      qreal pixelYSkew(void)  const { return m_PixelYSkew; }
      //! \brief x-координата центра верхнего левого пиксела в метрах
      qreal topLeftX(void) const { return m_TopLeftX; }
      //! \brief y-координата центра верхнего левого пиксела в метрах
      qreal topLeftY(void) const { return m_TopLeftY; }
      //! \brief масштаб
      int zoom(void) const { return m_Zoom; }
      qreal realZoom(void) const 
      {
      if(m_Zoom < 0) {
         qreal Scale = PI * 2. * qnetmap::QNetMapConsts::PopularVisualisationSphereRadius / (m_PixelXScale * 256);
         return log(Scale) / log(2.);
         }
      else return m_Zoom;
      }
      //! \brief признак того, что карта загружена
      bool rasterMapLoaded(void) const { return m_RasterMapLoaded; }
      //! \brief признак пустой карты
      bool rasterMapIsNull(void) const { return m_PixmapMap.isNull(); }
      //! \brief создает новый объект с масштабированной картой
      //!        (масштаб берется из адаптера карты)
      //! \param TRasterMapAdapter* RasterMapAdapter_ - указатель на адаптер растровой карты
      TRasterMap* zoomedRasterMap(TRasterMapAdapter* RasterMapAdapter_);
      //! \brief возвращает кусок растровой карты в виде тайла по его номеру в тайловом массиве
      //! \param int X_ - номер тайла по X
      //! \param int Y_ - номер тайла по Y
      //! \param int Zoom_ - масштаб
      QPixmap tile(int X_, int Y_, int Zoom_);
      //! \brief Возвращает географические координаты центра растровой карты
      QPointF centerMap(void) const;
      //! \brief Возвращает географические координаты верхнего левого угла карты 
      QPointF topLeftMap(void) const;
      //! \brief Возвращает географические координаты нижнего правого угла карты
      QPointF bottomRightMap(void) const;
      //! \brief Возвращает размер растровой карты в пикселах
      QSize mapSize(void) const { return m_PixmapMap.size(); }

   private:
      Q_DISABLE_COPY(TRasterMap)

      //! \brief конструктор
      //!        Сохраняет внутри класса карту, измененную в соответствии с 
      //!        текущими параметрами адаптера карты
      //! \param TRasterMapAdapter* RasterMapAdapter_ - указатель на адаптер растровой карты
      //! \param TRasterMap* RasterMap_ - указатель на класс с оригинальной растровой картой
      TRasterMap(TRasterMapAdapter* RasterMapAdapter_, TRasterMap* RasterMap_);
      //! \brief возвращает трансформированную растровую карту
      QPixmap transformed(const QMatrix& Matrix_, Qt::TransformationMode Mode_ = Qt::FastTransformation);

      //! \var указатель на адаптер растровой карты
      TRasterMapAdapter* m_RasterMapAdapter;
      //! \var признак загрузки растровой карты
      bool m_RasterMapLoaded;
      //! \var загруженная карта
      QPixmap m_PixmapMap;
      //! \var размер пиксела по оси X в метрах
      qreal m_PixelXScale;
      //! \var x-skew
      qreal m_PixelXSkew;
      //! \var y-skew
      qreal m_PixelYSkew;
      //! \var размер пиксела по оси Y в метрах
      qreal m_PixelYScale;
      //! \var x-координата верхнего левого угла верхнего левого пиксела в метрах
      qreal m_TopLeftX;
      //! \var y-координата верхнего левого угла верхнего левого пиксела в метрах
      qreal m_TopLeftY;
      //! \var x-координата верхнего левого пиксела в пикселах
      int m_TopLeftPixelX;
      //! \var y-координата верхнего левого пиксела в пикселах
      int m_TopLeftPixelY;
      //! \var масштаб преобразованной растровой карты (-1 - оригинальная)
      int m_Zoom;
      //! \var число пи
      static qreal PI;
   };
}
#endif
