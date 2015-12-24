/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl ImageManager code by Kai Winter
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

#include "qnetmap_imagemanager.h"
#include "qnetmap_interfaces.h"
#include "qnetmap_mapplugin.h"
#include "qnetmap_pngimage.h"
#include "qnetmap_mapnetwork.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   // Инициализация статической переменной класса
   TImageManager* TImageManager::w_Instance = NULL;

   //-----------------------------------------------------------------------------------
   TImageManager::TImageManager(TParent* Parent_):
      TParent(Parent_),
      m_EmptyPixmap(QPixmap(1,1)), w_MapNetwork(NULL), m_UniqueNumber(0), m_LoadTiles(true)
   {
   if(!w_Instance || !w_Instance->mapNetwork()) w_MapNetwork = new TMapNetwork(this);
   m_EmptyPixmap.fill(Qt::transparent);

   QNetworkProxyFactory::setUseSystemConfiguration(true);
   }

   //-----------------------------------------------------------------------------------
   TImageManager::~TImageManager()
   {
   delete w_MapNetwork;
   w_MapNetwork = NULL;
   w_Instance   = NULL;
   }

   //-----------------------------------------------------------------------------------
   QPixmap TImageManager::getImage(const TMapAdapter* MapAdapter_, const int X_, const int Y_)
   {
   // в качестве масштаба берем текущий масштаб карты
   return getImage(MapAdapter_, X_, Y_, MapAdapter_->currentZoom());
   }

   //-----------------------------------------------------------------------------------
   QPixmap TImageManager::getImage(
      const TMapAdapter* MapAdapter_, const int X_, const int Y_, const int Zoom_)
   {
   if(!m_LoadTiles) return QPixmap();
   // получаем указатель на интерфейс карты
   TMapAdapterInterfaceWrapper* MapInterface =
      dynamic_cast<TMapAdapterInterfaceWrapper*>(const_cast<TMapAdapter*>(MapAdapter_));
   //
   QFileInfo TileFile;
   QString TileStorageFullPath;
   int StorageType = MapInterface->storageType();
   if(StorageType != Consts::RasterMap) {
      TileStorageFullPath = MapInterface->tileStorageFullPath(X_, Y_, Zoom_);
      TileFile.setFile(TileStorageFullPath);
      }

   //qnetmaplog(tr("StorageType: %1, TilePath: %2, FileExists: %3").arg(StorageType).arg(TileFile.absoluteFilePath()).arg(TileFile.exists()?"true":"false"), this);
   switch(StorageType) {
   case Consts::RasterMap:
      // берем соответствующую часть из растровой карты
      return MapAdapter_->tile(X_, Y_, Zoom_);
      break;
   case Consts::StorageOnly:
      {
      if(TileFile.exists()) {
         // берем из хранилища, если тайл там есть, иначе возвращается пустой OPixmap
         return QPixmap(TileFile.absoluteFilePath());
         }
      break;
      }
   case Consts::InternetAndStorage:
      {
      if(MapInterface->cacheInMappingStorage() && TileFile.exists()) {
         // берем из хранилища, если тайл там есть, иначе идем дальше и запрашиваем из интернет
         TPNGImage Image(TileFile.absoluteFilePath());
         if(Image.isPNGImage()) {
            if(Image.mapVersion() != MapInterface->mapVersion()) {
               // сохраненная и текущая версии карт не совпадают, делаем новый запрос
               QUrl Url = "http://" + MapAdapter_->host() + MapAdapter_->query(X_, Y_, Zoom_);
               if(!w_MapNetwork->imageIsLoading(Url.toString())) {   // проверка, может запрос уже сделан
                  w_MapNetwork->loadURL(MapAdapter_, Url, TileFile.absoluteFilePath(), true, MapInterface->mapVersion());
                  }
               }
            // возвращаем тайл из хранилища, после выполнения запроса тайл заменится в хранилище
            // новая версия тайла отобразится при следующем его показе
            return Image.pixmap();
            }
         }
      }
   case Consts::InternetOnly:
   default:
      QNetworkDiskCache* Cache = w_MapNetwork->networkDiskCache();
      QIODevice* Data = NULL;
      QUrl Url;
      QString Query = MapAdapter_->query(X_, Y_, Zoom_);
      // один и тот-же тайл может прийти с разных url
      unsigned HostsCount = MapAdapter_->hostsCount();
      for(unsigned i = 0; i < HostsCount && Data == NULL; ++i) {
         Url = "http://" + MapAdapter_->host() + Query;
         Data = Cache->data(Url);
         }
      if(Data == NULL) {
         // формируем урл со случайным именем хоста
         Url = "http://" + MapAdapter_->host(true) + Query;
         //
         if(!w_MapNetwork->contentNotFound(Url.toString())) {
            // отправляем запрос в интернет, в этом случае возвращается пустой QPixmap
            if(!w_MapNetwork->imageIsLoading(Url.toString())) {   // проверка, может запрос уже сделан
               // если тайл нужно сохранить в хранилище, передаем путь для хранения
               if(MapInterface->cacheInMappingStorage())   
                  w_MapNetwork->loadURL(
                     MapAdapter_, Url, TileFile.absoluteFilePath(), true, MapInterface->mapVersion());
               else {
                  w_MapNetwork->loadURL(MapAdapter_, Url);
                  }
               }
            }
         }
      else {
         std::string s = Url.toString().toStdString();
         // вытаскиваем данные из кеша
         QPixmap PixMap;
         PixMap.loadFromData(Data->readAll());
         delete Data;
         if(MapInterface->cacheInMappingStorage() && !TileFile.exists()) {
            // отправляем запрос в интернет
            if(!w_MapNetwork->imageIsLoading(Url.toString())) {   // проверка, может запрос уже сделан
                  w_MapNetwork->loadURL(
                     MapAdapter_, Url, TileFile.absoluteFilePath(), true, MapInterface->mapVersion());
               }
            }
         return PixMap;
         }
      break;
      }
   // рекурсивный вызов для получения куска тайла из слоя с более крупным зумом
   if(!MapAdapter_->isTransparent() && Zoom_ > MapAdapter_->minZoom()) {
      int X, Y, Zoom = Zoom_ - 1;
      QPixmap Pixmap;
      //
      switch(MapAdapter_->tilesNumbering()) {
      case TMapAdapter::Consts::TilesNumbering00TopLeft:
         // определяем какой тайл нужен
         X = X_ >> 1;
         Y = Y_ >> 1;
         // пробуем получить
         Pixmap = getImage(MapAdapter_, X, Y, Zoom);
         if(Pixmap.size().width() > 1) {
            int TileWidth  = Pixmap.size().width()  >> 1;
            int TileHeight = Pixmap.size().height() >> 1;
            // вырезаем и масштабируем нужный кусок
            QRect TileRect(QPoint(TileWidth * (X_ % 2), TileHeight * (Y_ % 2)), QSize(TileWidth, TileHeight));
            QPixmap Tile(QSize(TileWidth, TileHeight));
            QPainter Painter;
            Painter.begin(&Tile);
            Painter.drawPixmap(0, 0, Pixmap.copy(TileRect));
            Painter.end();
            return Tile.transformed(QMatrix(2, 0, 0, 2, 0, 0), Qt::SmoothTransformation);
            }
         break;
      case TMapAdapter::Consts::TilesNumbering00BottomLeft:
         break;
      case TMapAdapter::Consts::TilesNumberingYahoo:
         break;
         }
      }
   // нигде ничего не найдено - возвращаем пустой рисунок
   return m_EmptyPixmap;
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::abortLoading(TMapAdapter* MapAdapter_)
   {
   w_MapNetwork->abortLoading(MapAdapter_);
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::setProxy( const QString& Host_, int Port_ )
   {
   w_MapNetwork->setProxy(Host_, Port_);
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::unSetProxy( void )
   {
   w_MapNetwork->unSetProxy();
   }


   //-----------------------------------------------------------------------------------
   void TImageManager::setCacheDir(const QDir& Path_)
   {
   w_MapNetwork->setCacheDir(Path_);
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::receivedImage()
   {
   emit(imageReceived());
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::receivedHtml()
   {
   emit(htmlReceived());
   }

   //-----------------------------------------------------------------------------------
   void TImageManager::loadingQueueEmpty()
   {
   emit(loadingFinished());
   }  

   //-----------------------------------------------------------------------------------
   void TImageManager::setInstance( TImageManager* Instance_ )
   {
      w_Instance = Instance_;
   }

}
