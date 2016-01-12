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

#ifndef QNETMAP_MAPPLUGIN_H
#define QNETMAP_MAPPLUGIN_H

#include <QtPlugin>
#include <QPoint>
#include <QPointF>

#include "qnetmap_global.h"
#include "qnetmap_interfaces.h"
#include "qnetmap_imagemanager.h"

#ifdef QNETMAP_QT_4
#define Q_PLUGIN_METADATA(x)
#endif

namespace qnetmap
{
   //! \class TMapPlugin
   //! \brief Шаблон класса плагина, реализующего интерфейс карты
   //! \param class TMapAdapterClass - класс, реализующий адаптер карты
   template<class TMapAdapterClass>
   class TMapPlugin : public TMapAdapterClass, public IMapAdapterInterface
   {
   public:
      //! \brief Конструктор класса
      //! \param int  StorageType_ - тип хранения (см. TImageManager::Consts)
      //! \param bool CacheInMappingStorage_ - признак сохранять-ли скачанные карты в хранилище
      TMapPlugin(const int  StorageType_ = TImageManager::Consts::InternetOnly,
                 const bool CacheInMappingStorage_ = false);
      TMapPlugin(const TMapPlugin& Source_) : TMapAdapterClass(Source_)
      {
      setStorageType(Source_.storageType());
      setMappingStoragePath(Source_.mappingStoragePath());
      m_CacheInMappingStorage = Source_.m_CacheInMappingStorage;
      m_Plugin = Source_.m_Plugin;
      }
      virtual ~TMapPlugin() {}

