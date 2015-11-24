/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl MapAdapter code by Kai Winter
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

#ifndef QNETMAP_MAPADAPTER_H
#define QNETMAP_MAPADAPTER_H

#include "qnetmap_global.h"
#include "qnetmap_mapadapterparameters.h"

#include <QRect>
#include <QPoint>
#include <QPixmap>
#include <QPointF>
#include <QSize>
#include <QLocale>
#include <QMutex>

namespace qnetmap
{
   class TMapControl;

   //! \class TMapAdapter
   //! \brief Класс адаптера карты
   //! Used to fit map servers into QMapControl
   /*!
    * MapAdapters are needed to convert between world- and display coordinates.
    * This calculations depend on the used map projection.
    * There are two ready-made MapAdapters:
    *  - TileMapAdapter, which is ready to use for OpenStreetMap or Google (Mercator projection)
    *  - WMSMapAdapter, which could be used for the most WMS-Server (some servers show errors, because of image ratio)
    *
    * MapAdapters are also needed to form the HTTP-Queries to load the map tiles.
    * The maps from WMS Servers are also divided into tiles, because those can be better cached.
    *
    * @see TileMapAdapter, @see WMSMapAdapter
    *
    *	@author Kai Winter <sergey.shumeyko@gmail.com>
    */
   class QNETMAP_EXPORT TMapAdapter : public TBase
   {
      Q_OBJECT

   public:
      typedef TBase TParent;

      //! \struct Consts
      struct Consts {
         // Константы типов источников карт
         static const uint InternetMap       = 11; //!< из интернет
         static const uint RasterMap         = 12; //!< растровая
         static const uint EmptyMap          = 13; //!< пустая
         static const uint UniversalTileMap  = 14; //!< универсальная тайловая
         // Константы прозрачности карт
         static const uint OpaqueMap         = 21; //!< непрозрачная
         static const uint TransparentMap    = 22; //!< прозрачная
         // Константы типов карт
         static const uint TypeSatelliteMap  = 31; //!< спутниковая
         static const uint TypeSchemeMap     = 32; //!< схема
         static const uint TypeRoadMap       = 33; //!< дороги
         static const uint TypeRasterMap     = 34; //!< растровая
         static const uint TypeEmptyMap      = 35; //!< пустая
         static const uint TypeInfoMap       = 36; //!< информационная
         // Константы типов проекций
         static const uint ProjectionTypeMercator = 41; //!< Меркатор
         // Константы базисов проекций
         static const uint ProjectionBasisEPSG3857 = 51; //!< Google сфера
         static const uint ProjectionBasisEPSG3395 = 52; //!< Сфероид WGS-84 (Yandex)
         // Константы типов нумерации тайлов
         static const uint TilesNumbering00TopLeft    = 61; //!< Google
         static const uint TilesNumberingYahoo        = 62; //!< Yahoo
         static const uint TilesNumbering00BottomLeft = 63; //!< Navitel
         // Константы типов обновляемости тайлов
         static const uint QueryUpdatabilityStatic  = 71; //!< Статичные
         static const uint QueryUpdatabilityDynamic = 72; //!< Регулярно изменяемые
      };

      friend class TLayer;
      friend class TImageManager;
      
      virtual ~TMapAdapter();

