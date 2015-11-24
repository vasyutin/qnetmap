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

#ifndef QNETMAP_WIDGET_H
#define QNETMAP_WIDGET_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QWidget>
   #include <QBoxLayout>
   #include <QLabel>
   #include <QPrintDialog>
   #include <QPrinter>
#else
   #include <QtPrintSupport/QPrinter>
#endif
#include <QtGui>
#include <QPluginLoader>
#include <QDir>
#include <QStringList>

#include "qnetmap_mapcontrol.h"

namespace qnetmap
{
   class TMapAdapter;
   class TLayer;
   class IMapAdapterInterface;
   class TCurrentCoordinate;
   class TRulerScaling;

   //////////////////////////////////////////////////////////////////////////
   class TMapPluginParameters
   {
   public:
      explicit TMapPluginParameters(IMapAdapterInterface* MapAdapterInterface_)
         : w_MapAdapterInterface(MapAdapterInterface_) {}
      virtual ~TMapPluginParameters() {}

      //! \brief Получить ID карты
      //! \return QString - ID карты
      QString mapID(void) const { return w_MapAdapterInterface->UUID(); }
      //! \brief Получить тип карты
      //! \return QString - тип карты
      int mapType(void) const { return w_MapAdapterInterface->mapType(); }
      //! \brief Получить описание карты
      //! \return QString - описание карты
      QString mapDescription(void) const { return w_MapAdapterInterface->description(); }
      //! \brief Получить имя карты
      //! \return QString - имя карты
      QString mapName(void) const { return w_MapAdapterInterface->pluginName(); }
      //! \brief Получить указатель на интерфейс карты
      //! \return IMapAdapterInterface* - Указатель на интерфейс карты
      IMapAdapterInterface* mapAdapterInterface(void)  const { return w_MapAdapterInterface; }

   private:
      TMapPluginParameters() : w_MapAdapterInterface(NULL) {}

      //! \var Указатель на интерфейс адаптера карты
      IMapAdapterInterface* w_MapAdapterInterface;
   };

   // Типы карт (плагинов), поддерживаемых в данной реализации системы.
   // Каждый элемент состоит из 2 полей: 
   // - наименование карты для меню;
   // - класс, содержащий параметры карты
   typedef QMap<QString, TMapPluginParameters> TSupportedMaps;

   //////////////////////////////////////////////////////////////////////////
   class QNETMAP_EXPORT TMapWidget: public TMapControl 
   {
      typedef TMapControl TParent;

      Q_OBJECT

   public:
	   typedef TMapControl TParent;
      // Список слоев, содержащих прозрачные карты
      // Каждый элемент состоит из 2 полей: 
      // - наименование карты для меню;
      // - указатель на слой, содержащий карту
      typedef QMap<QString, TLayer*> TTransparentLayers;
   	
