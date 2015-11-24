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

#ifndef QNETMAP_INTERFACES_H
#define QNETMAP_INTERFACES_H

#include <QtPlugin>

#include "qnetmap_global.h"
#include "qnetmap_tilemapadapter.h"

namespace qnetmap
{
   class TMapControl;

   //! \class IMapAdapterInterface
   //! \brief Абстрактный класс, реализующий интерфейс к плагинам карт
   class QNETMAP_EXPORT IMapAdapterInterface
   {
   public:
      virtual ~IMapAdapterInterface() {}

      virtual QObject* createInstance() = 0;

      virtual QPoint coordinateToDisplay(const QPointF&) const = 0;
      virtual QPoint coordinateToTile(const QPointF&) const = 0;
      virtual QPoint coordinateToTileLocal(const QPointF&) const = 0;
      virtual QPointF displayToCoordinate(const QPoint&) const = 0;
      virtual QString host(const bool Random_ = false) const = 0;
      virtual unsigned hostsCount(void) const = 0;
      virtual void setParameters(TMapAdapterParameters*) = 0;
      virtual void setImageManagerInstance(TImageManager*) = 0;

      virtual int tileSizeX(void) const = 0; 
      virtual int tileSizeY(void) const = 0; 
      virtual int currentZoom(void) const = 0;
      virtual int maxZoom(void) const = 0;
      virtual int minZoom(void) const = 0;
      virtual bool isValid(int, int) const = 0;
      virtual bool isValid(int, int, int) const = 0;
      virtual QString query(const int, const int, const int) const = 0;
      virtual QString mapVersion(void) const = 0;
      virtual QPixmap tile(int, int, int) const = 0;
      virtual void zoom_in(void) = 0;
      virtual void zoom_out(void) = 0;
      virtual void setZoom(int ZoomLevel_) = 0;
      virtual int xtilesonzoomlevel(int) const = 0;
      virtual int ytilesonzoomlevel(int) const = 0;
      virtual int xoffset(int) const = 0;
      virtual int yoffset(int) const = 0;
      virtual int numberOfXTiles(void) const = 0;
      virtual int numberOfYTiles(void) const = 0;
      virtual int zoom(int) const = 0;
      virtual QString pluginName(void) const = 0;
      virtual void setPluginName(const QString&) = 0;
      virtual bool isTransparent(void) const = 0;
      virtual int tilesNumbering(void) const  = 0;
      virtual bool isEmptyMap(void) const = 0;
      virtual void setMappingStoragePath(const QString&, const bool InsertIntoATemplate_ = false) = 0;
      virtual QString mappingStoragePath(void) const = 0;
      virtual QString tileStorageFullPath(const int, const int, const int) const = 0;
      virtual uint sourceType(void) const = 0;
      virtual int mapType(void) const = 0;
      virtual uint updatability(void) const = 0;
      virtual QString description(void) const = 0;
      virtual void setStorageType(const int) = 0;
      virtual int storageType(void) const = 0;
      virtual bool cacheInMappingStorage(void) const = 0;
      virtual void setMapControl(TMapControl*) = 0;
      virtual TMapControl* mapControl(void) const = 0;
      virtual void setPlugin(QObject*) = 0;
      virtual QObject* plugin(void) const = 0;
      virtual void setRasterFileFullPath(const QString&) = 0;
      virtual QString rasterFileFullPath(void) const = 0;
      virtual bool isGeocoded(void) const = 0;
      virtual QPointF centerMap(void) const = 0;
      virtual QPointF topLeftMap(void) const = 0;
      virtual QPointF bottomRightMap(void) const = 0;
      virtual bool isInitialized(void) const = 0;
      virtual QString UUID(void) const = 0;
      virtual QSize mapSize(void) const = 0;
      virtual QSize baseMapSize(void) const = 0;
      virtual qreal pixelXScale(void) const = 0;
      virtual qreal pixelYScale(void) const = 0;
      //virtual void setNumberOfRequestsToBan(const int) = 0;
      //virtual int numberOfRequestsToBan(void) const = 0;
   };

   typedef QList<IMapAdapterInterface*> IMapAdapterInterfaces;

   //! \class TMapAdapterInterfaceWrapper
   //! \brief Класс-обертка.
   //!        Служит для единообразного вызова виртуальных методов плагинов разных карт.
   class QNETMAP_EXPORT TMapAdapterInterfaceWrapper : public TTileMapAdapter
   {
      Q_OBJECT

   public:
      //! \brief Конструктор класса
      //! \param IMapAdapterInterface* MapInterface_ - указатель на интерфейс карты
      //! \param TMapControl* MapControl_ - указатель на управляющий класс
      TMapAdapterInterfaceWrapper(IMapAdapterInterface* MapInterface_, TMapControl* MapControl_) 
         : TTileMapAdapter("", "", 0, 0) { 
         m_MapInterface = MapInterface_;
         m_MapInterface->setMapControl(MapControl_);
      }
//       TMapAdapterInterfaceWrapper(const TMapAdapterInterfaceWrapper& Source_) : TTileMapAdapter(Source_)
//       {
//       m_MapInterface = Source_.m_MapInterface;
//       }
      virtual ~TMapAdapterInterfaceWrapper() { delete m_MapInterface; }

