 /*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
*
* Based on QMapControl MapControl code by Kai Winter
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
#include "qnetmap_mapcontrol.h"
#include "qnetmap_universaltilemapxmlhandler.h"
#include "qnetmap_layer.h"
#include "qnetmap_imagepoint.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_mapnetwork.h"
#include "qnetmap_layermanager.h"
#include "qnetmap_currentcoordinate.h"

// полученный с помощью программы tohex.exe код с png-изображением точки выбора
#include "../images/qnetmap_pointmarker.hex"

namespace qnetmap
{
   QList<double> TMapControl::m_DistanceList;   
   QAtomicInt TMapControl::m_AtomicLoadedMapControlsCount = 0;

   //----------------------------------------------------------------------
   TMapControl::TMapControl(QSize size, MouseMode mousemode, 
                            bool showScale, bool showCrosshairs,
                            TSelectedArea* Area_,
                            QWidget *Parent_)
      : QWidget(Parent_),
        m_RequestFinished(false),
        m_Size(size),
        w_RubberBand(NULL),
        w_DraggingRubberBand(NULL),
        m_OldMouseMode(mousemode),
        m_ScaleVisible(showScale),
        m_CrosshairsVisible(showCrosshairs),
        m_AbortLoading(false),
        w_PointMarkerImage(NULL),
        m_SelectionIsShown(Area_ == NULL || !Area_->toShow() || Area_->points().empty()),
        m_SelectArea(false),
        m_Modified(false),
        m_EditMode(false),
        w_GeometryClicked(NULL),
        w_OldGeometryClicked(NULL),
        m_MouseMovedBetweenPressRelease(false),
        m_SelectionInViewMode(true)
   {
   // статический член
   m_DistanceList<<5000000<<5000000<<2000000<<1000000<<1000000<<500000<<200000<<100000
                 <<50000<<20000<<10000<<5000<<2000<<1000<<1000<<500<<200<<100<<50<<20<<10<<5;

   if(Area_) m_SelectedArea = *Area_;
   //
   setMouseMode(mousemode);
   m_PointMarker.loadFromData(qnetmap_pointmarker, sizeof(qnetmap_pointmarker));

   w_LayerManager = new TLayerManager(this, size);
   m_ScreenMiddle = QPoint(size.width()/2, size.height()/2);

   m_MouseLeftButtonPressed = false;

   QNM_DEBUG_CHECK(connect(TImageManager::instance(), SIGNAL(imageReceived()),   this, SLOT(updateRequestNew())));
   QNM_DEBUG_CHECK(connect(TImageManager::instance(), SIGNAL(loadingFinished()), this, SLOT(loadingFinished())));

   setMouseTracking(true);

   // таймер, по сигналам которого отрисовывается карта, если были выполнены запросы
   startTimer(500);
   //
   m_AtomicLoadedMapControlsCount.fetchAndAddOrdered(1);
   }

   //----------------------------------------------------------------------
   TMapControl::~TMapControl()
   {
   m_AtomicLoadedMapControlsCount.fetchAndAddOrdered(-1);
   //
   deleteLayerManager();
   deleteRubberBand();
   //
   if(m_AtomicLoadedMapControlsCount == 0) {
      delete TImageManager::instance();
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::deleteLayerManager(void)
   {
      if(w_LayerManager) {
         TImageManager::instance()->abortLoading();
         delete w_LayerManager;
         w_LayerManager = NULL;
      }
   }

   //----------------------------------------------------------------------
   TLayer* TMapControl::layer(const QString& layername) const
   {
   return w_LayerManager->layer(layername);
   }

   //----------------------------------------------------------------------
   QList<QString> TMapControl::layerNames() const
   {
   return w_LayerManager->layerNames();
   }

   //----------------------------------------------------------------------
   int TMapControl::numberOfLayers() const
   {
   return w_LayerManager->layers().size();
   }

   //----------------------------------------------------------------------
   void TMapControl::followGeometry(const TGeometry* Geometry_) const
   {
   QNM_DEBUG_CHECK(connect(Geometry_, SIGNAL(positionChanged(TGeometry*)), SLOT(positionChanged(TGeometry*))));
   }

   //----------------------------------------------------------------------
   void TMapControl::stopFollowing(TGeometry* Geometry_)
   {
   Geometry_->disconnect(SIGNAL(positionChanged(TGeometry*)));
   }

   //----------------------------------------------------------------------
   void TMapControl::positionChanged(TGeometry* Geometry_)
   {
   QPoint start = w_LayerManager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
   QPoint dest  = w_LayerManager->layer()->mapadapter()->coordinateToDisplay(Geometry_->coordinate());

   QPoint step = (dest - start);

   w_LayerManager->scrollView(step);

   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::moveTo(QPointF coordinate)
   {
   m_Target = coordinate;
   m_Steps = 25;
   if(m_MoveMutex.tryLock()) QTimer::singleShot(40, this, SLOT(tick()));
   else {
      // stopMove(coordinate);
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::tick()
   {
   QPoint start = w_LayerManager->layer()->mapadapter()->coordinateToDisplay(currentCoordinate());
   QPoint dest  = w_LayerManager->layer()->mapadapter()->coordinateToDisplay(m_Target);

   QPoint step  = (dest - start) / m_Steps;
   //QPointF next = currentCoordinate() - step;

   w_LayerManager->scrollView(step);

   update();
   m_Steps--;
   if(m_Steps > 0) QTimer::singleShot(40, this, SLOT(tick()));
   else            m_MoveMutex.unlock();
   }

   //----------------------------------------------------------------------
   QPointF TMapControl::currentCoordinate() const
   {
   return w_LayerManager->currentCoordinate();
   }

   //----------------------------------------------------------------------
   QPointF TMapControl::displayToWorldCoordinate(const QPoint Point_)
   {
   return w_LayerManager->layer()->mapadapter()->displayToCoordinate(displayToImage(Point_));
   }

   //----------------------------------------------------------------------
   QRectF TMapControl::displayToWorldCoordinate(const QRect Rect_)
   {
   TMapAdapter* MapAdapter = const_cast<TMapAdapter*>(w_LayerManager->layer()->mapadapter());
   QRect Rect = Rect_.normalized();
   return QRectF(MapAdapter->displayToCoordinate(displayToImage(Rect.topLeft())), 
                 MapAdapter->displayToCoordinate(displayToImage(Rect.bottomRight())));
   }

   //----------------------------------------------------------------------
   QPoint TMapControl::worldToDisplayCoordinate(const QPointF PointF_)
   {
   TMapAdapter* MapAdapter = const_cast<TMapAdapter*>(w_LayerManager->layer()->mapadapter());
   return imageToDisplay(MapAdapter->coordinateToDisplay(PointF_));
   }

   //----------------------------------------------------------------------
   QRect TMapControl::worldToDisplayCoordinate(const QRectF Rect_)
   {
   TMapAdapter* MapAdapter = const_cast<TMapAdapter*>(w_LayerManager->layer()->mapadapter());
   return QRect(imageToDisplay(MapAdapter->coordinateToDisplay(Rect_.topLeft())),
                imageToDisplay(MapAdapter->coordinateToDisplay(Rect_.bottomRight())));
   }

   //----------------------------------------------------------------------
   QPoint TMapControl::displayToImage(const QPoint Point_)
   {
   return QPoint(Point_.x() - m_ScreenMiddle.x() + w_LayerManager->mapmiddle_px().x(),
                 Point_.y() - m_ScreenMiddle.y() + w_LayerManager->mapmiddle_px().y());
   }

   //----------------------------------------------------------------------
   QPoint TMapControl::imageToDisplay(const QPoint Point_)
   {
   return QPoint(Point_.x() + m_ScreenMiddle.x() - w_LayerManager->mapmiddle_px().x(),
                 Point_.y() + m_ScreenMiddle.y() - w_LayerManager->mapmiddle_px().y());
   }

   //----------------------------------------------------------------------
   void TMapControl::update(void)
   {
      resizeRubberBand();
      updateRequestNew();
      TParent::update();
   }

   //----------------------------------------------------------------------
   void TMapControl::updateRequest(const QRect rect)
   {
   TParent::update(rect.normalized());
   }

   //----------------------------------------------------------------------
   void TMapControl::updateRequestNew()
   {
   // устанавливаем признак неотрисованных тайлов
   m_RequestFinished = true;
   }

   //----------------------------------------------------------------------
   void TMapControl::forceUpdate(void)
   {
   // обновляем карту
   w_LayerManager->forceRedraw();
   // сбрасываем признак неотрисованных тайлов
   m_RequestFinished = false;
   }

   //----------------------------------------------------------------------
   void TMapControl::zoomIn()
   {
   w_LayerManager->zoomIn();
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::zoomOut()
   {
   w_LayerManager->zoomOut();
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::setZoom( const int zoomlevel, const bool ZoomBackground_ /*= true*/, const bool MapUpdate_ /*= true*/ )
   {
   w_LayerManager->setZoom(zoomlevel, ZoomBackground_, MapUpdate_);
   resizeRubberBand();
   update();
   }

   //----------------------------------------------------------------------
   int TMapControl::currentZoom() const
   {
   return w_LayerManager->currentZoom();
   }

   //----------------------------------------------------------------------
   void TMapControl::scrollLeft(const int pixel)
   {
   w_LayerManager->scrollView(QPoint(-pixel, 0));
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::scrollRight(const int pixel)
   {
   w_LayerManager->scrollView(QPoint(pixel, 0));
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::scrollUp(const int pixel)
   {
   w_LayerManager->scrollView(QPoint(0, -pixel));
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::scrollDown(const int pixel)
   {
   w_LayerManager->scrollView(QPoint(0, pixel));
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::scroll(const QPoint scroll)
   {
   w_LayerManager->scrollView(scroll);
   update();
   }

   //----------------------------------------------------------------------
   void TMapControl::setView(const QPointF& coordinate) const
   {
   w_LayerManager->setView(coordinate);
   }

   //----------------------------------------------------------------------
   void TMapControl::setView(const QList<QPointF> coordinates) const
   {
   w_LayerManager->setView(coordinates);
   }

   //----------------------------------------------------------------------
//    void TMapControl::setViewAndZoomIn(const QList<QPointF> coordinates) const
//    {
//    w_LayerManager->setViewAndZoomIn(coordinates);
//    }

   //----------------------------------------------------------------------
   void TMapControl::setView(const TPoint* point) const
   {
   w_LayerManager->setView(point->coordinate());
   }

   //----------------------------------------------------------------------
   void TMapControl::loadingFinished()
   {
   w_LayerManager->removeZoomImage();
   }

   //----------------------------------------------------------------------
   void TMapControl::setModified( bool Modified_ /*= true*/ )
   {
      if(!Modified_) {
         // сбрасываем признак изменения у всех объектов всех карт
         TLayers const& Layers = w_LayerManager->layers();
         foreach(TLayer* Layer, Layers) {
            TGeometryList const& Geometries = Layer->geometries();
            foreach(TGeometry* Geometry, Geometries) Geometry->setModified(false, false);
         }
      }
      if(m_Modified != Modified_) {
         m_Modified = Modified_; 
         emit(modifiedChanged());
      }
   }
   
   //----------------------------------------------------------------------
   bool TMapControl::addLayer(TLayer* Layer_)
   {
   bool RetValue = w_LayerManager->addLayer(Layer_);
   // Если необходимо показать выбор, сделанный ранее,
   // проверяем, что уровень назван как надо и показываем
   if(!m_SelectionIsShown && layer("Geometries Layer") && m_SelectedArea.isValid()) {
      switch(m_SelectedArea.type()) {
      case PointAreaType:
         // показываем выбранную точку
         m_SelectionIsShown = setPointMarker(m_SelectedArea.points().first(), &m_PointMarker, true);
         break;
      case RubberBandAreaType:
         // показываем выбранный прямоугольник
         QList<QPointF> Points = m_SelectedArea.points();
         if(QApplication::style()->objectName() == "cleanlooks") {
            w_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            }
         else w_RubberBand = new QRubberBand(QRubberBand::Line, this);
         m_RubberBandCoordinate = QRectF(Points.first(), *(Points.begin() + 1));
         resizeRubberBand();
         w_RubberBand->show();
         break;
         }
      }
   return RetValue;
   }

   //----------------------------------------------------------------------
   void TMapControl::setMouseMode(const MouseMode MouseMode_)
   {
   m_OldMouseMode = m_MouseMode;
   m_MouseMode = MouseMode_;
   // устанавливаем нужный вид курсора
   switch(m_MouseMode) {
   case Dragging:
      setCursor(QCursor(Qt::OpenHandCursor));
      break;
   case Panning:
   case RubberBand:
   case SelectArea:
      if(!m_SelectedArea.isValid() || !m_SelectedArea.isReadOnly()) setCursor(QCursor(Qt::CrossCursor));
      break;
   case SelectPoint:
      if(!m_SelectedArea.isValid() || !m_SelectedArea.isReadOnly()) setCursor(QCursor(Qt::PointingHandCursor));
      break;
   default:
      setCursor(QCursor(Qt::ArrowCursor));
      break;
      }
   }
   
   //----------------------------------------------------------------------
   TMapControl::MouseMode TMapControl::mouseMode()
   {
   return m_MouseMode;
   }

   //----------------------------------------------------------------------
   void TMapControl::saveMouseMode(void)
   {
   m_OldMouseMode = m_MouseMode;
   }

   //----------------------------------------------------------------------
   void TMapControl::restoreMouseMode(void)
   {
   setMouseMode(m_OldMouseMode);
   }

   //----------------------------------------------------------------------
   void TMapControl::enablePersistentCache(const QDir& Path_)
   {
   TImageManager::instance()->setCacheDir(Path_);
   }

   //----------------------------------------------------------------------
   void TMapControl::setProxy(const QString& Host_, const int Port_)
   {
   TImageManager::instance()->setProxy(Host_, Port_);
   }

   //----------------------------------------------------------------------
   void TMapControl::unSetProxy( void )
   {
   TImageManager::instance()->unSetProxy();
   }

   //----------------------------------------------------------------------
//    int TMapControl::setProxyList(const QString& FileName_, const unsigned int MSecs_)
//    {
//    return TImageManager::instance()->mapNetwork()->loadProxyList(FileName_, MSecs_);
//    }

   //----------------------------------------------------------------------
   void TMapControl::showScale(const bool Visible_)
   {
   m_ScaleVisible = Visible_;
   }

   //----------------------------------------------------------------------
   void TMapControl::showCrosshairs(const bool Visible_)
   {
   m_CrosshairsVisible = Visible_;
   }

   //----------------------------------------------------------------------
   void TMapControl::resize(const QSize NewSize_)
   {
   TParent::resize(NewSize_);
   m_Size = NewSize_;
   m_ScreenMiddle = QPoint(NewSize_.width() / 2, NewSize_.height() / 2);

   w_LayerManager->resize(NewSize_);
   }

   //----------------------------------------------------------------------
   void TMapControl::timerEvent(QTimerEvent *Event_)
   {
      Q_UNUSED(Event_)
      //
      if(layerManager()->mouseButtonPressed()) return;
      if(TBase::middlePointAtomicInt() == 1) return;
      //
      if(visibleRegion().contains(mapFromGlobal(w_LayerManager->toolTipGlobalPos()))) {
         // обновляем карту, если есть неотрисованные тайлы
         if(m_RequestFinished) forceUpdate();
         // показываем тултип, если надо
         if(!m_MouseLeftButtonPressed) {
            QString ToolTipText = w_LayerManager->toolTipText();
            if(ToolTipText.isEmpty()) QToolTip::hideText();
            else                      QToolTip::showText(w_LayerManager->toolTipGlobalPos(), ToolTipText);
         }
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::wheelEvent(QWheelEvent *Event_)
   {
   if(Event_->delta() > 0) zoomIn();
   else                    zoomOut();
   Event_->accept();
   }

   // mouse events
   //----------------------------------------------------------------------
   void TMapControl::mousePressEvent(QMouseEvent* Event_)
   {
   m_MouseMovedBetweenPressRelease = false;
   //
   if(w_LayerManager->layers().size() > 0) {
      w_GeometryClicked = w_LayerManager->mouseEvent(Event_);
      bool FirstClickToGeometry = w_GeometryClicked && (w_GeometryClicked != w_OldGeometryClicked);
      if(w_GeometryClicked) w_OldGeometryClicked = w_GeometryClicked;
      //
      switch(Event_->button()) {
      case Qt::LeftButton:
         m_MouseLeftButtonPressed = true;
         m_MouseClickPoint = QPoint(Event_->x(), Event_->y());
         m_MouseCurrentPoint = m_MouseClickPoint;
         switch(m_MouseMode) {
         case SelectArea:
            m_SelectArea = true;
         case RubberBand:
            if(!m_SelectedArea.isValid() || !m_SelectedArea.isReadOnly()) {
               deleteRubberBand();
               QRect Rect(m_MouseClickPoint, QSize());
               if(QApplication::style()->objectName() == "cleanlooks") {
                  w_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                  }
               else w_RubberBand = new QRubberBand(QRubberBand::Line, this);
               w_RubberBand->setGeometry(Rect.normalized());
               m_RubberBandCoordinate = displayToWorldCoordinate(Rect.normalized());
               w_RubberBand->show();
               }
            break;
         case Dragging:
            m_OldCursor = cursor();
            if(!(Event_->modifiers() == Qt::ControlModifier)) {
               if(w_GeometryClicked) {
                  if(w_GeometryClicked->geometryType() == "Point" || w_GeometryClicked->geometryType() == "Label") {
                     m_GeometryClickedCoordinateShift = m_MouseClickPoint - worldToDisplayCoordinate(((TPoint*)w_GeometryClicked)->coordinate());
                  }
                  else if(!FirstClickToGeometry && editMode() &&
                          (w_GeometryClicked->geometryType() == "Area" || w_GeometryClicked->geometryType() == "PolyLine")) {
                     QPointF PointF = displayToWorldCoordinate(m_MouseClickPoint);
                     m_GeometryClickedCoordinateShift = QPoint();
                     if(((TPolyLine*)w_GeometryClicked)->touchesPoints(PointF)) {
                        m_PointParameters = ((TPolyLine*)w_GeometryClicked)->touchesPoint(PointF);
                        if(m_PointParameters.PointNumber >= 0) {
                           m_GeometryClickedCoordinateShift = m_MouseClickPoint - worldToDisplayCoordinate(m_PointParameters.Coordinate);
                        }
                     }
                     else if(((TPolyLine*)w_GeometryClicked)->touchesControlPoints(PointF)) {
                        m_PointParameters = ((TPolyLine*)w_GeometryClicked)->touchesControlPoint(PointF);
                        if(m_PointParameters.PointNumber >= 0) {
                           m_GeometryClickedCoordinateShift = m_MouseClickPoint - worldToDisplayCoordinate(m_PointParameters.Coordinate);
                        }
                     }
                  }
                  else m_GeometryClickedCoordinateShift = QPoint();
               }
            }
            else {
               if(!w_GeometryClicked && selectionInViewMode()) {
                  setCursor(QCursor(Qt::CrossCursor));
                  m_DruggingPoint = Event_->pos();
                  if(!w_DraggingRubberBand) {
                     if(QApplication::style()->objectName() == "cleanlooks") {
                        w_DraggingRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                        }
                     else w_DraggingRubberBand = new QRubberBand(QRubberBand::Line, this);
                     if(w_DraggingRubberBand) {
                        w_DraggingRubberBand->setGeometry(QRect(m_DruggingPoint, QSize()));
                        w_DraggingRubberBand->show();
                     }
                  }
               }
            }
            break;
         case SelectPoint:
            if(!m_SelectedArea.isValid() || !m_SelectedArea.isReadOnly()) {
               m_MouseLeftButtonPressed = false;
               // ставим флаг
               QPoint Point = Event_->pos();
               QPointF PointF = displayToWorldCoordinate(Point);
               m_SelectionIsShown = setPointMarker(PointF, &m_PointMarker);
               // вырезаем кусок экрана, если надо
               if(m_SelectedArea.isValid() && m_SelectedArea.toReturnImage()) {
                  TLayer* GeometryLayer = layer("Geometries Layer");
                  bool GeometryLayerVisibility = GeometryLayer->isVisible();
                  GeometryLayer->setVisible(false);
                  QRect Rect(Point.x() - m_SelectedArea.imageWidth()  / 2,
                             Point.y() - m_SelectedArea.imageHeight() / 2,
                             m_SelectedArea.imageWidth(),
                             m_SelectedArea.imageHeight());
                  GeometryLayer->setVisible(GeometryLayerVisibility);
                  // сообщаем о событии
                  emit(mouseSelectPointEvent(Event_, displayToImage(Point), PointF, cutPartOfTheScreen(Rect)));
                  }
               else {
                  // сообщаем о событии
                  emit(mouseSelectPointEvent(Event_, displayToImage(Point), PointF, QPixmap()));
                  }
               }
            break;
         case Panning:
         case None:
            break;
            }
         break;
//       case Qt::RightButton:
//          if(m_MouseMode != None) zoomIn();
//          break;
//       case Qt::MidButton:
//          if(m_MouseMode != None) zoomOut();
//          break;
      default:
          break;
         }
      }

   emit(mouseEventCoordinate(Event_, displayToWorldCoordinate(Event_->pos())));
   Event_->accept();
   }

   //----------------------------------------------------------------------
   void TMapControl::mouseMoveEvent(QMouseEvent* Event_)
   {
      try {
         if(m_MouseMoveAtomicInt.testAndSetAcquire(0, 1)) {
            //
            w_LayerManager->mouseEvent(Event_);
            //
            if(m_MouseLeftButtonPressed) {
               m_MouseMovedBetweenPressRelease = true;
               //
               switch(m_MouseMode) {
	            case Dragging: {
		            QPoint Point(Event_->x(), Event_->y());
                  if(editMode() && w_GeometryClicked) { 
                     if(w_GeometryClicked->geometryType() == "Point" || w_GeometryClicked->geometryType() == "Label") {
                        QPointF PointF = displayToWorldCoordinate(Point - m_GeometryClickedCoordinateShift);
                        ((TGeometry*)w_GeometryClicked)->setCoordinate(PointF);
                        showPointCoordinatesToolTip(Event_->pos(), PointF);
                     }
                     else if(w_GeometryClicked->geometryType() == "Area" || w_GeometryClicked->geometryType() == "PolyLine") {
                        QPointF PointF = displayToWorldCoordinate(Point - m_GeometryClickedCoordinateShift);
						if(m_PointParameters.PointNumber >= 0) {
                        switch(m_PointParameters.PointType) {
							case PolyLineNodePoint:
							   ((TPolyLine*)w_GeometryClicked)->setCoordinate(m_PointParameters.PointNumber, PointF);
							   showPointCoordinatesToolTip(Event_->pos(), PointF);
							   break;
							case PolyLineInputPoint:
							   ((TPolyLine*)w_GeometryClicked)->setTensionContinuityBias(m_PointParameters.PointNumber, PointF, PolyLineInputPoint);
							   break;
							case PolyLineOutputPoint:
							   ((TPolyLine*)w_GeometryClicked)->setTensionContinuityBias(m_PointParameters.PointNumber, PointF, PolyLineOutputPoint);
							   break;
							}
						}
                     }
                  }
                  else if(w_DraggingRubberBand) w_DraggingRubberBand->setGeometry(QRect(m_DruggingPoint, Event_->pos()).normalized());
                  else {
                     w_LayerManager->scrollOffscreenImage(m_MouseClickPoint - Point);
		               if(!m_SelectArea) {
			               w_LayerManager->changeMiddlePoint(m_MouseClickPoint - Point);
			               m_MouseClickPoint = Point;
		                  }
                     // смещаем прямоугольник выбора
                     if(w_RubberBand) {
                        w_RubberBand->hide();
                        QRect Rect = w_RubberBand->geometry();
                        Rect.translate(Event_->x() - m_MouseCurrentPoint.x(), Event_->y() - m_MouseCurrentPoint.y());
                        w_RubberBand->setGeometry(Rect.normalized());
                        }
                     m_MouseCurrentPoint = QPoint(Event_->x(), Event_->y());
                     break;
                     }
		            }
               case Panning:
                  m_MouseCurrentPoint = QPoint(Event_->x(), Event_->y());
                  break;
               case SelectArea:
                  if(m_SelectedArea.isValid() && m_SelectedArea.isReadOnly()) break;
               case RubberBand: {
                  QRect Rect(m_MouseClickPoint, Event_->pos());
                  w_RubberBand->setGeometry(Rect.normalized());
                  m_RubberBandCoordinate = displayToWorldCoordinate(Rect.normalized());
                  break;
                  }
               default:
                  break;
                  }
               update();
               }
            else {
               if(w_LayerManager->geometriesUnderMouseCount()) { 
                  setCursor(QCursor(Qt::ArrowCursor)); 
               }
               else setCursor(QCursor(Qt::OpenHandCursor));
               emit mouseMoveEventCoordinate(this, displayToWorldCoordinate(Event_->pos()));
            }

            m_MouseMoveAtomicInt.fetchAndStoreAcquire(0);
         }
         Event_->accept();
      }
      catch(std::exception& Exception_) {
         QMessageBox(QMessageBox::Critical, "TMapControl MouseMove: ", Exception_.what());
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::mouseReleaseEvent(QMouseEvent* Event_)
   {
      try {
         w_LayerManager->mouseEvent(Event_);
         QToolTip::hideText();

         m_MouseLeftButtonPressed = false;
         QPoint Point(Event_->x(), Event_->y());
         //
         switch(m_MouseMode) {
         case Dragging:
            if(w_GeometryClicked) { 
               if(!m_MouseMovedBetweenPressRelease && 
                  m_PointParameters.PointType == PolyLineNodePoint && 
                  m_PointParameters.PointNumber > 0) {
                  //
                  ((TPolyLine*)w_GeometryClicked)->nextEditState(m_PointParameters.PointNumber);
               }
               w_GeometryClicked = NULL; 
               m_PointParameters.PointType = NoPolyLinePoint;
            }
            else if(w_DraggingRubberBand) {
               QPointF Point1 = displayToWorldCoordinate(m_MouseClickPoint);
               QPointF Point2 = displayToWorldCoordinate(Point);
               w_LayerManager->setSelected(QRectF(Point1, Point2).normalized());
               delete w_DraggingRubberBand;
               w_DraggingRubberBand = NULL;
            }
            else {
               if(w_RubberBand) w_RubberBand->show();
            }
            if(Event_->button() == Qt::LeftButton) setCursor(m_OldCursor);
            if(m_MouseMovedBetweenPressRelease) w_LayerManager->updateRequest();
            break;
         case Panning: {
            QPointF ulCoord = displayToWorldCoordinate(m_MouseClickPoint);
            QPointF lrCoord = displayToWorldCoordinate(Point);

            emit boxDragged(QRectF(ulCoord, QSizeF((lrCoord-ulCoord).x(), (lrCoord-ulCoord).y())));
            break;
            }
         case RubberBand: {
            QRect Rect = QRect(m_MouseClickPoint, Point);
            // shooting mode selection
            w_RubberBand->hide();
            deleteRubberBand();
            // completion signal to select the area
            emit rubberRectSelected(Rect, currentZoom());
            break;
            }
         default:
            break;
         }
         //
         if(m_SelectArea) {
            if(!m_SelectedArea.isValid() || !m_SelectedArea.isReadOnly()) {
               QRect RectSelected(m_MouseClickPoint, Point);
               QRectF RectF = displayToWorldCoordinate(RectSelected);
               // cut a piece of the screen, if necessary
               if(m_SelectedArea.isValid() && m_SelectedArea.toReturnImage()) {
                  // detailed in the respective specified and selected rectangles (they must be similar)
                  qreal whSelected  = (qreal)RectSelected.width() / RectSelected.height();
                  qreal whSpecified = (qreal)m_SelectedArea.imageWidth() / m_SelectedArea.imageHeight();
                  QRect RectNecessary = RectSelected;
                  QPoint RectCenter = RectNecessary.center();
                  if(whSelected > whSpecified)
                     RectNecessary.setHeight((qreal)RectNecessary.width() / whSpecified);
                  else if(whSelected < whSpecified)
                     RectNecessary.setWidth((qreal)RectNecessary.height() * whSpecified);
                  // увеличиваем так, чтобы после масштабирования по границе выделения было 3 пиксела плюсом
                  unsigned Delta = (qreal)RectNecessary.width() / m_SelectedArea.imageWidth() * 3 + 0.5;
                  RectNecessary.setWidth(RectNecessary.width()   + Delta * 2);
                  RectNecessary.setHeight(RectNecessary.height() + Delta * 2);
                  // возвращаем центр на место
                  QRect Rect(RectCenter.x() - RectNecessary.width()  / 2, 
                        RectCenter.y() - RectNecessary.height() / 2,
                        RectNecessary.width(), RectNecessary.height());
                  // cut
                  QPixmap RealPixmap = cutPartOfTheScreen(Rect);
                  // масштабируем рисунок (вписываемся в заданное) и исходный прямоугольник выбора
                  QPixmap Pixmap = RealPixmap.scaled(m_SelectedArea.imageWidth(), m_SelectedArea.imageHeight(),
                     Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                  qreal Scale = (qreal)Rect.width() / m_SelectedArea.imageWidth();
                  RectSelected.setWidth((qreal)RectSelected.width()   / Scale);
                  RectSelected.setHeight((qreal)RectSelected.height() / Scale);
                  RectSelected.moveCenter(QPoint(m_SelectedArea.imageWidth() / 2 - 1, m_SelectedArea.imageHeight() / 2 - 1));
                  // рисуем границы выделенного прямоугольника
                  QPainter Painter;
                  Painter.begin(&Pixmap);
                  Painter.drawRect(RectSelected);
                  Painter.end();
                  // сигнал о завершении выбора области
                  emit rubberRectSelected(RectF, Pixmap);
                  }
               else {
                  // сигнал о завершении выбора области
                  emit rubberRectSelected(RectF, QPixmap());
                  }
               }
            m_SelectArea = false;
            }

         emit(mouseEventCoordinate(Event_, displayToWorldCoordinate(Event_->pos())));
         Event_->accept();
         //
         m_MouseMovedBetweenPressRelease = false;
      }
      catch(std::exception& Exception_) {
         QMessageBox(QMessageBox::Critical, "TMapControl MouseRelease: ", Exception_.what());
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::paintEvent(QPaintEvent* Event_)
   {
   QWidget::paintEvent(Event_);
   QPainter Painter(this);

   drawScreen(&Painter);

   if(m_CrosshairsVisible) {
      Painter.drawLine(m_ScreenMiddle.x(), m_ScreenMiddle.y() - 10, 
                       m_ScreenMiddle.x(), m_ScreenMiddle.y() + 10);
      Painter.drawLine(m_ScreenMiddle.x() - 10, m_ScreenMiddle.y(), 
                       m_ScreenMiddle.x() + 10, m_ScreenMiddle.y());
      }

   if(m_MouseLeftButtonPressed && m_MouseMode == Panning) {
      QRect rect = QRect(m_MouseClickPoint, m_MouseCurrentPoint);
      Painter.drawRect(rect);
      }

   emit viewChanged(currentCoordinate(), currentZoom());
   }

   //----------------------------------------------------------------------
   void TMapControl::drawScreen(QPainter *Painter_) const
   {
	if(currentZoom() < w_LayerManager->layer()->mapadapter()->minZoom()) {
		w_LayerManager->layer()->setZoom(w_LayerManager->layer()->mapadapter()->minZoom());
	}
   w_LayerManager->drawImage(Painter_);
   w_LayerManager->drawGeometries(Painter_);

   // draw scale
	int CurrentZoom = currentZoom();
   if(m_ScaleVisible) {
      double DistanceFromList = m_DistanceList.at(CurrentZoom);
      if(CurrentZoom >= 0 && m_DistanceList.size() > CurrentZoom)  {
         double line;
         line = 2. * DistanceFromList / pow(2.0, 21 - CurrentZoom) / 0.149291;

         // draw the scale
         Painter_->setPen(Qt::black);
         QPoint p1(10, m_Size.height() - 20);
         QPoint p2((int)line + 10, m_Size.height() - 20);
         Painter_->drawLine(p1, p2);

         Painter_->drawLine(10, m_Size.height() - 15, 10, m_Size.height() - 25);
         Painter_->drawLine((int)line + 10, m_Size.height() - 15, (int)line + 10, m_Size.height() - 25);

         QString distance;
         if(DistanceFromList >= 1000)
              distance = QVariant(DistanceFromList/1000).toString() + " km";
         else distance = QVariant(DistanceFromList).toString() + " m";

         Painter_->drawText(QPoint((int)line + 20, m_Size.height() - 15), distance);
         }
      }
   }

   //----------------------------------------------------------------------
   QRect TMapControl::print(QPrinter *Printer_, QRect Rect_, QPoint PointUpLeft_, bool DrawEdging_)
   {
   QRect ScreenRect(QPoint(0, 0), m_Size);
   if(Rect_ != QRect()) ScreenRect = Rect_;

   QPixmap ScreenPixmap(w_LayerManager->ComposedOffscreenImageSize());
   QPainter ScreenPixmapPainter(&ScreenPixmap);
   drawScreen(&ScreenPixmapPainter);
   ScreenPixmapPainter.end();

   int X = PointUpLeft_.x();
   int Y = PointUpLeft_.y();
   QRect Rect(X, Y, ScreenRect.width(), ScreenRect.height());
   QPainter PrinterPainter;
   PrinterPainter.begin(Printer_);
   PrinterPainter.drawPixmap(X, Y, ScreenPixmap, 
                             ScreenRect.x(), ScreenRect.y(), ScreenRect.width(), ScreenRect.height());
   if(DrawEdging_) PrinterPainter.drawRect(Rect);
   PrinterPainter.end();

   return Rect;
   }

   //----------------------------------------------------------------------
   QPixmap TMapControl::cutPartOfTheScreen(QRect Rect_)
   {
   QPixmap ScreenPixmap(w_LayerManager->ComposedOffscreenImageSize());
   QPainter ScreenPixmapPainter(&ScreenPixmap);
   drawScreen(&ScreenPixmapPainter);
   ScreenPixmapPainter.end();
   
   return ScreenPixmap.copy(Rect_);
   }

   //----------------------------------------------------------------------
   void TMapControl::deleteRubberBand(void)
   {
   if(w_RubberBand) {
      delete w_RubberBand; 
      w_RubberBand = NULL; 
      emit rubberRectDeselect();
      }
   }

   //----------------------------------------------------------------------
   void TMapControl::resizeRubberBand(void)
   {
   if(w_RubberBand) {
      w_RubberBand->setGeometry(worldToDisplayCoordinate(m_RubberBandCoordinate));
      }
   }

   //----------------------------------------------------------------------
   bool TMapControl::setPointMarker(const QPointF Point_, QPixmap* PointMarker_, bool NoUpdate_)
   {
   TLayer* GeometryLayer = layer("Geometries Layer");
   if(GeometryLayer) {
      /// если маркер был, то удаляем
      if(w_PointMarkerImage) { 
         GeometryLayer->removeGeometry(w_PointMarkerImage);
         delete w_PointMarkerImage;
      }
      /// создаем новый и устанавливаем
      w_PointMarkerImage = new TImagePoint(Point_, PointMarker_, "", BottomMiddle);
      GeometryLayer->addGeometry(w_PointMarkerImage, NoUpdate_);
      if(!NoUpdate_) updateRequestNew();
      
      return true;
      }
   return false;
   }

   //----------------------------------------------------------------------
   IMapAdapterInterfaces TMapControl::loadPlugins(const QString PluginsPath_,
                                                  const bool MessagePluginsNotFound_)
   {
   IMapAdapterInterfaces MapInterfaces;
   QDir PluginsDir;
   QStringList NamesFilter;
   NamesFilter << "qnetmap_*.dll" << "libqnetmap_*.so";
   //
   if(PluginsPath_.isEmpty()) {
      // ищем в папке программы
      PluginsDir = QDir(QApplication::applicationDirPath());
      if(PluginsDir.entryList(NamesFilter, QDir::Files).size() == 0) {
         // ищем в текущей папке
         PluginsDir = QDir(".");
         }
      }
   else {
      // ищем в указанной папке
      PluginsDir = QDir(PluginsPath_);
      }
   // загружаем
   foreach(QString FileName, PluginsDir.entryList(NamesFilter, QDir::Files)) {
      QString AbsoluteFilePath = PluginsDir.absoluteFilePath(FileName);
      if(!QLibrary::isLibrary(AbsoluteFilePath)) break;
      //
      QPluginLoader loader(AbsoluteFilePath);
      QObject *Plugin = loader.instance();
      if(Plugin) {
         IMapAdapterInterface *MapInterface = qobject_cast<IMapAdapterInterface *>(Plugin);
         MapInterface->setImageManagerInstance(TImageManager::instance());
         MapInterface->setPlugin(Plugin);
         if(MapInterface->sourceType() == TMapAdapter::Consts::UniversalTileMap) {
            // ищем и обрабатываем xml-файл универсального плагина
            QFile XMLFile(PluginsDir.absolutePath() + "/qnetmap.xml");
            if(XMLFile.exists()) {
               TTileMapAdapterParametersList MapAdapterParametersList;
               THttpParametersList& HttpParametersList = TMapNetwork::httpParametersList();
               QXmlInputSource Source(&XMLFile);
               TUniversalTileMapXMLHandler Handler(&MapAdapterParametersList, HttpParametersList);
               QXmlSimpleReader Reader;
               Reader.setContentHandler(&Handler);
               Reader.setErrorHandler(&Handler); 
               if(Reader.parse(Source)) {
                  // инициализируем плагины
                  bool FirstLoop = true;
                  IMapAdapterInterface* Interface;
                  for(TTileMapAdapterParametersList::const_iterator 
                      it = MapAdapterParametersList.begin(); it < MapAdapterParametersList.end(); ++it) {
                     //
                     TTileMapAdapterParameters Parameters = *it;
                     if(Parameters.isValid()) {
                        // используем текущий интерфейс или создаем его копию
                        if(FirstLoop) { Interface = MapInterface; FirstLoop = false; }
                        else Interface = qobject_cast<IMapAdapterInterface*>(MapInterface->createInstance());
                        // устанавливаем параметры карты
                        Interface->setParameters(&Parameters);
                        //
                        MapInterfaces.push_back(Interface);
                        }
                     }
                  }
               else {
                  // ошибка при чтении XML-файла
                  QString Error = Handler.errorString();
                  QMessageBox::critical(NULL, QNetMapTranslator::tr("XML file error" /* Ru: Ошибка в XML-файле */), Error);
                  }
               }
            else {
               loader.unload();
               QMessageBox::critical(NULL, QNetMapTranslator::tr("Error"), 
                  QNetMapTranslator::tr("xml-file \"%1\" was not found" /* Ru: xml-файл "%1" не найден */).arg(XMLFile.fileName()));
               }
            }
         else MapInterfaces.push_back(MapInterface);
         }
      else {
         QMessageBox::critical(
            NULL, QNetMapTranslator::tr("Error loading plugin module" /* Ru: Ошибка загрузки плагина */), loader.errorString());
         }
      }
   //
   if(MapInterfaces.size() == 0 && MessagePluginsNotFound_) {
      // картографические плагины не найдены, ругаемся
      QMessageBox(QMessageBox::Critical, 
         QNetMapTranslator::tr("Error"),
         QNetMapTranslator::tr(
				"The map services plugins was not found in folder \"%1\"."
				/* Ru: В каталоге "%1" не найдены плагины для доступа к картографическим сервисам */)
            .arg(PluginsDir.absolutePath()));
      }
   //
   return MapInterfaces;
   }

   //----------------------------------------------------------------------
   void TMapControl::showPointCoordinatesToolTip( const QPoint MousePoint_, const QPointF Coordinates_ )
   {
      QPoint TopLeftPoint(QPoint(0, 0)), BottomRightPoint(QPoint(size().width(), size().height()));
      QToolTip::showText(QWidget::mapToGlobal(MousePoint_), TCurrentCoordinate::textCoordinates(Coordinates_, this), 
         this, QRect(TopLeftPoint, BottomRightPoint).normalized());
   }

}