      //! \brief Конструктор класса
      //! \param IMapAdapterInterfaces MapInterfaces_ - список всех интерфейсов карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param TMapControl::TSelectedArea* Area_ - отображаемая область
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& MapID_, int Zoom_, TMapControl::TSelectedArea* Area_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты и масштаб 
      //!        устанавливаются последними использованными
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param TMapControl::TSelectedArea* Area_ - отображаемая область
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_, TMapControl::TSelectedArea* Area_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param QPointF CenterPoint_ - координаты центральной точки
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& MapID_, int Zoom_, QPointF CenterPoint_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты, масштаб и координаты центральной точки 
      //!        устанавливаются последними использованными
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //! \param QString PluginsPath_ - путь к плагинам карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param QPointF CenterPoint_ - координаты центральной точки
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const QString& PluginsPath_,
         const QString& MapID_, int Zoom_, QPointF CenterPoint_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты, масштаб и координаты центральной точки 
      //!        устанавливаются последними использованными
      //! \param QString PluginsPath_ - путь к плагинам карт
      //! \param QString CacheDirectory_ - папка с кешем
      //! \param QString MapsStorageDirectory - папка с хранилищем
      //! \param int MapsStorageType_ - тип хранилища
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const QString& PluginsPath_,
         const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
         const QString& Organization_, const QString& Application_,
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param TMapControl::TSelectedArea* Area_ - отображаемая область
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& MapID_, int Zoom_, TMapControl::TSelectedArea* Area_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты и масштаб
      //!        устанавливаются последними использованными
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param TMapControl::TSelectedArea* Area_ - отображаемая область
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_, TMapControl::TSelectedArea* Area_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param QPointF CenterPoint_ - координаты центральной точки
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& MapID_, int Zoom_, QPointF CenterPoint_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты, масштаб и координаты центральной точки 
      //!        устанавливаются последними использованными
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const IMapAdapterInterfaces& MapInterfaces_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //! \param QString PluginsPath_ - путь к плагинам карт
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param QPointF CenterPoint_ - координаты центральной точки
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const QString& PluginsPath_,
         const QString& MapID_, int Zoom_, QPointF CenterPoint_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief Конструктор класса
      //!        Идентификатор карты, масштаб и координаты центральной точки 
      //!        устанавливаются последними использованными
      //! \param QString PluginsPath_ - путь к плагинам карт
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      //! \param QWidget *Parent_ - указатель на родительский виджет
      TMapWidget(
         const QString& PluginsPath_,
         const QString& Organization_ = "", const QString& Application_ = "",
         QWidget *Parent_ = NULL);
      //! \brief
      virtual ~TMapWidget();