      //! \brief Перевод географических координат в пиксельные
      //! \param QPointF - географические координаты
      //! \return QPoint - пиксельные координаты
      QPoint coordinateToDisplay(const QPointF&) const;
      //! \brief Перевод географических координат в тайловые
      //! \param QPointF - географические координаты
      //! \return QPoint - тайловые координаты
      QPoint coordinateToTile(const QPointF&) const;
      //! \brief Перевод географических координат в локальные пиксельные на тайле
      //! \param QPointF - географические координаты
      //! \return QPoint - локальные пиксельные координаты на тайле
      QPoint coordinateToTileLocal(const QPointF&) const;
      //! \brief Перевод пиксельных координат в географические
      //! \param QPoint - пиксельные координаты
      //! \return QPointF - географические координаты
      QPointF displayToCoordinate(const QPoint&) const;
      //! \brief Получить хост провайдера карты 
      //! \return QString - хост
      QString host(const bool Random_ = false) const;
      //! \brief Получить количество хостов провайдера карты 
      //! \return int - количество
      unsigned hostsCount(void) const;
      //! \brief установка параметров адаптера карты
      void setParameters(TMapAdapterParameters* Parameters_);
      //! \brief 
      void setImageManagerInstance(TImageManager* Instance_);
      //! \brief размер тайла по оси Х
      //! \return int - размер тайла по оси Х в пикселах
      int tileSizeX(void) const; 
      //! \brief размер тайла по оси Y
      //! \return int - размер тайла по оси Y в пикселах
      int tileSizeY(void) const; 
      //! \brief Получить текущий масштаб
      //! \return int - масштаб
      int currentZoom(void) const;
      //! \brief Получить максимальный масштаб карты
      //! \return int - масштаб
      int maxZoom(void) const;
      //! \brief Получить минимальный масштаб карты
      //! \return int - масштаб
      int minZoom(void) const;
      //! \brief Проверка существования тайла с данными x, y
      //!        z подразумевается текущий
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \return bool - true, если комбинация верная
      bool isValid(int x, int y) const;
      //! \brief Проверка существования тайла с данными x, y, z
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      //! \return bool - true, если комбинация верная
      bool isValid(int x, int y, int z) const;
      //! \brief Формирование строки для запроса тайла 
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      QString query(const int x, const int y, const int z) const;
      //! \brief Увеличивает масштаб на одну ступень
      void zoom_in(void);
      //! \brief Уменьшает масштаб на одну ступень
      void zoom_out(void);
      //! \brief Устанавливает масштаб отображения карты 
      //! \param int ZoomLevel_ - масштаб
      void setZoom(int ZoomLevel_);
      //! \brief Количество тайлов по оси Х на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      int xtilesonzoomlevel(int) const;
      //! \brief Количество тайлов по оси Y на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      int ytilesonzoomlevel(int) const;
      //! \brief Смещение относительно Google Maps по x в тайлах
      //! \param int X_ - номер тайла по x в Google Maps
      //! \return int - смещение по x в тайлах
      int xoffset(int) const;
      //! \brief Смещение относительно Google Maps по y в тайлах
      //! \param int Y_ - номер тайла по y в Google Maps
      //! \return int - смещение по y в тайлах
      int yoffset(int) const;
      //! \brief Приведение масштаба относительно Google Maps
      //! \param int Z_ - масшаб в Google Maps
      //! \return int - приведенный масштаб
      int numberOfXTiles(void) const;
      int numberOfYTiles(void) const;
      int zoom(int) const;
      //! \brief Получить имя плагина
      //! \return QString - имя
      QString pluginName(void) const;
      //! \brief Установить имя плагина
      //! \param QString - имя
      void setPluginName(const QString&);
      //! \brief Получить признак прозрачности карты
      //! \return bool - true - карта прозрачная
      bool isTransparent(void) const;
      //! \brief принцип нумерации тайлов
      int tilesNumbering(void) const;
      //! \brief Получить признак пустой карты (не получает данные из внешних источников)
      //! \return bool - true - карта пустая
      bool isEmptyMap(void) const;
      uint sourceType(void) const;
      int mapType(void) const;
      uint updatability(void) const;
      QString mapVersion(void) const;
      QString description(void) const;
      //! \brief Установить тип хранения
      //! \param int Type_ - тип хранения (см. TImageManager::Consts)
      virtual void setStorageType(const int Type_);
      //! \brief Получить тип хранения
      //! \return int - тип хранения (см. TImageManager::Consts)
      virtual int storageType(void) const;
      //! \brief Установить признак записи скачанных тайлов в хранилище
      //! \param bool CacheInMappingStorage_ - true - тайлы записываются в хранилище
      virtual void setCacheInMappingStorage(const bool CacheInMappingStorage_)
      { 
      m_CacheInMappingStorage = CacheInMappingStorage_; 
      }
      //! \brief Получить признак записи скачанных тайлов в хранилище
      //! \return bool - true - тайлы записываются в хранилище
      virtual bool cacheInMappingStorage(void) const 
      { 
         return storageType() != TImageManager::Consts::InternetOnly && 
                               m_CacheInMappingStorage && 
                               updatability() == TMapAdapter::Consts::QueryUpdatabilityStatic; 
      }
      //! \brief Установить путь хранилища тайлов карты
      //! \param QString Path_ - путь к хранилищу (без последнего каталога)
      virtual void setMappingStoragePath(const QString& Path_, const bool InsertIntoATemplate_ = false);
      //! \brief Получить путь к хранилищу тайлов карты
      //! \return QString - путь к хранилищу
      virtual QString mappingStoragePath(void) const;
      //! \brief Получить полный путь файла хранилища, содержащего тайл карты
      //! \param int X_ - номер тайла по x
      //! \param int Y_ - номер тайла по y
      //! \param int Z_ - масшаб
      //! \return QString - полный путь файла
      virtual QString tileStorageFullPath(const int X_, const int Y_, const int Z_) const;
      //! \brief Запомнить в адаптере карты указатель на управляющий класс
      //! \param TMapControl* MapControl_ - Указатель на управляющий класс
      virtual void setMapControl(TMapControl* MapControl_);
      //! \brief Получить указатель на управляющий класс
      //! \return Указатель на управляющий класс
      virtual TMapControl* mapControl(void) const;
      //! \brief 
      virtual void setPlugin(QObject* Plugin_) { m_Plugin = Plugin_; }
      //! \brief Получить 
      //! \return 
      virtual QObject* plugin(void) const { return m_Plugin; }
      //! \brief 
      virtual void setRasterFileFullPath(const QString& RasterFileFullPath_);
      //! \brief 
      virtual QString rasterFileFullPath(void) const;
      //! \brief 
      bool isGeocoded(void) const;
      //! \brief 
      QPointF centerMap(void) const;
      //! \brief 
      QPointF topLeftMap(void) const;
      //! \brief 
      QPointF bottomRightMap(void) const;
      //! \brief 
      bool isInitialized(void) const;
      //! \brief 
      QString UUID(void) const;
      //! \brief Возвращает картинку тайла для растровой карты
      //! \param int X_ - номер тайла по x
      //! \param int Y_ - номер тайла по y
      //! \param int Zoom_ - масштаб
      QPixmap tile(int X_, int Y_, int Zoom_) const;
      //! \brief 
      QSize mapSize(void) const;
      //! \brief 
      QSize baseMapSize(void) const;
      //! \brief 
      qreal pixelXScale(void) const;
      //! \brief 
      qreal pixelYScale(void) const;
/*
      //! \brief Установить ограничение провайдера карты на количество запросов
      //! \param int - количество запросов до бана
      void setNumberOfRequestsToBan(const int);
      //! \brief Получить ограничение провайдера карты на количество запросов
      //! \return int - количество запросов до бана
      int numberOfRequestsToBan(void) const;
*/