      //! \brief Создает новый объект класса плагина
      //!        Каждый плагин обязан реализовать этот метод
      //! \return QObject* - указатель на интерфейс плагина,
      //!                    для преобразования надо использовать qobject_cast<IMapAdapterInterface *>
      QObject* createInstance() { return m_MapInterface->createInstance(); }

      void setRasterFileFullPath(const QString& RasterFileFullPath_)
         { m_MapInterface->setRasterFileFullPath(RasterFileFullPath_); }
      QString rasterFileFullPath(void) const { return m_MapInterface->rasterFileFullPath(); }
      bool isGeocoded(void) const { return m_MapInterface->isGeocoded(); }
      bool isEmptyMap(void) const { return m_MapInterface->isEmptyMap(); }
      bool isTransparent(void) const { return m_MapInterface->isTransparent(); }
      uint sourceType(void) const { return m_MapInterface->sourceType(); }
      int mapType(void) const { return m_MapInterface->mapType(); }
      uint updatability(void) const { return m_MapInterface->updatability(); }
      QString description(void) const { return m_MapInterface->description(); }
      QPointF centerMap(void) const { return m_MapInterface->centerMap(); }
      QPointF topLeftMap(void) const { return m_MapInterface->topLeftMap(); }
      QPointF bottomRightMap(void) const { return m_MapInterface->bottomRightMap(); }
      bool isInitialized(void) const { return m_MapInterface->isInitialized(); }
      QString UUID(void) const { return m_MapInterface->UUID(); }
      //! \brief принцип нумерации тайлов
      int tilesNumbering(void) const { return m_MapInterface->tilesNumbering(); }
      //! \brief Получить хост провайдера карты 
      //! \return QString - хост
      QString host(const bool Random_ = false) const { return m_MapInterface->host(Random_); }
      unsigned hostsCount(void) const { return m_MapInterface->hostsCount(); }
      //! \brief установка параметров адаптера карты
      void setParameters(TMapAdapterParameters* Parameters_) 
      {
      return m_MapInterface->setParameters(Parameters_); 
      }
      //! \brief установка параметров адаптера карты
      void setImageManagerInstance(TImageManager* Instance_) 
      {
         return m_MapInterface->setImageManagerInstance(Instance_); 
      }
      //! \brief размер тайла по оси X
      //! \return int - размер тайла по оси X в пикселах
      int tileSizeX() const { return m_MapInterface->tileSizeX(); }
      //! \brief размер тайла по оси Y
      //! \return int - размер тайла по оси Y в пикселах
      int tileSizeY() const { return m_MapInterface->tileSizeY(); }
      //! \brief Получить минимальный масштаб карты
      //! \return int - масштаб
      int minZoom()  const { return m_MapInterface->minZoom(); }
      //! \brief Получить максимальный масштаб карты
      //! \return int - масштаб
      int maxZoom()  const { return m_MapInterface->maxZoom(); }
      //! \brief Получить текущий масштаб
      //! \return int - масштаб
      int currentZoom() const { return m_MapInterface->currentZoom(); }
      //! \brief Перевод географических координат в тайловые
      //! \param QPointF - географические координаты
      //! \return QPoint - тайловые координаты
      QPoint coordinateToTile(const QPointF& Coordinate_) const
         { return m_MapInterface->coordinateToTile(Coordinate_); }
      //! \brief Перевод географических координат в локальные пиксельные на тайле
      //! \param QPointF - географические координаты
      //! \return QPoint - локальные пиксельные координаты на тайле
      QPoint coordinateToTileLocal(const QPointF& Coordinate_) const
         { return m_MapInterface->coordinateToTileLocal(Coordinate_); }
      //! \brief Перевод географических координат в пиксельные
      //! \param QPointF - географические координаты
      //! \return QPoint - пиксельные координаты
      QPoint coordinateToDisplay(const QPointF& Coordinate_) const
         { return m_MapInterface->coordinateToDisplay(Coordinate_); }
      //! \brief Перевод пиксельных координат в географические
      //! \param QPoint - пиксельные координаты
      //! \return QPointF - географические координаты
      QPointF displayToCoordinate(const QPoint& point) const
         { return m_MapInterface->displayToCoordinate(point); }