      virtual bool isGeocoded(void) const = 0;
      virtual void setURL(const QString&, const QString&) = 0;
      virtual QString host(const bool Random_ = false) const = 0;
      virtual unsigned hostsCount(void) const = 0;
      virtual QPointF centerMap(void) const = 0;
      virtual QPointF topLeftMap(void) const = 0;
      virtual QPointF bottomRightMap(void) const = 0;
      //! \brief Проверка существования тайла с данными x, y, z подразумевается текущий
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \return bool - true, если комбинация верная
      virtual bool isValid(int x, int y) const { return isValid(x, y, currentZoom()); }
      //! \brief Проверка существования тайла с данными x, y, z
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      //! \return bool - true, если комбинация верная
      virtual bool isValid(int x, int y, int z) const;
      //! \brief Установить путь к файлу с растровой картой
      //! \param QString RasterFileFullPath_ - Путь к файлу с растровой картой
      virtual void setRasterFileFullPath(const QString&) {}
      //! \brief Путь к файлу с растровой картой
      virtual QString rasterFileFullPath(void) const { return QString(); }
      //! \brief текущая версия
      virtual QString mapVersion(void) const { return QString(); }
      //! \brief размер тайла по оси X
      //! \return int - размер тайла по оси X в пикселах
      virtual int tileSizeX() const;
      //! \brief размер тайла по оси Y
      //! \return int - размер тайла по оси Y в пикселах
      virtual int tileSizeY() const;
      //! \brief глобальные пикселькоординаты
      //! \param int X_ - номер тайла по X
      //! \param int Y_ - номер тайла по Y
      virtual QRect tileRect(int X_, int Y_) const;
      //! returns the min zoom value
      virtual int minZoom() const;
      //! returns the max zoom value
      virtual int maxZoom() const;
      //! returns the current zoom
      virtual int currentZoom() const;
      //! \brief Перевод географических координат в пиксельные
      //! \param QPointF - географические координаты
      //! \return QPoint - пиксельные координаты
      virtual QPoint coordinateToDisplay(const QPointF& Point_, int Zoom_ = -1) const;
      //! \brief Перевод пиксельных координат в географические
      //! \param QPoint - пиксельные координаты
      //! \return QPointF - географические координаты
      virtual QPointF displayToCoordinate(const QPoint&) const;
      //! \brief Перевод географических координат в тайловые
      //! \param QPointF - географические координаты
      //! \return QPoint - тайловые координаты
      virtual QPoint coordinateToTile(const QPointF&) const;
      //! \brief Перевод географических координат в локальные пиксельные на тайле
      //! \param QPointF - географические координаты
      //! \return QPoint - локальные пиксельные координаты на тайле
      virtual QPoint coordinateToTileLocal(const QPointF&) const;
      //! \brief Запомнить в адаптере карты указатель на управляющий класс
      //! \param TMapControl* MapControl_ - Указатель на управляющий класс
      virtual void setMapControl(TMapControl* MapControl_) { m_MapControl = MapControl_; }
      //! \brief Получить указатель на управляющий класс
      //! \return Указатель на управляющий класс
      virtual TMapControl* mapControl(void) const { return m_MapControl; }
      //! \brief Определяет "активность" адаптера карты (объект существует)
      static bool mapAdapterIsActive(TMapAdapter* MapAdapter_);
      //! \brief Проверка прозрачности карты
      //! \return bool - Возвращает true, если карта прозрачная
      virtual bool isTransparent(void) const { return m_Transparency == Consts::TransparentMap; }
      //! \brief принцип нумерации тайлов
      virtual int tilesNumbering(void) const { return m_TilesNumbering; }
      //! \brief Проверка на "пустую" карту
      //! \return bool - Возвращает true, если карта "пустая"
      virtual bool isEmptyMap(void) const { return m_SourceType == Consts::EmptyMap; }
      //! \brief Устанавливает прозрачность карты
      //! \param int Transparency_ - прозрачность карты (см. Consts)
      virtual void setTransparency(const uint Transparency_);
      //! \brief Возвращает тип источника карты
      virtual uint sourceType(void) const { return m_SourceType; }
      //! \brief Устанавливает тип источника карты
      //! \param int SourceType_ - тип источника карты (см. Consts)
      virtual void setSourceType(const uint SourceType_);
      //! \brief Возвращает тип карты
      virtual int mapType(void) const { return m_MapType; }
      //! \brief тип обновляемости тайлов
      virtual uint updatability(void) const { return m_Updatability; }
      //! \brief Возвращает описание карты
      virtual QString description(void) const { return m_Description; }
      //! \brief Устанавливает описание карты
      //! \param QString Description_ - описание карты
      virtual void setDescription(const QString Description_) { m_Description = Description_; }
      //! \brief Устанавливает тип карты
      //! \param int MapType_ - тип карты (см. Consts)
      virtual void setMapType(const uint MapType_);
      //! \brief Возвращает признак того, что адаптер правильно инициализирован
      //! \return bool - Признак того, что плагин правильно инициализирован
      virtual bool isInitialized(void) const { return m_IsInitialized; }
      //! \brief Устанавливает признак правильной инициализации в истину
      virtual void setInitialized(bool Initialized_) { m_IsInitialized = Initialized_; }
      //! \brief Установить границы масштабирования
      //! \param int Min_ - минимальный масштаб
      //! \param int Max_ - максимальный масштаб
      virtual void setZoomRestrictions(const int Min_, const int Max_);
      //! \brief Перевод радиан в градусы
      //! \param qreal - радианы
      //! \return qreal - градусы
      virtual qreal rad_deg(const qreal) const;
      //! \brief Перевод градусов в радианы
      //! \param qreal - градусы
      //! \return qreal - радианы
      virtual qreal deg_rad(const qreal) const;
      //! \brief Уникальный идентификатор адаптера
      virtual QString UUID(void) const { return m_UUID; }
      //! \brief Установить тип хранения
      //! \param int Type_ - тип хранения (см. TImageManager::Consts)
      virtual void setStorageType(const int Type_) { m_StorageType = Type_; }
      //! \brief Получить тип хранения
      //! \return int - тип хранения (см. TImageManager::Consts)
      virtual int storageType(void) const { return m_StorageType; }
      //! \brief Получить имя плагина
      //! \return QString - имя
      virtual QString pluginName(void) const { return m_PluginName; }
      //! \brief Установить имя плагина
      //! \param QString - имя
      virtual void setPluginName(const QString& Name_);
      //! \brief размер карты
      virtual QSize mapSize(void) const;
      //! \brief размер базовой карты
      virtual QSize baseMapSize(void) const { return QSize(m_TileSizeX, m_TileSizeY); }
      //! \brief масштаб по оси х
      virtual qreal pixelXScale(void) const { return pixelXScale(m_CurrentZoom); }
      //! \brief масштаб по оси у
      virtual qreal pixelYScale(void) const { return pixelYScale(m_CurrentZoom); }
      //! \brief масштаб по оси х на указанном уровне
      virtual qreal pixelXScale(int Zoom_) const;
      //! \brief масштаб по оси у на указанном уровне
      virtual qreal pixelYScale(int Zoom_) const;
      //! \brief возвращает тайл по координатам х, у на масштабе z
      virtual QPixmap tile(int X_, int Y_, int Z_) const 
      {
      Q_UNUSED(X_)
      Q_UNUSED(Y_)
      Q_UNUSED(Z_)

      return QPixmap(); 
      }
      //! \brief установка параметров адаптера карты
      virtual void setParameters(TMapAdapterParameters* Parameters_);
      //! \brief тип проекции карты
      int projectionType(void) const { return m_ProjectionType; }
      //! \brief геофизическая основа карты (сфера, эллипс)
      int basis(void) const { return m_Basis; }
      //! \brief сдвиг масштаба относительно нумерации масштабов google
      int zoomShift(void) const { return m_ZoomShift; }
      //! \brief устанавливает путь к хранилищу
      virtual void setMappingStoragePath(const QString& Path_, const bool InsertIntoATemplate_ = false);
      //! \brief возвращает путь к хранилищу
      virtual QString mappingStoragePath(void) const { return m_MappingStoragePath; }

