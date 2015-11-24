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

#include "qnetmap_rastermapadapter.h"

#include <QTextStream>

namespace qnetmap
{
   //------------------------------------------------------------------
   TRasterMapAdapter::TRasterMapAdapter(const QString& RasterFileFullPath_,
                                        int TileSizeX_, int TileSizeY_, 
                                        int MinZoom_, int MaxZoom_)
      : TMapAdapter(TileSizeX_, TileSizeY_, MinZoom_, MaxZoom_),
        m_RasterFileFullPath(RasterFileFullPath_),
        m_IsGeocoded(false), m_OriginalRasterMap(NULL),
        m_ZoomedRasterMap(NULL)

   {
   setSourceType(TMapAdapter::Consts::RasterMap);

   m_NumberOfXTiles = xtilesonzoomlevel(MinZoom_);
   m_NumberOfYTiles = ytilesonzoomlevel(MinZoom_);
   }

   //------------------------------------------------------------------
   TRasterMapAdapter::~TRasterMapAdapter() 
   {
   if(m_OriginalRasterMap != NULL) delete m_OriginalRasterMap;
   if(m_ZoomedRasterMap   != NULL) delete m_ZoomedRasterMap;
   }

   //------------------------------------------------------------------
   void TRasterMapAdapter::zoom_in()
   {
   // ограничиваем размер картинки разумными пределами
   if(m_CurrentZoom >= 0 && 
      (m_ZoomedRasterMap->mapSize().width() > 10000 || m_ZoomedRasterMap->mapSize().height() > 10000)) {
         return;
      }
   //
   if(m_CurrentZoom < m_MaxZoom) m_CurrentZoom = m_CurrentZoom + 1;
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   //
   formZoomedRasterMap();
   }

   //------------------------------------------------------------------
   void TRasterMapAdapter::zoom_out()
   {
   if(m_CurrentZoom > m_MinZoom) m_CurrentZoom = m_CurrentZoom - 1;
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   //
   formZoomedRasterMap();
   }

   //------------------------------------------------------------------
   QPixmap TRasterMapAdapter::tile(int X_, int Y_, int Zoom_) const
   {
   return m_ZoomedRasterMap->tile(X_, Y_, Zoom_);
   }

   //----------------------------------------------------------------------
   void TRasterMapAdapter::setZoom(int ZoomLevel_)
   {
   // ограничиваем размер картинки разумными пределами
   if(m_CurrentZoom >= 0 && ZoomLevel_ > m_CurrentZoom &&
      (m_ZoomedRasterMap->mapSize().width() > 8000 || m_ZoomedRasterMap->mapSize().height() > 8000)) {
      return;
      }
   //
   m_CurrentZoom = ZoomLevel_ > m_MaxZoom ? m_MaxZoom : (ZoomLevel_ < m_MinZoom ? m_MinZoom : ZoomLevel_);
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   //
   formZoomedRasterMap();
   }

   //----------------------------------------------------------------------
   void TRasterMapAdapter::setRasterFileFullPath(const QString& RasterFileFullPath_)   
   {
   m_RasterFileFullPath = RasterFileFullPath_.trimmed(); 
   if(m_ZoomedRasterMap != NULL) { delete m_ZoomedRasterMap; m_ZoomedRasterMap = NULL; }
   // считываем World файл
   bool WorldFileReaded = false;
   qreal WorldFileParameters[6];
   QFile WorldFile(m_RasterFileFullPath + "w");
   if(WorldFile.exists() && WorldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      int i = -1;
      QTextStream in(&WorldFile);
      QString Line = in.readLine();
      bool LineReaded = true;
      while(LineReaded && !Line.isNull() && ++i < 6) {
         LineReaded = false;
         WorldFileParameters[i] = Line.toDouble(&LineReaded);
         Line = in.readLine();
         }
      if(i == 5 && LineReaded && WorldFileParameters[0] > 0 && WorldFileParameters[3] < 0) {
         WorldFileReaded = true;
         }
      else {
         QMessageBox::critical(NULL,
            QNetMapTranslator::tr("Error georeferencing map" /* Ru: Ошибка привязки карты */),
            QNetMapTranslator::tr("Invalid world file for the map \"%1\"."
					/* Ru: Некорректный World-файл для карты "%1". */).arg(RasterFileFullPath_));
         }
      }
   if(!WorldFileReaded) {
      // У карты нет файла привязки, выставляем некие параметры, чтобы ее увидеть
      WorldFileParameters[0] = 10.;
      WorldFileParameters[1] = 0.;
      WorldFileParameters[2] = 0.;
      WorldFileParameters[3] = -10.;
      // помещаем верхний левый угол карты в 0 координат (в world-файле указывается середина пиксела)
      WorldFileParameters[4] = 5.;
      WorldFileParameters[5] = -5.;
      }
   if(m_OriginalRasterMap == NULL) {
      // формируем объект карты
      m_OriginalRasterMap = new TRasterMap(RasterFileFullPath_,
         WorldFileParameters[0], WorldFileParameters[1], WorldFileParameters[2],
         WorldFileParameters[3], WorldFileParameters[4], WorldFileParameters[5]);
      }
   else {
      m_OriginalRasterMap->setRasterMap(RasterFileFullPath_,
         WorldFileParameters[0], WorldFileParameters[1], WorldFileParameters[2],
         WorldFileParameters[3], WorldFileParameters[4], WorldFileParameters[5]);
      }
   //
   if(m_OriginalRasterMap->rasterMapLoaded() && !m_OriginalRasterMap->rasterMapIsNull()) {
      // устанавливаем признаки правильной инициализации
      setInitialized(true);
      if(WorldFileReaded) m_IsGeocoded = true;
      formZoomedRasterMap();
      // устанавливаем границы масштабирования
      /// размер карты не менее 256 пикселей по горизонтали или вертикали
      qreal ScaleMax = max((qreal)m_OriginalRasterMap->mapSize().width()  / 256,
                           (qreal)m_OriginalRasterMap->mapSize().height() / 256);
      /// размер карты не более 8000 пикселей по горизонтали или вертикали
      /// (иначе чрезмерно жрет память и замедляется реакция)
      qreal ScaleMin = min(8000. / m_OriginalRasterMap->mapSize().width(),
                           8000. / m_OriginalRasterMap->mapSize().height());
      setZoomRestrictions(m_OriginalRasterMap->realZoom() - log(ScaleMax) / log(2.),
                          m_OriginalRasterMap->realZoom() + log(ScaleMin) / log(2.));
      }
   else {
      delete m_OriginalRasterMap;
      m_OriginalRasterMap = NULL;
      }
   }

   //----------------------------------------------------------------------
   QString TRasterMapAdapter::rasterFileFullPath(void) const
   {
   return m_RasterFileFullPath; 
   }

   //----------------------------------------------------------------------
   void TRasterMapAdapter::formZoomedRasterMap(void)
   {
   if(m_ZoomedRasterMap != NULL) delete m_ZoomedRasterMap;
   //
   m_ZoomedRasterMap = m_OriginalRasterMap->zoomedRasterMap(this);
   }

   //----------------------------------------------------------------------
   QSize TRasterMapAdapter::mapSize(void) const
   {
   return m_ZoomedRasterMap->mapSize();
   }

   //----------------------------------------------------------------------
   QSize TRasterMapAdapter::baseMapSize(void) const
   {
   return m_OriginalRasterMap->mapSize();
   }
}