      //! \brief Проверка существования тайла с данными x, y
      //!        z подразумевается текущий
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \return bool - true, если комбинация верная
      bool isValid( int X_, int Y_) const { return m_MapInterface->isValid(X_, Y_); }
      //! \brief Проверка существования тайла с данными x, y, z
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      //! \return bool - true, если комбинация верная
      bool isValid( int X_, int Y_, int Z_) const { return m_MapInterface->isValid(X_, Y_, Z_); }
      //! \brief Формирование строки для запроса тайла 
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      QString query(const int X_, const int Y_, const int Z_) const { return m_MapInterface->query(X_, Y_, Z_); }
      //! \brief Возвращает версию карт 
      QString mapVersion(void) const { return m_MapInterface->mapVersion(); }
      //! \brief Возвращает тайл растровой карты 
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      QPixmap tile(int X_, int Y_, int Z_) const { return m_MapInterface->tile(X_, Y_, Z_); }
      //! \brief Увеличивает масштаб на одну ступень
      void zoom_in()  { return m_MapInterface->zoom_in(); }
      //! \brief Уменьшает масштаб на одну ступень
      void zoom_out() { return m_MapInterface->zoom_out(); }
      //! \brief Устанавливает масштаб отображения карты 
      //! \param int ZoomLevel_ - масштаб
      void setZoom(int ZoomLevel_) { m_MapInterface->setZoom(ZoomLevel_); }
      //! \brief Количество тайлов по оси X на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      int xtilesonzoomlevel(int ZoomLevel_) const { return m_MapInterface->xtilesonzoomlevel(ZoomLevel_); }
      //! \brief Количество тайлов по оси Y на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      int ytilesonzoomlevel(int ZoomLevel_) const { return m_MapInterface->ytilesonzoomlevel(ZoomLevel_); }
      //! \brief Смещение по x в тайлах
      //! \param int X_ - номер тайла по x
      //! \return int - смещение по x в тайлах
      int xoffset(int X_) const { return m_MapInterface->xoffset(X_); }
      //! \brief Смещение по y в тайлах
      //! \param int Y_ - номер тайла по y
      //! \return int - смещение по y в тайлах
      int yoffset(int Y_) const { return m_MapInterface->yoffset(Y_); }
      //! \brief Приведение масштаба
      //! \param int Z_ - масшаб
      //! \return int - приведенный масштаб
      int numberOfXTiles(void) const { return m_MapInterface->numberOfXTiles(); }
      int numberOfYTiles(void) const { return m_MapInterface->numberOfXTiles(); }
      int zoom(int Z_) const { return m_MapInterface->zoom(Z_); }
/*
      //! \brief Установить ограничение провайдера карты на количество запросов
      //! \param int - количество запросов до бана
      void setNumberOfRequestsToBan(const int Number_) { m_MapInterface->setNumberOfRequestsToBan(Number_); }
      //! \brief Получить ограничение провайдера карты на количество запросов
      //! \return int - количество запросов до бана
      int numberOfRequestsToBan(void) const { return m_MapInterface->numberOfRequestsToBan(); }
*/
      //! \brief Установить путь к хранилищу тайлов карты
      //! \return QString - путь к хранилищу
      virtual void setMappingStoragePath(const QString& Path_, const bool InsertIntoATemplate_ = false) 
         { m_MapInterface->setMappingStoragePath(Path_, InsertIntoATemplate_); }
      //! \brief Получить путь к хранилищу тайлов карты
      //! \return QString - путь к хранилищу
      virtual QString mappingStoragePath(void) const { return m_MapInterface->mappingStoragePath(); }
      //! \brief Получить полный путь файла хранилища, содержащего тайл карты
      //! \param int X_ - номер тайла по x
      //! \param int Y_ - номер тайла по y
      //! \param int Z_ - масшаб
      //! \return QString - полный путь файла
      virtual QString tileStorageFullPath(const int X_, const int Y_, const int Z_) const
         { return m_MapInterface->tileStorageFullPath(X_, Y_, Z_); }
      //! \brief Получить тип хранения
      //! \return int - тип хранения (см. TImageManager::Consts)
      virtual int storageType(void) const { return m_MapInterface->storageType(); }
      //! \brief Установить тип хранения
      //! \param int Type_ - тип хранения (см. TImageManager::Consts)
      virtual void setStorageType(const int Type_) { m_MapInterface->setStorageType(Type_); }
      //! \brief Получить признак записи скачаных тайлов в хранилище
      //! \return bool - true - тайлы записываются в хранилище
      virtual bool cacheInMappingStorage(void) const { return m_MapInterface->cacheInMappingStorage(); }
      //! \brief Получить указатель на управляющий класс
      //! \return Указатель на управляющий класс
      virtual TMapControl* mapControl(void) const { return m_MapInterface->mapControl(); }
      //! \brief 
      QSize mapSize(void) const { return m_MapInterface->mapSize(); }
      //! \brief 
      QSize baseMapSize(void) const { return m_MapInterface->baseMapSize(); }
      qreal pixelXScale(void) const { return m_MapInterface->pixelXScale(); }
      qreal pixelYScale(void) const { return m_MapInterface->pixelYScale(); }

   private:
      Q_DISABLE_COPY(TMapAdapterInterfaceWrapper)

      //! \brief Запомнить в адаптере карты указатель на управляющий класс
      //! \param TMapControl* MapControl_ - Указатель на управляющий класс
      void setMapControl(TMapControl* MapControl_) { m_MapInterface->setMapControl(MapControl_); }

      //! \var Указатель на интерфейс карты
      IMapAdapterInterface* m_MapInterface;
   };
}

#define IMapAdapterInterface_IID "QNetMap.IMapAdapterInterface"
Q_DECLARE_INTERFACE(qnetmap::IMapAdapterInterface, IMapAdapterInterface_IID)

#endif