   private:
      //| \var Список активных адаптеров карт
      static QSet<TMapAdapter*> m_ActiveMapAdapters;
      //| \var Мьютекс списка активных адаптеров карт
      static QMutex m_ActiveMapAdaptersMutex;

      //! \var тип хранения (см. TImageManager::Consts)
      int m_StorageType;
      //! \var Прозрачность карты (см. Consts)
      uint m_Transparency;
      //! \var Тип источника карты (см. Consts)
      uint m_SourceType;
      //! \var Признак того, что адаптер правильно инициализирован
      bool m_IsInitialized;
      //! \var Тип карты (см. Consts)
      uint m_MapType;
      //! \var Описание карты
      QString m_Description;
      //! \var Указатель на управляющий класс
      TMapControl* m_MapControl;
      //! \var уникальный номер
      QString m_UUID;
      //! \var Имя плагина
      QString  m_PluginName;
      //! \var Имя папки в хранилище для тайлов этой карты
      QString m_FolderName;
      //! \var Шаблон пути к хранилищу тайлов карты (должен содержать %1 для вставки в это место пути)
      QString  m_TemplateMappingStoragePath;
      //! \var Путь к хранилищу тайлов карты
      QString  m_MappingStoragePath;

      //! \var тип проекции карты
      int m_ProjectionType;
      //! \var геофизическая основа карты (сфера, эллипс)
      int m_Basis;
      //! \var принцип нумерации тайлов
      int m_TilesNumbering;
      //! \var сдвиг масштаба относительно нумерации масштабов google
      int m_ZoomShift;
      //! \var тип обновляемости тайлов
      uint m_Updatability;

   protected:
      TMapAdapter(int TileSizeX_, int TileSizeY_, int minZoom = 0, int maxZoom = 0);
      TMapAdapter(const TMapAdapter& Source);
      //! \brief Количество тайлов по оси X на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      virtual int xtilesonzoomlevel(int zoomlevel) const;
      //! \brief Количество тайлов по оси Y на соответствующем масштабе
      //! \param int zoomlevel - масштаб
      //! \return int - количество тайлов
      virtual int ytilesonzoomlevel(int zoomlevel) const;
      //! \brief число тайлов по х на текущем масштабе
      virtual int numberOfXTiles(void) const { return m_NumberOfXTiles; }
      //! \brief число тайлов по оси у на текущем масштабе
      virtual int numberOfYTiles(void) const { return m_NumberOfYTiles; }
      //! \brief Смещение по x в тайлах
      //! \param int X_ - номер тайла по x
      //! \return int - смещение по x в тайлах
      virtual int xoffset(int x) const { return x; }
      //! \brief Смещение по y в тайлах
      //! \param int Y_ - номер тайла по y
      //! \return int - смещение по y в тайлах
      virtual int yoffset(int y) const { return y; }
      //! \brief Приведение масштаба
      //! \param int Z_ - масшаб
      //! \return int - приведенный масштаб
      virtual int zoom(int z) const { return z; }
      //! \brief Увеличивает масштаб на одну ступень
      virtual void zoom_in();
      //! \brief Уменьшает масштаб на одну ступень
      virtual void zoom_out();
      //! \brief Формирование строки для запроса тайла 
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      virtual QString query(const int x, const int y, const int z) const;
      //! \brief Устанавливает масштаб отображения карты 
      //! \param int ZoomLevel_ - масштаб
      virtual void setZoom(int ZoomLevel_);

      int m_MinZoom;
      int m_MaxZoom;
      int m_TileSizeX;
      int m_TileSizeY;
      int m_CurrentZoom;
      int m_NumberOfXTiles;
      int m_NumberOfYTiles;
      QLocale m_Locale;

      //! \var Число ПИ 
      static qreal PI;
   };
}

#endif