      //! \brief Поддерживаемые непрозрачные карты (основные)
      static TSupportedMaps& supportedMaps(void);
      //! \brief Поддерживаемые непрозрачные карты (основные)
      //! \param IMapAdapterInterfaces& MapInterfaces_ - список всех интерфейсов карт
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      static TSupportedMaps& supportedMaps(
         const IMapAdapterInterfaces& MapInterfaces_, const QString& Organization_, const QString& Application_);
      //! \brief Поддерживаемые растровые карты
      static TSupportedMaps& supportedRasterMaps(void);
      //! \brief Поддерживаемые прозрачные карты
      static TSupportedMaps& supportedTransparentMaps(void);
      //! \brief Возвращает интерфейсы растровых карт по даным реестра
      static IMapAdapterInterfaces& rasterMapInterfaces(void);
      //! \brief Возвращает интерфейс "пустой" карты
      static IMapAdapterInterface* emptyMapInterface(void) { return w_EmptyMapInterface; }
      //! \brief Возвращает исходный интерфейс растровой карты
      static IMapAdapterInterface* rasterMapInterface(void) { return w_RasterMapInterface; }
      //! \brief Adds a layer of paint
      //! \param QString& LayerName_ - layer name
      //! \return true if success
      bool addGeometryLayer(const QString& LayerName_);
      //! \brief Sets current layer for paint
      //! \param QString& LayerName_ - layer name
      //! \return true if success
      bool setCurrentGeometryLayer(const QString& LayerName_);
      //! \brief Добавляет растровую карту в список
      //! \param QString& FilePath_ - путь к файлу карты
      //! \param QString& MapName - имя карты
      bool addRasterMap(const QString& FilePath_, const QString& MapName_);
      //! \brief Удаляет растровую карту из списка
      //! \param QString& MapName - имя карты
      bool deleteRasterMap(const QString& MapName_);
      //! \brief Показать карту, плагин которой имеет указанное имя 
      //! \param QString MapName_ - имя плагина
      void showMapName(const QString &MapName_);
      //! \brief Показать прозрачную карту, плагин которой имеет указанное имя 
      //! \param QString MapName_ - имя плагина
      void showTransparentMapName(const QString &MapName_);
      //! \brief Показать карту нужного типа. 
      //! \param QString MapID_ - внутренний идентификатор, под которым эта карта (плагин) 
      //!                         известен этому виджету.
      //!		                  Один из тех, которые ранее вернула функция supportedMaps.
      //! \return true, если карта с данным ID показана
      bool showMapID(const QString &MapID_);
      //! \brief Показать/скрыть прозрачную карту нужного типа
      //! \param QString MapID_ - внутренний идентификатор, под которым эта карта (плагин) 
      //!                         известен этому виджету.
      //!		                   Один из тех, которые ранее вернула функция supportedTransparentMaps.
      void showTransparentMapID(const QString &MapID_);
      //! \brief Возвращает тип карты, показываемой в данный момент.
      QString mapID(void) const;
      //! \brief Возвращает тип прозрачной карты, соответствующий имени карты.
      //! \param QString MapName_ - имя плагина
      QString transparentMapID(const QString &MapName_) const;
      //! \brief Возвращает имя карты, показываемой в данный момент.
      QString mapName(void) const;
      //! \brief Получить список интерфейсов поддерживаемых карт
      IMapAdapterInterfaces& mapInterfaces(void);
      //! \brief Получить указатель на слой, содержащий прозрачную карту
      //! \param QString MapID_ - внутренний идентификатор, под которым эта карта (плагин) 
      //!                         известен этому виджету.
      //!		                   Один из тех, которые ранее вернула функция supportedMaps.
      //! \return TLayer* - указатель на слой, содержащий карту
      TLayer* transparentLayerForID(const QString& MapID_) const;
      //! \brief Получить указатель на слой, содержащий прозрачную карту
      //! \param QString MapName_ - Имя плагина карты.
      //! \return TLayer* - указатель на слой, содержащий карту
      TLayer* transparentLayerForName(const QString& MapName_) const;
      //! \brief Получить список типов видимых прозрачных карт
      QStringList visibleTransparentMaps(void) const;
      //! \brief Скрывает все прозрачные карты
      void hideAllTransparentMaps(void);
      //! \brief Адаптер основной карты
      //! \return TMapAdapter* - указатель на адаптер основной карты
      inline TMapAdapter* baseMapAdapter(void) const { return w_BaseMapAdapter; }
      //! \brief Указатель на информационный уровень
      inline TLayer* geometryLayer(void) const { return w_GeometryLayer; }
      //! \brief Инициализация растровых карт
      static void initializeRasterMaps(void);
      //! \brief Устанавливает видимость слоя с картой
      //! \param QString MapID_ - Идентификатор слоя
      //! \param bool Visibility_ - Видимость
      void setVisibleMapID(const QString& MapID_, bool Visibility_);
      //! \brief Возвращает состояние видимости слоя с базовой картой
      bool isVisibleBaseMap(void);
      //! \brief Устанавливает видимость слоя с базовой картой
      //! \param bool Visibility_ - Видимость
      void setVisibleBaseMap(bool Visibility_);
      //! \brief Устанавливает тип отображения координат текущей точки
      //! \param int View_ - тип отображения координат текущей точки (См. TCurrentCoordinate::Consts)
      void setCurrentCoordinateView(int View_);
      //! \brief Устанавливает порядок отображения координат текущей точки
      //! \param int Order_ - порядок отображения координат текущей точки (См. TCurrentCoordinate::Consts)
      void setCurrentCoordinateOrder(int Order_);
      //! \brief Устанавливает активность виджета
      //! \param bool Enabled_ - Активность
      void setEnabled(bool Enabled_);
      //! \brief сохраняет данные интерфейсов растровых карт в реестре
      void saveRasterMapInterfaces(void) const;
      //! \brief Установить путь хранилища тайлов всех интернет-карт виджета
      //! \param QString Path_ - путь к хранилищу (без последнего каталога)
      void setAllMapsStoragePath(const QString& Path_);
      //! \brief Установить тип хранения всех интернет-карт виджета
      //! \param int Type_ - тип хранения (см. TImageManager::Consts)
      void setAllMapsStorageType(const int Type_);
      //! \brief Установить путь к кешу
      //! \param QString Path_ - путь к кешу
      void setCacheDirectory(const QString& Path_);
      //! \brief Возвращает установленную в виджете строку организации
      static QString organization(void) { return m_Organization; }
      //! \brief Возвращает установленную в виджете строку приложения
      static QString application(void) { return m_Application; }
      //! \brief Возвращает количество созданных виджетов TMapWidget
      static unsigned widgetsCount(void) { return m_AtomicLoadedWidgetsCount; }
      //! \brief Make the image of the given area
      //! \param Rect_ The given rectangle geographic area.
      //! \param MapProviderID_ The map provider unique identifier
      //! \param ImageWidth_ The width of the result image.
      //! \param ImageHeight_ The height of the result image.
      //! \param Pixmap_ The result Image 
      //! \param Description_ The description of the given area if it's possible to get.
      //! \return In case of error returns false. If no error occurred returns true.
      static bool areaImage(
         /*in*/ const QString& MapProviderID_, 
         /*in*/ const QRectF &Rect_,
         /*in*/ const int ImageWidth_,
         /*in*/ const int ImageHeight_,
         /*out*/ QPixmap &Pixmap_,
         /*out*/ QString &Description_);
      //! \brief Вызывает посылку сигнала об изменении в списке растровых карт
      void rasterMapsLinkChanged(void);
      //! \brief Поиск интерфейса карты по ее ID среди непрозрачных карт
      //! \param MapID_ The map provider unique identifier
      static IMapAdapterInterface* findBaseMapInterface(const QString& MapID_);
      //! \brief Вычисление масштаба, при котором нужная область будет вписана в указанные рамки
      int calculateZoom(const QRectF Rect_, const uint ImageWidth_, const uint ImageHeight_);
      //! \brief Вычисление масштаба, при котором нужная область будет вписана в указанные рамки.
      //!        Используются параметры указанной карты.
      static int calculateZoom(IMapAdapterInterface* MapAdapter_, 
                               const QRectF Rect_,
                               const uint ImageWidth_, 
                               const uint ImageHeight_);
      //! \brief Возвращает состояние виджета, свернутое в байтовый массив
      QByteArray getSettings(void) const;
      //! \brief Восстанавливает состояние виджета по байтовому массиву, полученному через getSettings
      void restoreSettings(QByteArray &ByteArray_);
      //! \brief Supported base maps count
      int mapsCount(void) const { return m_Maps.count(); }
      //! \brief Supported transparent maps count
      int transparentMapsCount(void) const { return m_TransparentMaps.count(); }
      //! \brief Supported raster maps count
      int rasterMapsCount(void) const { return m_RasterMaps.count(); }
      //! \brief 
      static QRgb colorSelected(void) { return m_ColorSelected; }
      //! \brief 
      static void setColorSelected(const QRgb Color_) { m_ColorSelected = Color_; }
      //! \brief 
      static QRgb colorFocused(void) { return m_ColorFocused; }
      //! \brief 
      static void setColorFocused(const QRgb Color_) { m_ColorFocused = Color_; }
      //! \brief 
      static QRgb mainColorPointsInEditMode(void) { return m_MainColorPointsInEditMode; }
      //! \brief 
      static void setMainColorPointsInEditMode(const QRgb Color_) { m_MainColorPointsInEditMode = Color_; }
      //! \brief 
      static QRgb secondaryColorPointsInEditMode(void) { return m_SecondaryColorPointsInEditMode; }
      //! \brief 
      static void setSecondaryColorPointsInEditMode(const QRgb Color_) { m_SecondaryColorPointsInEditMode = Color_; }
      //! \brief Устанавливает видимость координат курсора
      //! \param bool Visibility_ - Видимость
      void setVisibityCurrentCoordinate(bool Visibility_);
		//! \brief 
		void showCoordinates(void);
		//! \brief 
		void hideCoordinates(void);