   private:
      //! \var Признак записи скачаных тайлов в хранилище
      bool     m_CacheInMappingStorage;
      //! \var 
      QObject* m_Plugin;
   };

   //////////////////////////////////////////////////////////////////////////
   template<class TMapAdapterClass>
   TMapPlugin<TMapAdapterClass>::TMapPlugin(const int  StorageType_, const bool CacheInMappingStorage_) :
      m_CacheInMappingStorage(CacheInMappingStorage_), m_Plugin(NULL) 
   {
   setStorageType(StorageType_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPoint TMapPlugin<TMapAdapterClass>::coordinateToDisplay(const QPointF& PointF) const
   {
   return TMapAdapterClass::coordinateToDisplay(PointF);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPoint TMapPlugin<TMapAdapterClass>::coordinateToTile(const QPointF& PointF) const
   {
   return TMapAdapterClass::coordinateToTile(PointF);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPoint TMapPlugin<TMapAdapterClass>::coordinateToTileLocal(const QPointF& PointF) const
   {
   return TMapAdapterClass::coordinateToTileLocal(PointF);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPointF TMapPlugin<TMapAdapterClass>::displayToCoordinate(const QPoint& TPoint) const
   {
   return TMapAdapterClass::displayToCoordinate(TPoint);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::pluginName() const
   {
   return TMapAdapterClass::pluginName();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setPluginName(const QString& Name_)
   {
   TMapAdapterClass::setPluginName(Name_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::currentZoom(void) const
   {
   return TMapAdapterClass::currentZoom();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::host(const bool Random_) const
   {
   return TMapAdapterClass::host(Random_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   unsigned TMapPlugin<TMapAdapterClass>::hostsCount(void) const
   {
   return TMapAdapterClass::hostsCount();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setParameters(TMapAdapterParameters* Parameters_)
   {
   return TMapAdapterClass::setParameters(Parameters_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setImageManagerInstance(TImageManager* Instance_)
   {
      return TMapAdapterClass::setImageManagerInstance(Instance_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::maxZoom(void) const
   {
   return TMapAdapterClass::maxZoom();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::minZoom(void) const
   {
   return TMapAdapterClass::minZoom();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::tileSizeX(void) const 
   {
   return TMapAdapterClass::tileSizeX();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::tileSizeY(void) const 
   {
   return TMapAdapterClass::tileSizeY();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isValid(int X_, int Y_) const 
   {
   return TMapAdapterClass::isValid(X_, Y_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isValid(int X_, int Y_, int Z_) const 
   {
   return TMapAdapterClass::isValid(X_, Y_, Z_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::query(int X_, int Y_, int Z_) const 
   {
   return TMapAdapterClass::query(X_, Y_, Z_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::mapVersion(void) const 
   {
   return TMapAdapterClass::mapVersion();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPixmap TMapPlugin<TMapAdapterClass>::tile(int X_, int Y_, int Z_) const 
   {
   return TMapAdapterClass::tile(X_, Y_, Z_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setZoom(int ZoomLevel_)
   {
   return TMapAdapterClass::setZoom(ZoomLevel_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::zoom_in(void)
   {
   return TMapAdapterClass::zoom_in(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::zoom_out(void) 
   { 
   return TMapAdapterClass::zoom_out(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::xtilesonzoomlevel(int ZoomLevel_) const
   {
   return TMapAdapterClass::xtilesonzoomlevel(ZoomLevel_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::ytilesonzoomlevel(int ZoomLevel_) const
   {
   return TMapAdapterClass::ytilesonzoomlevel(ZoomLevel_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::xoffset(int X_) const
   {
   return TMapAdapterClass::xoffset(X_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::yoffset(int Y_) const
   {
   return TMapAdapterClass::yoffset(Y_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::numberOfXTiles(void) const
   {
      return TMapAdapterClass::numberOfXTiles(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::numberOfYTiles(void) const
   {
      return TMapAdapterClass::numberOfYTiles(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::zoom(int Z_) const
   {
   return TMapAdapterClass::zoom(Z_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isTransparent(void) const
   {
   return TMapAdapterClass::isTransparent();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::tilesNumbering(void) const
   {
   return TMapAdapterClass::tilesNumbering();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isEmptyMap(void) const
   {
   return TMapAdapterClass::isEmptyMap();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   uint TMapPlugin<TMapAdapterClass>::sourceType(void) const
   {
   return TMapAdapterClass::sourceType();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::mapType(void) const
   {
   return TMapAdapterClass::mapType();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   uint TMapPlugin<TMapAdapterClass>::updatability(void) const
   {
   return TMapAdapterClass::updatability();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::description(void) const
   {
   return TMapAdapterClass::description();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::storageType(void) const
   {
   return TMapAdapterClass::storageType();
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setStorageType(const int StorageType_)
   {
   return TMapAdapterClass::setStorageType(StorageType_);
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setRasterFileFullPath(const QString& RasterFileFullPath_) 
   {
   TMapAdapterClass::setRasterFileFullPath(RasterFileFullPath_);
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::rasterFileFullPath(void) const 
   { 
   return TMapAdapterClass::rasterFileFullPath(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isGeocoded(void) const 
   { 
   return TMapAdapterClass::isGeocoded(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPointF TMapPlugin<TMapAdapterClass>::centerMap(void) const 
   { 
   return TMapAdapterClass::centerMap(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPointF TMapPlugin<TMapAdapterClass>::topLeftMap(void) const 
   { 
   return TMapAdapterClass::topLeftMap(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QPointF TMapPlugin<TMapAdapterClass>::bottomRightMap(void) const 
   { 
   return TMapAdapterClass::bottomRightMap(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   bool TMapPlugin<TMapAdapterClass>::isInitialized(void) const 
   { 
   return TMapAdapterClass::isInitialized(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::UUID(void) const 
   { 
   return TMapAdapterClass::UUID(); 
   }

/*
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setNumberOfRequestsToBan(const int Number_)
   {
   TMapAdapterClass::setNumberOfRequestsToBan(Number_); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   int TMapPlugin<TMapAdapterClass>::numberOfRequestsToBan(void) const
   {
   return TMapAdapterClass::numberOfRequestsToBan(); 
   }
*/

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setMappingStoragePath(const QString& Path_, 
                                                          const bool InsertIntoATemplate_)
   {
   TMapAdapterClass::setMappingStoragePath(Path_, InsertIntoATemplate_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::tileStorageFullPath(const int X_, const int Y_, const int Z_) const
   {
   // Имя файла - <путь к хранилищу>/z<масштаб>/x<номер тайла по х>/y<номер тайла по y>.png
   return mappingStoragePath() + QString("/z%1/x%2/y%3.png").arg(Z_).arg(X_).arg(Y_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QString TMapPlugin<TMapAdapterClass>::mappingStoragePath(void) const
   {
   return TMapAdapterClass::mappingStoragePath(); 
   }
   
   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   void TMapPlugin<TMapAdapterClass>::setMapControl(TMapControl* MapControl_)
   {
   TMapAdapterClass::setMapControl(MapControl_);
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   TMapControl* TMapPlugin<TMapAdapterClass>::mapControl(void) const
   {
   return TMapAdapterClass::mapControl(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QSize TMapPlugin<TMapAdapterClass>::mapSize(void) const
   {
   return TMapAdapterClass::mapSize(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   QSize TMapPlugin<TMapAdapterClass>::baseMapSize(void) const
   {
   return TMapAdapterClass::baseMapSize(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   qreal TMapPlugin<TMapAdapterClass>::pixelXScale(void) const
   {
   return TMapAdapterClass::pixelXScale(); 
   }

   // -----------------------------------------------------------------------
   template<class TMapAdapterClass>
   qreal TMapPlugin<TMapAdapterClass>::pixelYScale(void) const
   {
   return TMapAdapterClass::pixelYScale(); 
   }
}

#endif
