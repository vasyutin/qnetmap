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

#include "qnetmap_global.h"

#ifndef QNETMAP_QT_4
   #include <QtPrintSupport/QPrintDialog>
   #include <QtPrintSupport/QPrintPreviewDialog>
#endif

#include "qnetmap_widget.h"

#include "qnetmap_imagemanager.h"
#include "qnetmap_layermanager.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_loadprogressbar.h"
#include "qnetmap_loaddialogwizard.h"
#include "qnetmap_currentcoordinate.h"
#include "qnetmap_rastermapsdialog.h"
#include "qnetmap_rulerscaling.h"
#include "qnetmap_maplayer.h"
#include "qnetmap_geometrylayer.h"

// -----------------------------------------------------------------------

#include <QFile>
#include <string>

namespace qnetmap
{
   QRgb TMapWidget::m_ColorSelected                  = 0x6699ccff;
   QRgb TMapWidget::m_ColorFocused                   = 0x660099ff;
   QRgb TMapWidget::m_MainColorPointsInEditMode      = 0x0000ff00;
   QRgb TMapWidget::m_SecondaryColorPointsInEditMode = 0x00ff0000;

   IMapAdapterInterfaces TMapWidget::m_MapInterfaces;
   IMapAdapterInterfaces TMapWidget::m_RasterMapInterfaces;
   IMapAdapterInterfaces TMapWidget::m_TransparentMapInterfaces;
   IMapAdapterInterfaces TMapWidget::m_EmptyMapInterfaces;
   TSupportedMaps TMapWidget::m_Maps;
   TSupportedMaps TMapWidget::m_RasterMaps;
   TSupportedMaps TMapWidget::m_TransparentMaps;
   QString TMapWidget::m_CacheDirectory = "";
   QString TMapWidget::m_MapsStorageDirectory = "";
   QString TMapWidget::m_Organization = "";
   QString TMapWidget::m_Application = "";
   int TMapWidget::m_MapsStorageType = 0;
   QAtomicInt TMapWidget::m_AtomicLoadedWidgetsCount = 0;
   IMapAdapterInterface* TMapWidget::w_EmptyMapInterface  = NULL;
   IMapAdapterInterface* TMapWidget::w_RasterMapInterface = NULL;
   bool TMapWidget::m_LoadPlugins = false;
   QString TMapWidget::m_LastBaseMapID = "";
   QPointF TMapWidget::m_LastCenterPoint = QPointF(0, 0);
   int TMapWidget::m_LastZoom = 0;

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& MapID_, int Zoom_, TMapControl::TSelectedArea* Area_,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, Area_, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(MapID_, Zoom_, mapCenterPoint(Area_), Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_, TMapControl::TSelectedArea* Area_,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, Area_, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(m_LastBaseMapID, m_LastZoom, mapCenterPoint(Area_), Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& MapID_, int Zoom_, QPointF CenterPoint_,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(MapID_, Zoom_, CenterPoint_, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(m_LastBaseMapID, m_LastZoom, m_LastCenterPoint, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const QString& PluginsPath,
      const QString& MapID_, int Zoom_, QPointF CenterPoint_,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   // загружаем плагины карт и получаем список интерфейсов
   if(m_MapInterfaces.isEmpty()) {
      m_MapInterfaces = loadPlugins(PluginsPath);
      if(!m_MapInterfaces.isEmpty()) m_LoadPlugins = true;
      }
   //
   initialize(MapID_, Zoom_, CenterPoint_, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const QString& PluginsPath,
      const QString& CacheDirectory_, const QString& MapsStorageDirectory_, int MapsStorageType_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   m_CacheDirectory = CacheDirectory_;
   m_MapsStorageDirectory = MapsStorageDirectory_;
   m_MapsStorageType = MapsStorageType_;
   // загружаем плагины карт и получаем список интерфейсов
   if(m_MapInterfaces.isEmpty()) {
      m_MapInterfaces = loadPlugins(PluginsPath);
      if(!m_MapInterfaces.isEmpty()) m_LoadPlugins = true;
      }
   //
   initialize(m_LastBaseMapID, m_LastZoom, m_LastCenterPoint, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& MapID_, int Zoom_, TMapControl::TSelectedArea* Area_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, Area_, Parent_)
   {
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(MapID_, Zoom_, mapCenterPoint(Area_), Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_, TMapControl::TSelectedArea* Area_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, Area_, Parent_)
   {
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(m_LastBaseMapID, m_LastZoom, mapCenterPoint(Area_), Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& MapID_, int Zoom_, QPointF CenterPoint_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(MapID_, Zoom_, CenterPoint_, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const IMapAdapterInterfaces& MapInterfaces_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   if(m_MapInterfaces.empty()) m_MapInterfaces = MapInterfaces_;
   //
   initialize(m_LastBaseMapID, m_LastZoom, m_LastCenterPoint, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const QString& PluginsPath,
      const QString& MapID_, int Zoom_, QPointF CenterPoint_,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   // загружаем плагины карт и получаем список интерфейсов
   if(m_MapInterfaces.empty()) {
       m_MapInterfaces = loadPlugins(PluginsPath);
       if(!m_MapInterfaces.isEmpty()) m_LoadPlugins = true;
   }
   //
   initialize(MapID_, Zoom_, CenterPoint_, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::TMapWidget(
      const QString& PluginsPath,
      const QString& Organization_, const QString& Application_, QWidget *Parent_) 
      : TMapControl(QSize(640, 480), TMapControl::Dragging, true, true, NULL, Parent_)
   {
   // загружаем плагины карт и получаем список интерфейсов
   if(m_MapInterfaces.empty()) {
       m_MapInterfaces = loadPlugins(PluginsPath);
       if(!m_MapInterfaces.isEmpty()) m_LoadPlugins = true;
   }
   //
   initialize(m_LastBaseMapID, m_LastZoom, m_LastCenterPoint, Organization_, Application_);
   }

   //////////////////////////////////////////////////////////////////////////
   TMapWidget::~TMapWidget() 
   {
   m_AtomicLoadedWidgetsCount.fetchAndAddOrdered(-1);
   //
   if(m_AtomicLoadedWidgetsCount == 0) {
      TMapControl::deleteLayerManager();
      //
      foreach(IMapAdapterInterface* Interface, m_RasterMapInterfaces) delete Interface;
      m_RasterMapInterfaces.clear();
      m_RasterMaps.clear();
      w_RasterMapInterface = NULL;
      //
      m_TransparentMapInterfaces.clear();
      m_TransparentMaps.clear();
      //
      m_EmptyMapInterfaces.clear();
      w_EmptyMapInterface = NULL;
      //
      if(m_LoadPlugins) foreach(IMapAdapterInterface* Interface, m_MapInterfaces) delete Interface;
      m_MapInterfaces.clear();
      m_Maps.clear();
      w_BaseMapAdapter = NULL;
      }
   }

   //--------------------------------------------------------------------------
   QPointF TMapWidget::mapCenterPoint(TMapControl::TSelectedArea* Area_)
   {
   QPointF MapCenterPoint;
   if(Area_) {
      switch(Area_->type()) {
         case TMapControl::PointAreaType:
            MapCenterPoint = *(Area_->points().begin());
            break;
         case TMapControl::RubberBandAreaType:
            {
            QList<QPointF> Points = Area_->points();
            QPointF Point1 = Points.first();
            QPointF Point2 = *(Points.begin() + 1);
            MapCenterPoint = QPointF((Point1.x() + Point2.x()) / 2, (Point1.y() + Point2.y()) / 2);
            break;
            }
         default:
            break;
         }
      }
   return MapCenterPoint;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::initialize(
      const QString& MapID_, int Zoom_, QPointF CenterPoint_, const QString& Organization_, const QString& Application_) 
   {
   TImageManager::instance()->setLoadTiles(false);
   //
   if(m_Organization.isEmpty()) {
      if(Organization_.isEmpty()) m_Organization = QNetMapConsts::Organization;
      else                        m_Organization = Organization_;
      }
   if(m_Application.isEmpty()) {
      if(Application_.isEmpty())  m_Application  = QApplication::applicationName();
      else                        m_Application  = Application_;
      }
   // считываем пути из реестра
   QSettings Settings(m_Organization, m_Application);
   if(m_CacheDirectory.isEmpty()) {
      m_CacheDirectory = QDir::tempPath()/*QApplication::applicationDirPath()*/ + "/cache";
      if(Settings.contains(QString(QNetMapConsts::CachePathKey) + "_0")) {
         QString Directory = Settings.value(QString(QNetMapConsts::CachePathKey) + "_0").toString();
         if(QDir(Directory).exists()) m_CacheDirectory = Directory;
         }
      }
   if(m_MapsStorageType == 0) {
      if(Settings.contains(QString(QNetMapConsts::StorageTypeKey))) {
         m_MapsStorageType = Settings.value(QString(QNetMapConsts::StorageTypeKey)).toInt();
         }
      else m_MapsStorageType = TImageManager::Consts::InternetOnly;
      }
   if(m_MapsStorageDirectory.isEmpty()) {
      if(Settings.contains(QString(QNetMapConsts::StoragePathKey) + "_0")) {
         QString Directory = Settings.value(QString(QNetMapConsts::StoragePathKey) + "_0").toString();
         // проверка не только на наличие папки, но и на возможность записи в нее
         if(QDir(Directory).exists() && TQNetMapHelper::checkDirectoryForWritePermissions(Directory)) {
            m_MapsStorageDirectory = Directory;
         }
         else m_MapsStorageType = TImageManager::Consts::InternetOnly;  // ставим только Интернет
         }
      else m_MapsStorageType = TImageManager::Consts::InternetOnly; // хранилище не задано, ставим только Интернет
      }

   if(!m_MapInterfaces.empty()) {
      w_BaseMapAdapter    = NULL;
      w_BaseMapLayer      = NULL;
      m_BaseMapID         = MapID_;
      w_GeometryLayer     = NULL;
      w_CurrentCoordinate = NULL;
      m_VisibleCurrentCoordinate = true;
      //
      enablePersistentCache(m_CacheDirectory);
      //
      addWidgets();
      // плагин карты
      /// восстанавливаем настройки
      initializeMaps();
      showMapID(MapID_);
      setZoom(Zoom_);
      w_RulerScaling->setValue(Zoom_);
      if(CenterPoint_ != QPointF()) setView(CenterPoint_);
      showCrosshairs(true);
      /// добавляем слои прозрачных карт
      /// у каждой карты свой слой, видимость которого управляется через меню
      if(!m_TransparentMaps.empty()) {
         for(TSupportedMaps::iterator it = m_TransparentMaps.begin(); it != m_TransparentMaps.end(); ++it) {
            IMapAdapterInterface *MapInterface = it.value().mapAdapterInterface();
            //// создаем новый объект интерфейса
            IMapAdapterInterface *NewMapInterface = 
               qobject_cast<IMapAdapterInterface*>(MapInterface->createInstance());
            m_TransparentMapInterfaces.push_back(NewMapInterface);
            //// создаем новый слой
            TLayer* Layer = new TMapLayer(NewMapInterface->pluginName(), 
                                          new TMapAdapterInterfaceWrapper(NewMapInterface, this));
            Layer->setZoom(currentZoom());
            Layer->setVisible(false);        // делаем слой невидимым
            addLayer(Layer);
            //// сохраняем в список
            w_TransparentMapLayers.insert(it.key(), Layer);
            }
         }
      /// слой без карт для рисования
      if(w_EmptyMapInterface) {
         //// создаем новый объект интерфейса
         IMapAdapterInterface* GeometryInterface = qobject_cast<IMapAdapterInterface*>(w_EmptyMapInterface->createInstance());
         m_EmptyMapInterfaces.push_back(GeometryInterface);
         //// создаем новый слой
         w_GeometryLayer = new TGeometryLayer("Geometries Layer", new TMapAdapterInterfaceWrapper(GeometryInterface, this));
         w_GeometryLayer->setZoom(currentZoom());
         w_GeometryLayer->setVisible(true);      // делаем слой видимым
         addLayer(w_GeometryLayer);
         }
      //
      setInterfaceParameters();
      /// ловим события изменения вида карты
      QNM_DEBUG_CHECK(connect(this, SIGNAL(viewChanged(const QPointF&, int)), SLOT(changeView(const QPointF&, int))));
      }
   else assert(!"Empty the list of map interfaces!");
   //
   m_AtomicLoadedWidgetsCount.fetchAndAddOrdered(1);
   //
   TImageManager::instance()->setLoadTiles(true);
   update();
   }
                                               
   //--------------------------------------------------------------------------
   bool TMapWidget::addGeometryLayer(const QString& LayerName_)
   {
		if(LayerName_.isEmpty())                      return false;	// name is empty
		if(layerManager()->layer(LayerName_) != NULL) return false;	// layer with this name exist

		// make new interface object
		IMapAdapterInterface* Interface = qobject_cast<IMapAdapterInterface*>(w_EmptyMapInterface->createInstance());
		// make new layer
		TLayer* Layer = new TGeometryLayer(LayerName_, new TMapAdapterInterfaceWrapper(Interface, this));
		if(addLayer(Layer)) {
			m_EmptyMapInterfaces.push_back(Interface);
			Layer->setZoom(currentZoom());
			Layer->setVisible(true);      // set layer visibility
			return true;
		}
		else {
			delete Interface;
			delete Layer;
			return false;
		}
   }
                                               
   //--------------------------------------------------------------------------
   bool TMapWidget::setCurrentGeometryLayer(const QString& LayerName_)
   {
		if(LayerName_.isEmpty()) return false;							      // name is empty
		TLayer* Layer = layerManager()->layer(LayerName_);
		if(Layer == NULL) return false;									      // layer with this name not exist
		if(Layer->layertype() != TLayer::GeometryLayer) return false;	// layer not geometry

		w_GeometryLayer = Layer;
		return true;
   }
                                               
   //--------------------------------------------------------------------------
   void TMapWidget::addWidgets(void)
   {
   // scale ruler
   w_RulerScaling = new TRulerScaling(this);
   QNM_DEBUG_CHECK(connect(w_RulerScaling, SIGNAL(zoomChanged(int)),   SLOT(zoomChanged(int))));
   QNM_DEBUG_CHECK(connect(w_RulerScaling, SIGNAL(fitToPageClicked()), SLOT(fitToPageAll())));
   QNM_DEBUG_CHECK(connect(layerManager(), SIGNAL(currentZoomChanged(int)), w_RulerScaling, SLOT(slotSetValue(int))));
   // layouts for placement of buttons and the progress bar on the map
   QHBoxLayout* Columns = new QHBoxLayout(this);
   QVBoxLayout* RightColumn = new QVBoxLayout;
   QHBoxLayout* CoordinateLayout = new QHBoxLayout;
   QVBoxLayout* TopRightColumn = new QVBoxLayout;
   QHBoxLayout* ProgressBarLayout = new QHBoxLayout;
   //
   TopRightColumn->setMargin(0);
   RightColumn->setMargin(0);
   Columns->setMargin(0);
   ProgressBarLayout->setMargin(10);
   CoordinateLayout->setMargin(0);
   // display the current coordinate
   w_CurrentCoordinate = new TCurrentCoordinate(this, "QLabel { background-color : olive; color : white; }", this);
   QSpacerItem* CoordinateHSpacer = new QSpacerItem(1, 14, QSizePolicy::Expanding);
   CoordinateLayout->addItem(CoordinateHSpacer);
   CoordinateLayout->addWidget(w_CurrentCoordinate);
   QSpacerItem* VSpacer = new QSpacerItem(200, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
   TopRightColumn->addLayout(CoordinateLayout);
   TopRightColumn->addItem(VSpacer);
   // progress bar, he catches the necessary signals
   TLoadProgressBar* LoadProgressBar = new TLoadProgressBar(this, this);
   LoadProgressBar->setFixedSize(150, 24);
   QSpacerItem* HSpacer = new QSpacerItem(1, 14, QSizePolicy::Expanding);
   ProgressBarLayout->addItem(HSpacer);
   ProgressBarLayout->addWidget(LoadProgressBar);

   // add all the necessary elements for MapControl
   QVBoxLayout* InnerLayout = new QVBoxLayout;
   InnerLayout->setMargin(10);
   InnerLayout->addWidget(w_RulerScaling);
   InnerLayout->addStretch();
   Columns->addLayout(InnerLayout);
   RightColumn->addLayout(TopRightColumn);
   RightColumn->addLayout(ProgressBarLayout);
   Columns->addLayout(RightColumn);
   //setLayout(Columns);
   }

   //--------------------------------------------------------------------------
   TSupportedMaps& TMapWidget::supportedMaps(void)
   {
   if(m_Maps.empty()) initializeMaps();
   return m_Maps;
   }

   //--------------------------------------------------------------------------
   TSupportedMaps& TMapWidget::supportedMaps(
      const IMapAdapterInterfaces& MapInterfaces_, const QString& Organization_, const QString& Application_)
   {
   if(m_Organization.isEmpty())  m_Organization  = Organization_;
   if(m_Application.isEmpty())   m_Application   = Application_;
   if(m_MapInterfaces.isEmpty()) m_MapInterfaces = MapInterfaces_;
   if(m_Maps.empty()) supportedMaps();
   return m_Maps;
   }

   //--------------------------------------------------------------------------
   TSupportedMaps& TMapWidget::supportedRasterMaps(void)
   {
   if(m_Maps.empty() && m_RasterMaps.empty()) supportedMaps();
   return m_RasterMaps;
   }

   //--------------------------------------------------------------------------
   TSupportedMaps& TMapWidget::supportedTransparentMaps(void)
   {
   if(m_Maps.empty() && m_TransparentMaps.empty()) initializeMaps();
   return m_TransparentMaps;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::initializeMaps()
   {
   // интерфейс "пустой" карты
   if(!w_EmptyMapInterface) {
      foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
         if(MapInterface->sourceType() == TMapAdapter::Consts::EmptyMap) {
            w_EmptyMapInterface = MapInterface;
            break;
            }
         }
      }
   // интерфейс растровых карт
   if(!w_RasterMapInterface) {
      foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
         if(MapInterface->sourceType() == TMapAdapter::Consts::RasterMap) {
            w_RasterMapInterface = MapInterface;
            break;
            }
         }
      }
   // растровые карты из реестра
   rasterMapInterfaces();
   if(!m_RasterMapInterfaces.empty() && m_RasterMaps.empty()) {
      initializeRasterMaps();
      }
   // интерфейсы основных непрозрачных карт
   if(m_Maps.empty()) {
      foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
         if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap &&
            !MapInterface->isTransparent()) {
            //
            MapInterface->setMappingStoragePath(m_MapsStorageDirectory, true);
            MapInterface->setStorageType(m_MapsStorageType);
            m_Maps.insert(MapInterface->UUID(), TMapPluginParameters(MapInterface));
            }
         }
      }
   // интерфейсы прозрачных карт
   if(m_TransparentMaps.empty()) {
      foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
         if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap &&
            MapInterface->isTransparent()) {
            //
            MapInterface->setMappingStoragePath(m_MapsStorageDirectory, true);
            MapInterface->setStorageType(m_MapsStorageType);
            // добавляем в список
            m_TransparentMaps.insert(MapInterface->UUID(), TMapPluginParameters(MapInterface));
            }
         }
      }
   }

   //--------------------------------------------------------------------------
   void TMapWidget::initializeRasterMaps(void)
   {
   if(m_RasterMaps.empty()) {
      foreach(IMapAdapterInterface* Interface, m_RasterMapInterfaces) {
         m_RasterMaps.insert(Interface->UUID(), TMapPluginParameters(Interface));
         }
      }
   }

   //--------------------------------------------------------------------------
   void TMapWidget::showMapName(const QString &MapName_)
   {
   // ищем интерфейс плагина в списке основных карт
   for(TSupportedMaps::const_iterator it = m_Maps.begin(); it != m_Maps.end(); ++it) {
      if(it.value().mapName() == MapName_) {
         showMapID(it.value().mapID());
         return;
         }
      }
   // ищем интерфейс плагина в списке растровых карт
   for(TSupportedMaps::const_iterator it = m_RasterMaps.begin(); it != m_RasterMaps.end(); ++it) {
      if(it.value().mapName() == MapName_) {
         showMapID(it.value().mapID());
         return;
         }
      }
   }

   //--------------------------------------------------------------------------
   void TMapWidget::showTransparentMapName(const QString &MapName_)
   {
   showTransparentMapID(transparentMapID(MapName_));
   }

   //--------------------------------------------------------------------------
   IMapAdapterInterface* TMapWidget::findBaseMapInterface(const QString& MapID_)
   {
   IMapAdapterInterface* MapInterface = NULL;
   QString MapID = MapID_;
   TSupportedMaps Maps = TMapWidget::supportedMaps();
   //
   if(!MapID.isEmpty()) {
      // ищем интерфейс плагина в списке основных непрозрачных карт
      for(TSupportedMaps::const_iterator it = Maps.begin(); it != Maps.end() && !MapInterface; ++it) {
         if(it.value().mapID() == MapID) MapInterface = it.value().mapAdapterInterface();
         }
      if(!MapInterface) {
         // MapID не содержит UUID, или мы UUID убрали
         int Index = MapID.indexOf('{');
         if(Index > 0) MapID.truncate(Index);
         for(TSupportedMaps::const_iterator it = Maps.begin(); it != Maps.end() && !MapInterface; ++it) {
            QString CurrentMapID = it.value().mapID();
            CurrentMapID.truncate(CurrentMapID.indexOf('{'));
            if(CurrentMapID == MapID) MapInterface = it.value().mapAdapterInterface();
            }
         }
      //
      if(!MapInterface) {
         // ищем интерфейс плагина в списке растровых карт
         MapID = MapID_;
         TSupportedMaps RasterMaps = TMapWidget::supportedRasterMaps();
         for(TSupportedMaps::const_iterator it = RasterMaps.begin(); it != RasterMaps.end() && !MapInterface; ++it) {
            if(it.value().mapID() == MapID) MapInterface = it.value().mapAdapterInterface();
            }
         if(!MapInterface) {
            // MapID не содержит UUID, или мы UUID убрали
            int Index = MapID.indexOf('{');
            if(Index > 0) MapID.truncate(Index);
            for(TSupportedMaps::const_iterator it = RasterMaps.begin(); it != RasterMaps.end() && !MapInterface; ++it) {
               QString CurrentMapID = it.value().mapID();
               CurrentMapID.truncate(CurrentMapID.indexOf('{'));
               if(CurrentMapID == MapID) MapInterface = it.value().mapAdapterInterface();
               }
            }
         }
      }
   //
   return MapInterface;
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::showMapID(const QString& MapID_)
   {
   bool RetValue = false;
   m_BaseMapID = MapID_;
   int Zoom = -1;
   IMapAdapterInterface* MapInterface = findBaseMapInterface(MapID_);

   if(!MapInterface) {
      // если не нашелся плагин, берем первый в списке основных
      MapInterface = m_Maps.begin().value().mapAdapterInterface();
      }
   else RetValue = true;

   // создаем новый объект интерфейса
   MapInterface = qobject_cast<IMapAdapterInterface*>(MapInterface->createInstance());

   // уничтожаем объекты предыдущей карты
   if(w_BaseMapAdapter) {
      Zoom = w_BaseMapAdapter->currentZoom();
      if(w_BaseMapAdapter->sourceType() == TMapAdapter::Consts::InternetMap) { 
         // прекращаем загрузку предыдущей карты
         TImageManager::instance()->abortLoading(w_BaseMapAdapter);
         }
      delete w_BaseMapAdapter;
      }
   // интерфейс новой карты
   w_BaseMapInterface = MapInterface;
   // обертка адаптера новой карты
   w_BaseMapAdapter = new TMapAdapterInterfaceWrapper(w_BaseMapInterface, this);
   // создаем слой, если его еще не было
   if(!w_BaseMapLayer) {
      w_BaseMapLayer = new TMapLayer("Base Map Layer", w_BaseMapAdapter);
      addLayer(w_BaseMapLayer);
      }
   else w_BaseMapLayer->setMapAdapter(w_BaseMapAdapter);

   if(Zoom >= 0) TParent::setZoom(Zoom, false);
   w_RulerScaling->setInterval(w_BaseMapAdapter->minZoom(), w_BaseMapAdapter->maxZoom());
   w_RulerScaling->setValue(Zoom);
   updateRequestNew();

   // После изменений мы должны сообщить, какая карта сейчас показана. 
   switch(w_BaseMapAdapter->sourceType()) {
   case TMapAdapter::Consts::InternetMap:
      emit showsMapID(m_BaseMapID);
      break;
   case TMapAdapter::Consts::RasterMap:
      emit showsRasterMapID(m_BaseMapID);
      break;
      }
   //
   return RetValue;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::showTransparentMapID(const QString &MapID_)
   {
   // ищем слой карты в списке
   TLayer* Layer = transparentLayerForID(MapID_);
   if(Layer) {
      // переключаем видимость
      Layer->setVisible(!Layer->isVisible());
      // после изменений мы должны сообщить о переключении видимости слоя карты 
      emit changedVisibility(MapID_, Layer->isVisible());
      }
   else assert(!"Неправильный идентификатор карты.");
   }

   //--------------------------------------------------------------------------
   void TMapWidget::hideAllTransparentMaps(void)
   {
   // проходим по всем прозрачным картам
   TLayer* Layer;
   for(TSupportedMaps::const_iterator it = m_TransparentMaps.begin(); it != m_TransparentMaps.end(); ++it) {
      Layer = w_TransparentMapLayers[it.key()];
      // делаем слой невидимым
      if(Layer->isVisible()) Layer->setVisible(false);
      // после изменений мы должны сообщить о переключении видимости слоя карты 
      emit changedVisibility(it.value().mapID(), false);
      }
   }

   //--------------------------------------------------------------------------
   QString TMapWidget::mapID(void) const
   {
   return m_BaseMapID;
   }

   //--------------------------------------------------------------------------
   QString TMapWidget::transparentMapID(const QString &MapName_) const
   {
   QString MapID;
   // ищем интерфейс плагина в списке
   for(TSupportedMaps::const_iterator it = m_TransparentMaps.begin(); it != m_TransparentMaps.end(); ++it) {
      if(it.value().mapName() == MapName_) {
         MapID = it.value().mapID();
         break;
         }
      }
   return MapID;
   }

   //--------------------------------------------------------------------------
   QString TMapWidget::mapName(void) const
   {
   return w_BaseMapInterface->pluginName();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::resizeEvent(QResizeEvent *event)
   {
   TParent::resize(event->size());
   QWidget::resizeEvent(event);
   }

   //----------------------------------------------------------------------
   void TMapWidget::wheelEvent(QWheelEvent *Event_)
   {
   if(Event_->delta() > 0) zoomIn();
   else                    zoomOut();
   w_RulerScaling->setValue(w_BaseMapAdapter->currentZoom());
   Event_->accept();
   }

   // keys events
   //----------------------------------------------------------------------
   void TMapWidget::keyPressEvent(QKeyEvent* Event_)
   {
   TParent::keyPressEvent(Event_);
   }

   //----------------------------------------------------------------------
   void TMapWidget::keyReleaseEvent(QKeyEvent* Event_)
   {
   TParent::keyReleaseEvent(Event_);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::changeView(const QPointF &Coordinate_, int Zoom_)
   {
   m_LastBaseMapID = m_BaseMapID;
   m_LastCenterPoint = Coordinate_;
   m_LastZoom = Zoom_;
   // send signal about changes view parameters
   emit(viewChanged(m_BaseMapID, Zoom_, Coordinate_));
   }

   //--------------------------------------------------------------------------
   void TMapWidget::print(void)
   {
   QPrinter Printer(QPrinter::ScreenResolution);
   setPrinterParameters(Printer);
   QPrintDialog *PrintDialog = new QPrintDialog(&Printer, this);
   PrintDialog->setWindowTitle(QNetMapTranslator::tr("Print visible area" /* Ru: Печать видимой области */));
   if (PrintDialog->exec() == QDialog::Accepted) {
      TParent::print(&Printer, QRect(), QPoint(0, 0), true);
      }
   delete PrintDialog;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printPreview(void)
   {
   TMapPrinter Printer(QPrinter::ScreenResolution);
   setPrinterParameters(Printer);
   QPrintPreviewDialog Preview(&Printer, this);
   QNM_DEBUG_CHECK(connect(&Preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printMap(QPrinter*))));
   Preview.exec();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printMap(QPrinter *Printer_)
   {
   TMapPrinter *Printer = dynamic_cast<TMapPrinter *>(Printer_);
   TParent::print(Printer_, Printer->rect(), QPoint(0, 0), true);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setPrinterParameters( QPrinter& Printer_ ) const
   {
   Printer_.setOrientation(QPrinter::Landscape);
   Printer_.setFullPage(false);
   Printer_.setResolution(150);
   Printer_.setPageMargins(20, 5, 5, 5, QPrinter::Millimeter);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printArea(void)
   {
   saveMouseMode();
   setMouseMode(TMapControl::RubberBand);
   QNM_DEBUG_CHECK(connect(this, SIGNAL(rubberRectSelected(const QRect, int)),
		            this,   SLOT(printAreaSelected(const QRect, int))));
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printAreaSelected(const QRect Rect_, int Zoom_)
   {
   Q_UNUSED(Zoom_);
   disconnect(this, SIGNAL(rubberRectSelected(const QRect, int)),
	           this,   SLOT(printAreaSelected(const QRect, int)));
   restoreMouseMode();

   QPrinter Printer(QPrinter::ScreenResolution);
   setPrinterParameters(Printer);
   QPrintDialog *PrintDialog = new QPrintDialog(&Printer, this);
   PrintDialog->setWindowTitle(QNetMapTranslator::tr("Print selected area" /* Ru: Печать выбранной области */));
   if (PrintDialog->exec() == QDialog::Accepted) {
      TParent::print(&Printer, Rect_, QPoint(0, 0), true);
      }
   delete PrintDialog;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printAreaPreview()
   {
   saveMouseMode();
   setMouseMode(TMapControl::RubberBand);
   QNM_DEBUG_CHECK(connect(this, SIGNAL(rubberRectSelected(const QRect, int)),
		            this,   SLOT(printAreaPreviewSelected(const QRect, int))));
   }

   //--------------------------------------------------------------------------
   void TMapWidget::printAreaPreviewSelected(const QRect Rect_, int Zoom_)
   {
   Q_UNUSED(Zoom_);
   disconnect(this, SIGNAL(rubberRectSelected(const QRect, int)),
	           this,   SLOT(printAreaPreviewSelected(const QRect, int)));
   restoreMouseMode();

   TMapPrinter Printer(QPrinter::ScreenResolution, Rect_);
   setPrinterParameters(Printer);
   QPrintPreviewDialog Preview(&Printer, this);
   QNM_DEBUG_CHECK(connect(&Preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printMap(QPrinter*))));
   Preview.exec();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::chooseLoadingArea( void )
   {
   saveMouseMode();
   setMouseMode(TMapControl::RubberBand);
   QNM_DEBUG_CHECK(connect(this, SIGNAL(rubberRectSelected(const QRect, int)),
		            this,   SLOT(loadMapsDialog(const QRect, int))));
   }

   //--------------------------------------------------------------------------
   void TMapWidget::loadMapsDialog(const QRect Rect_, int Zoom_)
   {
   Q_UNUSED(Zoom_)

   disconnect(this, SIGNAL(rubberRectSelected(const QRect, int)),
		        this,   SLOT(loadMapsDialog(const QRect, int)));
   restoreMouseMode();

   QRect Rect = Rect_.normalized();
   QRect Viewport = layerManager()->viewportInPixeles();
   Rect.moveLeft(Viewport.x() + Rect.x());
   Rect.moveTop(Viewport.y()  + Rect.y());
   QPointF PointF1 = w_BaseMapAdapter->displayToCoordinate(Rect.topLeft());
   QPointF PointF2 = w_BaseMapAdapter->displayToCoordinate(Rect.bottomRight());

   TLoadDialogWizard Dialog(PointF1, PointF2, mapInterfaces(), m_Organization, m_Application, this);
   Dialog.exec();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::rasterMapsDialog(void)
   {
   TRasterMapsDialog Dialog(mapInterfaces(), m_Organization, m_Application);
   QNM_DEBUG_CHECK(connect(Dialog.mapWidget(), SIGNAL(rasterMapsListChanged()), SIGNAL(rasterMapsListChanged())));
   Dialog.exec();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::saveRasterMapInterfaces(void) const
   {
   if(!m_Organization.isEmpty() && !m_Application.isEmpty()) {
      QSettings Settings(m_Organization, m_Application);
      QString TemplatePropertyFilePath = QString(QNetMapConsts::RasterMapFilePathKey) + "_%1";
      QString TemplatePropertyMapName  = QString(QNetMapConsts::RasterMapNameKey)  + "_%1";
      IMapAdapterInterfaces::const_iterator it = m_RasterMapInterfaces.begin();
      int i = 0;
      do {
         if(it != m_RasterMapInterfaces.end()) {
            Settings.setValue(TemplatePropertyFilePath.arg(i), (*it)->rasterFileFullPath());
            Settings.setValue(TemplatePropertyMapName.arg(i),  (*it)->pluginName());
            ++it;
            }
         else {
            Settings.remove(TemplatePropertyFilePath.arg(i));
            Settings.remove(TemplatePropertyMapName.arg(i));
            }
         i++;
         } while((it != m_RasterMapInterfaces.end()) ||
                 (Settings.contains(TemplatePropertyFilePath.arg(i)) &&
                  Settings.contains(TemplatePropertyMapName.arg(i))));
      }
   else assert(!"Имена организации и приложения для реестра должны быть заданы");
   }

   //--------------------------------------------------------------------------
   IMapAdapterInterfaces& TMapWidget::rasterMapInterfaces(void)
   {
   if(m_RasterMapInterfaces.isEmpty()) {
      if(w_RasterMapInterface && w_RasterMapInterface->sourceType() == TMapAdapter::Consts::RasterMap) {
         if(!m_Organization.isEmpty() && !m_Application.isEmpty()) {
            QSettings Settings(m_Organization, m_Application);
            QString TemplatePropertyFilePath = QString(QNetMapConsts::RasterMapFilePathKey) + "_%1";
            QString TemplatePropertyMapName  = QString(QNetMapConsts::RasterMapNameKey)  + "_%1";
            // проходим по всем ключам реестра
            if(Settings.contains(TemplatePropertyFilePath.arg(0)) &&
               Settings.contains(TemplatePropertyMapName.arg(0)))    {
               //
               int i = 0;
               do {
                  QString FilePath = Settings.value(TemplatePropertyFilePath.arg(i)).toString(); 
                  QString MapName  = Settings.value(TemplatePropertyMapName.arg(i)).toString();
                  staticAddRasterMap(FilePath, MapName);
                  } while(Settings.contains(TemplatePropertyFilePath.arg(++i)) &&
                          Settings.contains(TemplatePropertyMapName.arg(i)));
               } 
            }
         else assert(!"Имена организации и приложения для реестра должны быть заданы");
         }
      else assert(!"Интерфейс не задан или не является интерфейсом растровой карты");
      }
   //
   return m_RasterMapInterfaces;
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::addRasterMap(const QString& FilePath_, const QString& MapName_)
   {
   bool Added = staticAddRasterMap(FilePath_, MapName_);
   // сообщаем об изменении списка растровых карт
   if(Added) emit rasterMapsListChanged();
   return Added;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::rasterMapsLinkChanged(void)
   {
   emit rasterMapsListChanged();
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::staticAddRasterMap(const QString& FilePath_, const QString& MapName_)
   {
   bool Added = false;
   QString FilePath = FilePath_.trimmed();
   QString MapName  = MapName_.trimmed();
   //
   if(!FilePath.isEmpty() && !MapName.isEmpty() && QFile(FilePath).exists()) {
      // создаем новый объект интерфейса и задаем ему путь к растровому файлу
      IMapAdapterInterface* NewInterface =
         qobject_cast<IMapAdapterInterface*>(w_RasterMapInterface->createInstance());
      NewInterface->setRasterFileFullPath(FilePath);
      NewInterface->setPluginName(MapName);
      // все получилось?
      if(NewInterface->isInitialized()) {
         m_RasterMapInterfaces.push_back(NewInterface);
         m_RasterMaps.insert(NewInterface->UUID(), TMapPluginParameters(NewInterface));
         Added = true;
         }
      else delete NewInterface;
      }
   return Added;
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::deleteRasterMap(const QString& MapName_)
   {
   bool Deleted = staticDeleteRasterMap(MapName_);
   // сообщаем об изменении списка растровых карт
   if(Deleted) emit rasterMapsListChanged();
   return Deleted;
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::staticDeleteRasterMap(const QString& MapName_)
   {
   bool Deleted = false;
   QString MapName = MapName_.trimmed();
   //
   if(!MapName.isEmpty()) {
      for(TSupportedMaps::iterator it = m_RasterMaps.begin(); it != m_RasterMaps.end(); ++it) {
         TMapPluginParameters& Parameters = *it;
         if(Parameters.mapName() == MapName) {
            IMapAdapterInterface* Interface = Parameters.mapAdapterInterface();
            m_RasterMapInterfaces.removeOne(Interface);
            m_RasterMaps.remove(Parameters.mapID());
            delete Interface;
            Deleted = true;
            break;
            }
         }
      }
   return Deleted;
   }

   //--------------------------------------------------------------------------
   IMapAdapterInterfaces& TMapWidget::mapInterfaces(void)
   {
   return m_MapInterfaces;
   }

   //--------------------------------------------------------------------------
   TLayer* TMapWidget::transparentLayerForID(const QString& MapID_) const
   { 
   TLayer* Layer = NULL;
   for(TSupportedMaps::const_iterator it = m_TransparentMaps.begin(); it != m_TransparentMaps.end(); ++it) {
      if(it.value().mapID() == MapID_) {
         Layer = w_TransparentMapLayers[it.key()];
         break;
         }
      }
   return Layer;
   }

   //--------------------------------------------------------------------------
   TLayer* TMapWidget::transparentLayerForName(const QString& MapName_) const
   { 
   TLayer* Layer = NULL;
   for(TSupportedMaps::const_iterator it = m_TransparentMaps.begin(); 
       it != m_TransparentMaps.end(); ++it) {
      if(it.value().mapName() == MapName_) {
         Layer = w_TransparentMapLayers[it.key()];
         break;
         }
      }
   return Layer;
   }

   //--------------------------------------------------------------------------
   QStringList TMapWidget::visibleTransparentMaps(void) const
   {
   QStringList VisibleTransparentMaps;
   TLayer* Layer = NULL;

   for(TSupportedMaps::const_iterator it = m_TransparentMaps.begin();
       it != m_TransparentMaps.end(); ++it) {
      Layer = w_TransparentMapLayers[it.key()];
      if(Layer && Layer->isVisible()) VisibleTransparentMaps.append(it.value().mapName());
      }

   return VisibleTransparentMaps;
   }

	//--------------------------------------------------------------------------
	void TMapWidget::showCoordinates( void )
	{
	if(w_CurrentCoordinate && m_VisibleCurrentCoordinate) w_CurrentCoordinate->show();
	}

	//--------------------------------------------------------------------------
	void TMapWidget::hideCoordinates( void )
	{
   if(w_CurrentCoordinate) w_CurrentCoordinate->hide();
	}

   //--------------------------------------------------------------------------
   void  TMapWidget::enterEvent(QEvent *Event_)
   {
   Q_UNUSED(Event_)
	showCoordinates();
   }

   //--------------------------------------------------------------------------
   void  TMapWidget::leaveEvent(QEvent *Event_)
   {
   Q_UNUSED(Event_)
	hideCoordinates();
   }

   //--------------------------------------------------------------------------
   void  TMapWidget::setVisibleMapID(const QString& MapID_, bool Visibility_)
   {
   TLayer* Layer = transparentLayerForID(MapID_);
   if(Layer) Layer->setVisible(Visibility_);
   }

   //------------------------------------------------------------------
   void TMapWidget::setInterfaceParameters(void)
   {
   if(!m_MapInterfaces.empty()) {
      // устанавливаем необходимые параметры карт
      foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
         switch(MapInterface->sourceType()) {
         case TMapAdapter::Consts::InternetMap:
            MapInterface->setMappingStoragePath(m_MapsStorageDirectory, true);
            MapInterface->setStorageType(m_MapsStorageType);
            break;
            }
         }
      // добавляем интерфейсы растровых карт на основании данных реестра
      if(w_RasterMapInterface) initializeRasterMaps();
      }
   }

   //------------------------------------------------------------------
   void TMapWidget::setAllMapsStoragePath(const QString& Path_)
   {
   m_MapsStorageDirectory = Path_;
   // устанавливаем путь для всех исходных интерфейсов карт
   foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap) {
         MapInterface->setMappingStoragePath(Path_, true);
         }
      }
   // устанавливаем путь для прозрачных карт
   foreach(IMapAdapterInterface* MapInterface, m_TransparentMapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap) {
         MapInterface->setMappingStoragePath(Path_, true);
         }
      }
   // устанавливаем путь для основной карты
   if(w_BaseMapInterface && w_BaseMapInterface->sourceType() == TMapAdapter::Consts::InternetMap) { 
      // прекращаем загрузку предыдущей карты
      TImageManager::instance()->abortLoading(w_BaseMapAdapter);
      w_BaseMapInterface->setMappingStoragePath(Path_, true);
      }
   //
   updateRequestNew();
   }

   //------------------------------------------------------------------
   void TMapWidget::setAllMapsStorageType(const int Type_)
   {
   m_MapsStorageType = Type_;
   // устанавливаем тип для всех исходных интерфейсов карт
   foreach(IMapAdapterInterface* MapInterface, m_MapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap) {
         MapInterface->setStorageType(Type_);
         }
      }
   // устанавливаем тип для прозрачных карт
   foreach(IMapAdapterInterface* MapInterface, m_TransparentMapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap) {
         MapInterface->setStorageType(Type_);
         }
      }
   // устанавливаем тип для основной карты
   if(w_BaseMapInterface && w_BaseMapInterface->sourceType() == TMapAdapter::Consts::InternetMap) { 
      // прекращаем загрузку предыдущей карты
      TImageManager::instance()->abortLoading(w_BaseMapAdapter);
      w_BaseMapInterface->setStorageType(Type_);
      }
   //
   updateRequestNew();
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setCurrentCoordinateView(int View_)
   {
   if(w_CurrentCoordinate) w_CurrentCoordinate->setView(View_);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setCurrentCoordinateOrder(int Order_)
   {
   if(w_CurrentCoordinate) w_CurrentCoordinate->setOrder(Order_);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setVisibityCurrentCoordinate(bool Visibility_)
   {
      m_VisibleCurrentCoordinate = Visibility_;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setCacheDirectory(const QString& Path_)
   {
   m_CacheDirectory = Path_;
   enablePersistentCache(Path_);
   }

   //--------------------------------------------------------------------------
   int TMapWidget::calculateZoom(const QRectF Rect_, const uint ImageWidth_, const uint ImageHeight_)
   {
   return calculateZoom(findBaseMapInterface(baseMapAdapter()->UUID()), Rect_, ImageWidth_, ImageHeight_);
   }

   //--------------------------------------------------------------------------
   int TMapWidget::calculateZoom(IMapAdapterInterface* MapAdapter_, const QRectF Rect_, 
                                 const uint ImageWidth_, const uint ImageHeight_)
   {
   int LastZoom = MapAdapter_->currentZoom();
   QPointF Point1F = Rect_.topLeft();
   QPointF Point2F = Rect_.bottomRight();
   QPoint Point1, Point2;
   int Zoom = MapAdapter_->maxZoom();
   do {
      MapAdapter_->setZoom(Zoom);
      Point1 = MapAdapter_->coordinateToDisplay(Point1F);
      Point2 = MapAdapter_->coordinateToDisplay(Point2F);
      Zoom--;
      } while((Point2.x() - Point1.x() > ImageWidth_  * 1.5 || 
               Point2.y() - Point1.y() > ImageHeight_ * 1.5) && Zoom >= 0);
   MapAdapter_->setZoom(LastZoom);
   //
   return Zoom;
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::areaImage(const QString &MapProviderID_, const QRectF &Rect_,
                              const int ImageWidth_, const int ImageHeight_,
                              QPixmap &Pixmap_, QString &Description_)
   {
   bool RetCode = false;
   IMapAdapterInterface* MapInterface = findBaseMapInterface(MapProviderID_);

   if(MapInterface) {
      // создаем новый объект интерфейса
      IMapAdapterInterface *NewMapInterface = 
         qobject_cast<IMapAdapterInterface*>(MapInterface->createInstance());
      // интерфейс создан, создаем обертку адаптера карты
      TMapAdapterInterfaceWrapper MapAdapter(NewMapInterface, NULL);
      // вычисляем и устанавливаем нужный масштаб
      int Zoom = calculateZoom(NewMapInterface, Rect_, ImageWidth_, ImageHeight_);
      MapAdapter.setZoom(Zoom);
      //
      QRect RectSelected = QRect(MapAdapter.coordinateToDisplay(Rect_.topLeft()),
                                 MapAdapter.coordinateToDisplay(Rect_.bottomRight())).normalized();
      // приводим в соответствие заданный и выбранный прямоугольники (они должны быть подобными)
      qreal whSelected  = (qreal)RectSelected.width() / RectSelected.height();
      qreal whSpecified = (qreal)ImageWidth_ / ImageHeight_;
      QRect RectNecessary = RectSelected;
      QPoint RectCenter = RectNecessary.center();
      if(whSelected > whSpecified)
         RectNecessary.setHeight((qreal)RectNecessary.width() / whSpecified);
      else if(whSelected < whSpecified)
         RectNecessary.setWidth((qreal)RectNecessary.height() * whSpecified);
      // увеличиваем так, чтобы после масштабирования по границе выделения было 3 пиксела плюсом
      unsigned Delta = (qreal)RectNecessary.width() / ImageWidth_ * 3 + 0.5;
      RectNecessary.setWidth(RectNecessary.width()   + Delta * 2);
      RectNecessary.setHeight(RectNecessary.height() + Delta * 2);
      // возвращаем центр на место
      QRect Rect(RectCenter.x() - RectNecessary.width()  / 2, RectCenter.y() - RectNecessary.height() / 2,
         RectNecessary.width(), RectNecessary.height());
      // новые географические координаты
      QPointF NewPoint1F = MapAdapter.displayToCoordinate(Rect.topLeft());
      QPointF NewPoint2F = MapAdapter.displayToCoordinate(Rect.bottomRight());
      // определяем координаты тайлов
      QPoint Tile1 = MapAdapter.coordinateToTile(NewPoint1F);
      QPoint Tile2 = MapAdapter.coordinateToTile(NewPoint2F);
      QPoint TileLocal1 = MapAdapter.coordinateToTileLocal(NewPoint1F);
      QPoint TileLocal2 = MapAdapter.coordinateToTileLocal(NewPoint2F);
      // получаем тайлы
      int XBegin = Tile1.x(), XEnd = Tile2.x(), XCount = XEnd - XBegin + 1;
      int YBegin = Tile1.y(), YEnd = Tile2.y(), YCount = YEnd - YBegin + 1;
      QHash<unsigned, QPixmap> Pixmaps;
      int LoadingPixmapCount = XCount * YCount;
      bool InternetSourceType = (MapAdapter.storageType() == TImageManager::Consts::InternetOnly ||
         MapAdapter.storageType() == TImageManager::Consts::InternetAndStorage);
      TImageManager* ImageManager = TImageManager::instance();
      bool FirstLoop = true;
      unsigned MaxLoopsCount = sqrt((double)LoadingPixmapCount) * 10;
      do {
         if(!FirstLoop && InternetSourceType) TSleep::msleep(sqrt((double)LoadingPixmapCount) * 100);
         for(int i = XBegin; i <= XEnd; ++i) {
            for(int j = YBegin; j <= YEnd; ++j) {
               unsigned Key = ((i - XBegin) << 16) + j - YBegin;
               if(!Pixmaps.contains(Key)) {
                  QPixmap Pixmap = ImageManager->getImage(&MapAdapter, i, j);
                  if(Pixmap.size().height() > 1) {
                     Pixmaps.insert(Key, Pixmap);
                     --LoadingPixmapCount;
                     }
                  }
               }
            }
         --MaxLoopsCount;
         FirstLoop = false;
         } while(LoadingPixmapCount > 0 && InternetSourceType && MaxLoopsCount > 0);
      //
      if(!LoadingPixmapCount) {
         // составляем картинку из тайлов
         QPainter Painter;
         int TileSizeX = MapAdapter.tileSizeX(), TileSizeY = MapAdapter.tileSizeY();
         QPixmap Pixmap(XCount * TileSizeX, YCount * TileSizeY);
         if(Painter.begin(&Pixmap)) {
            for(int i = 0; i < XCount; ++i) {
               for(int j = 0; j < YCount; ++j) {
                  Painter.drawPixmap(i * TileSizeX, j * TileSizeY, Pixmaps[(i << 16) + j]);
                  }
               }
            Painter.end();
            // вырезаем нужную часть
            QPixmap RealPixmap = Pixmap.copy(
               TileLocal1.x(), TileLocal1.y(), 
               Pixmap.size().width()  - TileLocal1.x() - TileSizeX + TileLocal2.x(),
               Pixmap.size().height() - TileLocal1.y() - TileSizeY + TileLocal2.y());
            // масштабируем рисунок (вписываемся в заданное) и исходный прямоугольник выбора
            QPixmap ScaledPixmap = RealPixmap.scaled(ImageWidth_, ImageHeight_,
               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            // рисуем границы выделенного прямоугольника
            qreal Scale = (qreal)Rect.width() / ImageWidth_;
            RectSelected.setWidth((qreal)RectSelected.width()   / Scale);
            RectSelected.setHeight((qreal)RectSelected.height() / Scale);
            RectSelected.moveCenter(QPoint(ImageWidth_ / 2 - 1, ImageHeight_ / 2 - 1));
            if(Painter.begin(&ScaledPixmap)) {
               Painter.drawRect(RectSelected);
               Painter.end();
               // формируем результат
               Pixmap_ = ScaledPixmap;
               Description_ = MapInterface->description();
               //
               RetCode = true;
               }
            }
         }
      // подчищаем
      delete NewMapInterface;
      }
   //
   return RetCode;
   }

   //--------------------------------------------------------------------------
   void TMapWidget::zoomInClicked(void) 
   { 
   zoomIn(); 
   w_RulerScaling->setValue(w_BaseMapAdapter->currentZoom()); 
   }

   //--------------------------------------------------------------------------
   void TMapWidget::zoomOutClicked(void)
   {
   zoomOut();
   w_RulerScaling->setValue(w_BaseMapAdapter->currentZoom()); 
   }

   //--------------------------------------------------------------------------
   void TMapWidget::fitToPage( const QRectF& RectF_ )
   {
      QRectF Viewport = layerManager()->viewport();
      unsigned i = 1, Zoom = currentZoom();
      while((Viewport.width() * i  < RectF_.width() || Viewport.height() * i < RectF_.height()) && Zoom > 0) {
            i <<= 1;
            --Zoom;
      }
      //
      setZoom(Zoom, false, false);
      layerManager()->setView(RectF_.center());
      w_RulerScaling->setValue(w_BaseMapAdapter->currentZoom()); 
   }

   //--------------------------------------------------------------------------
   void TMapWidget::fitToPageAll( void )
   {
		if(layerManager()->selectedGeometriesCount()) {
			// fit to page only for selected
			TSetGeometries Geometries = layerManager()->selectedGeometries();
			TSetGeometries::const_iterator it = Geometries.begin();
			if(it != Geometries.end()) {
				setZoom(w_BaseMapAdapter->maxZoom(), false, false);
				//
				QRectF BoundingBoxF = (*it)->boundingBoxF();
				++it;
				while(it != Geometries.end()) {
					BoundingBoxF = BoundingBoxF.united((*it)->boundingBoxF());
					++it;
				}
				//
				fitToPage(BoundingBoxF);
			}
		}
		else {
			// fit to page for all
			TGeometryList GeometryList = geometryLayer()->geometries();
			//
			TGeometryList::const_iterator it = GeometryList.begin();
			if(it != GeometryList.end()) {
				setZoom(w_BaseMapAdapter->maxZoom(), false, false);
				//
				QRectF BoundingBoxF = (*it)->boundingBoxF();
				++it;
				while(it != GeometryList.end()) {
					BoundingBoxF = BoundingBoxF.united((*it)->boundingBoxF());
					++it;
				}
				//
				fitToPage(BoundingBoxF);
			}
		}
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setEnabled(bool Enabled_)
   {
   w_CurrentCoordinate->setEnabled(Enabled_);
   if(!Enabled_) w_CurrentCoordinate->setText(QString());
   TParent::setEnabled(Enabled_);
   }

   //--------------------------------------------------------------------------
   void TMapWidget::setVisibleBaseMap(bool Visibility_) 
   { 
      w_BaseMapLayer->setVisible(Visibility_); 
   }

   //--------------------------------------------------------------------------
   bool TMapWidget::isVisibleBaseMap(void) 
   { 
      return w_BaseMapLayer->isVisible(); 
   }

   // -----------------------------------------------------------------------
   QByteArray TMapWidget::getSettings(void) const
   {
   QByteArray ByteArray;
   QDataStream DataStream(&ByteArray, QIODevice::WriteOnly);
   // сначала сохраняем версию
   DataStream << DataStream.version();
   // собираем необходимые настройки в список
   QList<QVariant> ListSettings;
   /// основная карта
   ListSettings.append(mapName());
   /// масштаб
   ListSettings.append(currentZoom());
   /// координаты центра
   ListSettings.append(currentCoordinate());
   /// список видимых прозрачных карт
   ListSettings.append(visibleTransparentMaps());
   // сохраняем список
   DataStream << ListSettings;
   // сбрасывем поток
   DataStream.device()->reset();

   return ByteArray;
   }

   // -----------------------------------------------------------------------
   void TMapWidget::restoreSettings(QByteArray &ByteArray_)
   {
   QDataStream DataStream(&ByteArray_, QIODevice::ReadOnly);

   int StreamVersion;
   DataStream >> StreamVersion;
   // восстанавливаем если версия потока и данных совпадают
   if(StreamVersion == DataStream.version()) {
      // восстанавливаем список
      QList<QVariant> ListSettings;
      DataStream >> ListSettings;
      // из списка восстанавливаем настройки
      QList<QVariant>::const_iterator it = ListSettings.begin();
      /// основная карта
      if((*it).type() != QVariant::String) return;
      showMapName((*it).toString());
      ++it;
      /// масштаб
      if((*it).type() != QVariant::Int) return;
      setZoom((*it).toInt());
      ++it;
      /// координаты центра
      if((*it).type() != QVariant::PointF) return;
      setView((*it).toPointF());
      ++it;
      /// список видимых прозрачных карт
      if((*it).type() != QVariant::StringList) return;
      QStringList VisibleTransparentMaps = (*it).toStringList();
      for(QStringList::const_iterator itm = VisibleTransparentMaps.begin(); 
          itm != VisibleTransparentMaps.end(); ++itm) {
         TLayer* Layer = transparentLayerForName(*itm);
         if(Layer) Layer->setVisible(true);
         }
      }
   }

}