   protected:
      void resizeEvent(QResizeEvent *Event_);
      void enterEvent(QEvent *Event_);
      void leaveEvent(QEvent *Event_);
      void wheelEvent(QWheelEvent *Event_);
      void keyPressEvent(QKeyEvent* Event_);
      void keyReleaseEvent(QKeyEvent *Event_);

      //! \class TMapPrinter
      //! \brief QPrinter, дополненный необходимыми данными
      class TMapPrinter : public QPrinter
      {
      public:
         TMapPrinter(PrinterMode Mode_, QRect Rect_ = QRect()) : QPrinter(Mode_), m_Rect(Rect_) {}
         virtual ~TMapPrinter() {}

         //! \brief Получить кординаты печатаемого прямоугольника
         QRect rect(void) const { return m_Rect; }

      private:
         //! \var Координаты печатаемого прямоугольника
         QRect m_Rect;
      };

   private:
      Q_DISABLE_COPY(TMapWidget)

      //! \brief Инициализация виджета
      //! \param QString MapID_ - Идентификатор отображаемой карты
      //! \param int Zoom_ - отображаемый масштаб
      //! \param QPointF CenterPoint_ - координаты центральной точки
      //! \param QString Organization_ - организация (для поиска в реестре)
      //! \param QString Application_ - приложение (для поиска в реестре)
      void initialize(const QString& MapID_, int Zoom_, QPointF CenterPoint_,
                      const QString& Organization_ = "", const QString& Application_ = "");
      //! \brief Инициализация карт
      static void initializeMaps(void);
      //! \brief Установка параметров карт
      void setInterfaceParameters(void);
      //! \brief Центральная точка области
      //! \param TMapControl::TSelectedArea* Area_ - область
      static QPointF mapCenterPoint(TMapControl::TSelectedArea* Area_);
      //! \brief Добавляет растровую карту в список
      //! \param QString& FilePath_ - путь к файлу карты
      //! \param QString& MapName - имя карты
      static bool staticAddRasterMap(const QString& FilePath_, const QString& MapName_);
      //! \brief Удаляет растровую карту из списка
      //! \param QString& MapName - имя карты
      static bool staticDeleteRasterMap(const QString& MapName_);
      //! \brief Добавляет необходимые виджеты на карту
      void addWidgets(void);
      //! \brief Устанавливает параметры печати
      //! \param QPrinter &Printer_ - устройство, для которого устанавливаются параметры
      void setPrinterParameters(QPrinter& Printer_) const;

