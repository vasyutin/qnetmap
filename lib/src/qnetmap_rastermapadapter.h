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

#ifndef QNETMAP_RASTERMAPADAPTER_H
#define QNETMAP_RASTERMAPADAPTER_H

#include "qnetmap_global.h"


#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QMessageBox>
#endif
#include <QPixmap>
#include <QFile>
#include <cmath>

#include "qnetmap_rastermap.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_imagemanager.h"

namespace qnetmap
{
   class QNETMAP_EXPORT TRasterMapAdapter : public TMapAdapter
   {
      Q_OBJECT

   public:
      template<typename T> T min(T a, T b) { return a < b ? a : b;}
      template<typename T> T max(T a, T b) { return a < b ? b : a;}

      TRasterMapAdapter(const QString& RasterFileFullPath_ = "",
                        int tileSizeX_ = 256, int tileSizeY_ = 256, 
                        int minZoom = 0, int maxZoom = 100);
      virtual ~TRasterMapAdapter();

   private:
      //! \brief Формирует масштабированную растровую карту
      void formZoomedRasterMap(void);

      //! \var Полный путь к файлу с растровой картой
      QString m_RasterFileFullPath;
      //! \var Признак того, что карта привязана к местности
      bool m_IsGeocoded;
      //! \var Указатель на оригинальную растровую карту
      TRasterMap* m_OriginalRasterMap;
      //! \var Указатель на масштабированную растровую карту
      TRasterMap* m_ZoomedRasterMap;

   protected:
      virtual void zoom_in();
      virtual void zoom_out();
      virtual QPixmap tile(int X_, int Y_, int Z_) const;
      virtual void setZoom(int ZoomLevel_);
      virtual QString host(const bool Random_ = false) const { Q_UNUSED(Random_); return QString(); }
      virtual unsigned hostsCount(void) const { return 0; }
      virtual void setParameters(TMapAdapterParameters*) {}
      //! \brief 
      virtual void setImageManagerInstance(TImageManager* Instance_) { TImageManager::setInstance(Instance_); }
      virtual void setURL(const QString&, const QString&) {}

      //! \brief Устанавливает путь к файлу растровой карты
      virtual void setRasterFileFullPath(const QString& RasterFileFullPath_);
      //! \brief Возвращает путь к файлу растровой карты
      virtual QString rasterFileFullPath(void) const;
      //! \brief Возвращает размер растровой карты в пикселах
      virtual QSize mapSize(void) const;
      //! \brief Возвращает размер базовой растровой карты в пикселах
      virtual QSize baseMapSize(void) const;
      //! \brief возвращает признак привязки карты с помощью world-файла
      virtual bool isGeocoded(void) const { return m_IsGeocoded; }
      //! \brief Возвращает географические координаты центра растровой карты
      virtual QPointF centerMap(void) const
      {
      if(m_ZoomedRasterMap) return m_ZoomedRasterMap->centerMap(); 
      else                  return QPointF(0, 0);
      }
      //! \brief Возвращает географические координаты верхнего левого угла карты 
      virtual QPointF topLeftMap(void) const
      {
      if(m_ZoomedRasterMap) return m_ZoomedRasterMap->topLeftMap(); 
      else                  return QPointF(0, 0);
      }
      //! \brief Возвращает географические координаты нижнего правого угла карты
      virtual QPointF bottomRightMap(void) const
      {
      if(m_ZoomedRasterMap) return m_ZoomedRasterMap->bottomRightMap(); 
      else                  return QPointF(0, 0);
      }
  };
}
#endif