      //! \var счетчик копий класса
      static QAtomicInt m_AtomicLoadedWidgetsCount;
      //! \var указатель на виджет, отображающий текущие координаты
      TCurrentCoordinate* w_CurrentCoordinate;
      bool m_VisibleCurrentCoordinate;
      //! \var список интерфейсов карт
      static IMapAdapterInterfaces m_MapInterfaces;
      //! \var список интерфейсов прозрачных карт
      static IMapAdapterInterfaces m_TransparentMapInterfaces;
      //! \var список интерфейсов растровых карт
      static IMapAdapterInterfaces m_RasterMapInterfaces;
      //! \var список интерфейсов карт для рисования
      static IMapAdapterInterfaces m_EmptyMapInterfaces;
      //! \var Индексированный список поддерживаемых карт с параметрами
      static TSupportedMaps m_Maps;
      //! \var Индексированный список поддерживаемых прозрачных карт с параметрами
      static TSupportedMaps m_TransparentMaps;
      //! \var Индексированный список растровых карт с параметрами
      static TSupportedMaps m_RasterMaps;
      //! \var Указатель на интерфейс плагина с 'пустой' картой
      static IMapAdapterInterface *w_EmptyMapInterface;
      //! \var Указатель на интерфейс плагина растровой карты
      static IMapAdapterInterface *w_RasterMapInterface;
      //! \var Последняя установленная основная карта (среди всех виджетов)
      static QString m_LastBaseMapID;
      //! \var Последняя установленная координата центра карты (среди всех виджетов)
      static QPointF m_LastCenterPoint;
      //! \var Последний установленный масштаб (среди всех виджетов)
      static int m_LastZoom;
      //! \var Список слоев прозрачных карт
      TTransparentLayers w_TransparentMapLayers;
      //! \var Указатель на адаптер основной карты
      TMapAdapter* w_BaseMapAdapter;
      //! \var Указатель на слой основной карты
      TLayer* w_BaseMapLayer;
      //! \var Тип основной карты
      QString m_BaseMapID;
      //! \var Указатель на интерфейс основной карты
      IMapAdapterInterface* w_BaseMapInterface;
      //! \var Указатель на слой для рисования
      TLayer* w_GeometryLayer;
      //! \var Путь к кешу
      static QString m_CacheDirectory;
      //! \var Путь к хранилищу
      static QString m_MapsStorageDirectory;
      //! \var Тип хранилища
      static int m_MapsStorageType;
      //! \var Организация (для поиска в реестре)
      static QString m_Organization;
      //! \var Приложение (для поиска в реестре)
      static QString m_Application;
      //! \var Признак того, что плагины загружены
      static bool m_LoadPlugins;
      //! \var Указатель на линейку масштаба
      TRulerScaling* w_RulerScaling;
      //! \var 
      static QRgb	m_ColorSelected;
      //! \var 
      static QRgb	m_ColorFocused;
      //! \var 
      static QRgb	m_MainColorPointsInEditMode;
      //! \var 
      static QRgb	m_SecondaryColorPointsInEditMode;

   public slots:
      //! \brief Все, что нужно сделать при изменениях карты
      //! \param QPointF &Coordinate_ - Координаты центра области
      //! \param int Zoom_ - Текущий масштаб
      void changeView(const QPointF &Coordinate_, int Zoom_);
      //! \brief Печать видимой области карты
      void print(void);
      //! \brief Печать видимой области карты с просмотром
      void printPreview(void);
      //! \brief Выбор области карты с последующей печатью
      void printArea(void);
      //! \brief Печать указанной области карты
      //! \param QRect Rect_ - прямоугольник с координатами области
      //! \param int Zoom_   - не используется
      void printAreaSelected(const QRect Rect_, int Zoom_);
      //! \brief Выбор области карты с последующими просмотром и печатью
      void printAreaPreview(void);
      //! \brief Просмотр и печать указанной области карты
      //! \param QRect Rect_ - прямоугольник с координатами области
      //! \param int Zoom_   - не используется
      void printAreaPreviewSelected(const QRect Rect_, int Zoom_);
      //! \brief Собственно печать
      //! \param QPrinter* Printer_ - указатель на класс TMapPrinter
      void printMap(QPrinter* Printer_);
      //! \brief Выбор области для формирования заданий на загрузку тайлов
      void chooseLoadingArea(void);
      //! \brief Диалог выбора параметров и формирования заданий на загрузку тайлов
      //! \param QRect Rect_ - прямоугольник с координатами области
      //! \param int Zoom_   - масштаб карты
      void loadMapsDialog(const QRect Rect_, int Zoom_);
      //! \brief настройка растровых карт 
      void rasterMapsDialog(void);
      //! \brief увеличение масштаба на единицу
      void zoomInClicked(void);
      //! \brief уменьшение масштаба на единицу
      void zoomOutClicked(void);
      //! \brief Устанавливает максимальный масштаб для отображения всех объектов карты
      void fitToPageAll(void);
      //! \brief Устанавливает максимальный масштаб для отображения указанной области
      void fitToPage(const QRectF& RectF_);
      //! \brief изменение масштаба
      void zoomChanged(int Zoom_) { setZoom(Zoom_); }

   signals:
      // После изменений мы должны сообщить, какая базовая карта сейчас показана. 
      void showsMapID(QString);
      // После изменений мы должны сообщить, какая растровая карта сейчас показана. 
      void showsRasterMapID(QString);
      // после изменений мы должны сообщить о переключении видимости слоя карты 
      void changedVisibility(QString, bool);
      // Изменились параметры карты
      void viewChanged(QString, int, QPointF);
      // Изменился список растровых карт
      void rasterMapsListChanged();
   };
}
#endif // #ifndef QNETMAP_WIDGET_H
