/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl LayerManager code by Kai Winter
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

#include <QListIterator>

#include "qnetmap_layermanager.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_layer.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_mapcontrol.h"

namespace qnetmap
{
   //----------------------------------------------------------------
   TLayerManager::TLayerManager(TMapControl* MapControl_, const QSize& size)
      : w_MapControl(MapControl_), m_Scroll(QPoint(0,0)), m_Size(size), m_WhileNewScroll(QPoint(0,0)),
        w_FocusedGeometry(NULL), m_MouseMoved(false), m_MouseButtonPressed(false), m_TooltipLinesMaxCount(20),
        m_OffscreenImageAtomicInt(0)
   {
   // accurately calculate?
   m_OffSize = size * 2;
   m_ComposedOffscreenImage  = QPixmap(m_OffSize);
   m_ComposedOffscreenImage2 = QPixmap(m_OffSize);
   m_ZoomImage = QPixmap(size);
   m_ZoomImage.fill(Qt::white);
   m_ScreenMiddle = QPoint(size.width() / 2, size.height() / 2);
   }

   //----------------------------------------------------------------
   TLayerManager::~TLayerManager()
   {
   foreach(TLayer* Layer, m_Layers) delete Layer;
   m_Layers.clear();
   }

   //----------------------------------------------------------------
   QPointF TLayerManager::currentCoordinate() const
   {
   return m_MapMiddle;
   }

   //----------------------------------------------------------------
   QPixmap TLayerManager::getImage() const
   {
   return m_ComposedOffscreenImage;
   }

   //----------------------------------------------------------------
   TLayer* TLayerManager::layer() const
   {
   Q_ASSERT_X(m_Layers.size()>0, "TLayerManager::getLayer()", "No layers were added!");
   return m_Layers.first();
   }

   //----------------------------------------------------------------
   TLayer* TLayerManager::layer(const QString& layername) const
   {
   QListIterator<TLayer*> layerit(m_Layers);
   while(layerit.hasNext()) {
      TLayer* l = layerit.next();
      if(l->layername() == layername) return l;
      }
   return NULL;
   }

   //----------------------------------------------------------------
   QList<QString> TLayerManager::layerNames() const
   {
   QList<QString> keys;
   QListIterator<TLayer*> layerit(m_Layers);
   while(layerit.hasNext()) keys.append(layerit.next()->layername());
   return keys;
   }

   //----------------------------------------------------------------
   const TLayers& TLayerManager::layers()
   {
      return m_Layers;
   }

   //----------------------------------------------------------------
   void TLayerManager::changeMiddlePoint(const QPoint& Point_)
   {
   TLock Lock(m_MiddlePointAtomicInt);
   //
   m_Scroll += Point_;
   m_ZoomImageScroll += Point_;
   m_MapMiddle_px += Point_;
   m_MapMiddle = layer()->mapadapter()->displayToCoordinate(m_MapMiddle_px);
   foreach(TLayer* Layer, m_Layers) Layer->changeMiddlePoint(m_MapMiddle_px);
   }

   //----------------------------------------------------------------
   void TLayerManager::scrollView( const QPoint& Point_ )
   {
   changeMiddlePoint(Point_);

   if(!checkOffscreen()) newOffscreenImage();
   else                  moveWidgets();
   w_MapControl->update();
   }

   //----------------------------------------------------------------
   void TLayerManager::scrollOffscreenImage(const QPoint& point)
   {
   TLock OffscreenImageLock(m_OffscreenImageAtomicInt, 2);
   //
   m_ComposedOffscreenImage2.fill();
   m_ComposedOffscreenImage2Painter.begin(&m_ComposedOffscreenImage2);
   m_ComposedOffscreenImage2Painter.drawPixmap(-point.x(), -point.y(), m_MousePressedOffscreen);
   m_ComposedOffscreenImage2Painter.end();

   m_ComposedOffscreenImage = m_ComposedOffscreenImage2;
   //
   OffscreenImageLock.unlock();
   //
   w_MapControl->update();
   }

   //----------------------------------------------------------------
   void TLayerManager::moveWidgets()
   {
   QListIterator<TLayer*> it(m_Layers);
   while(it.hasNext())  it.next()->moveWidgets(m_MapMiddle_px);
   }

   //----------------------------------------------------------------
   void TLayerManager::setView(const QPointF& coordinate, const bool MapUpdate_ /*= true*/)
   {
   setMiddlePoint();
   m_MapMiddle = coordinate;
   setMiddlePoint();

   if(MapUpdate_) {
      //TODO: muss wegen moveTo() raus
      if(!checkOffscreen())   newOffscreenImage();
      else {
         //TODO:
         // calculate displacement or more new offscreenImage
         newOffscreenImage();
         }
      w_MapControl->update();
      }
   }

   //----------------------------------------------------------------
   void TLayerManager::setView(const QList<QPointF>& coordinates)
   {
   setMiddle(coordinates);
   w_MapControl->update();
   }

   //----------------------------------------------------------------
//    void TLayerManager::setViewAndZoomIn(const QList<QPointF>& Coordinates_)
//    {
//    const TMapAdapter* MapAdapter = layer()->mapadapter();
//    int MaxZoom = MapAdapter->maxZoom();
//    QPoint OldMiddlePoint = mapmiddle_px();
//    unsigned OldZoom = currentZoom();
// 
//    setMiddle(Coordinates_, false);
//    setZoom(0, false, false);
//    while(containsAll(Coordinates_) && (MapAdapter->currentZoom() < MaxZoom)) zoomIn(false, false);
// 
//    if(!containsAll(Coordinates_)) zoomOut(false, false);
// 
//    if(OldZoom != currentZoom() || OldMiddlePoint != mapmiddle_px()) {
//       emit(currentZoomChanged(currentZoom()));
//       w_MapControl->update();
//       }
//    }

   //----------------------------------------------------------------
   bool TLayerManager::containsAll(const QList<QPointF>& Coordinates_) const
   {
   QRectF ViewPortF = viewport();
   for(QList<QPointF>::const_iterator it = Coordinates_.begin(); it != Coordinates_.end(); ++it) {
      if(!ViewPortF.contains(*it)) return false;
   }
   return true;
   }
   
   //----------------------------------------------------------------
   void TLayerManager::setMiddle( const QList<QPointF>& Coordinates_, const bool MapUpdate_ /*= true*/ )
   {
   qreal X = 0;
   qreal Y = 0;
   unsigned PointsCount = Coordinates_.size();
   if(!PointsCount) return;
   for(QList<QPointF>::const_iterator it = Coordinates_.begin(); it != Coordinates_.end(); ++it) {
      X += (*it).x() / PointsCount;
      Y += (*it).y() / PointsCount;
      }

   setView(QPointF(X, Y), MapUpdate_);
   }

   //----------------------------------------------------------------
   void TLayerManager::setMiddlePoint() 
   {
      m_MapMiddle_px   = layer()->mapadapter()->coordinateToDisplay(m_MapMiddle);
      m_WhileNewScroll = m_MapMiddle_px;
      foreach(TLayer* Layer, m_Layers) Layer->setSize(m_Size, m_MapMiddle_px);
   }

   //----------------------------------------------------------------
   QPoint TLayerManager::mapmiddle_px( void )
   {
   TLock Lock(m_MiddlePointAtomicInt);
   return m_MapMiddle_px;
   }

   //----------------------------------------------------------------
   QRect TLayerManager::viewportInPixeles() const
   {
   QPoint UpperLeft  = QPoint(m_MapMiddle_px.x() - m_ScreenMiddle.x(), m_MapMiddle_px.y() + m_ScreenMiddle.y());
   QPoint LowerRight = QPoint(m_MapMiddle_px.x() + m_ScreenMiddle.x(), m_MapMiddle_px.y() - m_ScreenMiddle.y());

   return QRect(UpperLeft, LowerRight).normalized();
   }

   //----------------------------------------------------------------
   QRectF TLayerManager::viewport() const
   {
   QPoint UpperLeft  = QPoint(m_MapMiddle_px.x() - m_ScreenMiddle.x(), m_MapMiddle_px.y() + m_ScreenMiddle.y());
   QPoint LowerRight = QPoint(m_MapMiddle_px.x() + m_ScreenMiddle.x(), m_MapMiddle_px.y() - m_ScreenMiddle.y());

   QPointF UpperLeftF  = layer()->mapadapter()->displayToCoordinate(UpperLeft);
   QPointF LowerRightF = layer()->mapadapter()->displayToCoordinate(LowerRight);

   return QRectF(UpperLeftF, LowerRightF);
   }

   //----------------------------------------------------------------
   bool TLayerManager::addLayer(TLayer* Layer_)
   {
   if(!Layer_) return false;
   Layer_->setLayerManager(this);
   if(m_Layers.contains(Layer_)) return false;
   for(TLayers::const_iterator it = m_Layers.begin(); it != m_Layers.end(); ++it) {
	   if(Layer_->layername() == (*it)->layername()) return false;
   }

   m_Layers.append(Layer_);

   Layer_->setSize(m_Size, m_MapMiddle_px);

   if(!connect(Layer_, SIGNAL(geometryDeleted(TGeometry*)), SIGNAL(geometryDeleted(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Layer_, SIGNAL(geometryDeleted(TGeometry*)), SLOT(layerGeometryDeleted(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Layer_, SIGNAL(geometryClicked(TGeometry*, QPointF, QMouseEvent*)), 
                       SLOT(layerGeometryClicked(TGeometry*, QPointF, QMouseEvent*)))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(geometryClicked(TGeometry*, QPointF, QMouseEvent*)), 
                       SIGNAL(geometryClicked(TGeometry*, QPointF, QMouseEvent*)))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(updateRequest(QRectF)), SLOT(updateRequest(QRectF)))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(updateRequest()), SLOT(updateRequest()))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(selectedChanged(TGeometry*)), SIGNAL(selectedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(selectedChanged(TGeometry*)), SLOT(geometrySelectedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Layer_, SIGNAL(focusedChanged(TGeometry*)), SIGNAL(focusedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }
   if(!connect(Layer_, SIGNAL(focusedChanged(TGeometry*)), SLOT(geometryFocusedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Layer_, SIGNAL(modifiedChanged(TGeometry*)), SIGNAL(modifiedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Layer_, SIGNAL(modifiedChanged(TGeometry*)), SLOT(geometryModifiedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }

   if(m_Layers.size() == 1) setView(QPointF(0, 0));
   
   return true;
   }

   //----------------------------------------------------------------
   void TLayerManager::layerGeometryClicked(TGeometry* Geometry_, QPointF Point_, QMouseEvent* Event_)
   {
      Q_UNUSED(Point_)
      bool ControlKeyPressed = Event_->button() == Qt::LeftButton && Event_->modifiers() == Qt::ControlModifier;
      //
      if(((Event_->button() == Qt::LeftButton  && Event_->type() == QEvent::MouseButtonPress) || 
          (Event_->button() == Qt::RightButton && Event_->type() == QEvent::MouseButtonRelease) && !mouseMoved())) {
         bool EditMode = mapControl()->editMode();
         // set old focused object to unfocused
         if((EditMode || !Geometry_->isStatic())) {
            if(w_FocusedGeometry && w_FocusedGeometry != Geometry_) w_FocusedGeometry->setFocused(false);
            Geometry_->setFocused(true, false);
            //
            bool CanChangeSelected = EditMode || mapControl()->selectionInViewMode();
            if(CanChangeSelected) {
               bool OldSelected = Geometry_->selected();
	            if(Geometry_->selected()) {
                  if(ControlKeyPressed) Geometry_->setSelected(false, false);
                  else {
                     if((Event_->button() != Qt::RightButton) || !OldSelected) clearSelected();
                     Geometry_->setSelected(true, false);
                  }
	            }
	            else {
                  if(!ControlKeyPressed && ((Event_->button() != Qt::RightButton) || !OldSelected)) clearSelected();
		            Geometry_->setSelected(true, false);
	            }
            }

	         // update geometry on map
            Geometry_->update();
         }
      }
   }

   //----------------------------------------------------------------
   bool TLayerManager::checkOffscreen() const
   {
   // calculate offscreenImage dimension (px)
   QPoint upperLeft  = m_MapMiddle_px - m_ScreenMiddle;
   QPoint lowerRight = m_MapMiddle_px + m_ScreenMiddle;

   QRect testRect = layer()->offscreenViewport();

   return testRect.contains(QRect(upperLeft, lowerRight));
   }

   //----------------------------------------------------------------
   void TLayerManager::newOffscreenImage( bool ClearImage_/*=true*/, bool ShowZoomImage_/*=true*/ )
   {
   m_WhileNewScroll = m_MapMiddle_px;

   TLock OffscreenImageLock(m_OffscreenImageAtomicInt, 2);
   //
   if(ClearImage_) m_ComposedOffscreenImage2.fill(Qt::white);

   QPainter Painter(&m_ComposedOffscreenImage2);
   if(ShowZoomImage_ && layer()->isVisible()) {
      Painter.drawPixmap(m_ScreenMiddle.x() - m_ZoomImageScroll.x(), 
                         m_ScreenMiddle.y() - m_ZoomImageScroll.y(),
                         m_ZoomImage);
      }
   //only draw basemaps
   foreach(TLayer* Layer, m_Layers) {
      if(Layer->isVisible() && Layer->layertype() == TLayer::MapLayer) {
         Layer->drawImage(&Painter, m_WhileNewScroll);
         }
      }

   m_ComposedOffscreenImage = m_ComposedOffscreenImage2;
   //
   m_Scroll = m_MapMiddle_px - m_WhileNewScroll;
   }

   //----------------------------------------------------------------
   QPixmap TLayerManager::getMapFragment(const QPointF& MiddlePoint_, const QSize& Size_, const int Zoom_)
   {
      int OldZoom = currentZoom();
      setZoom(Zoom_, false);
      TLock ZoomLock(m_ZoomAtomicInt);
      //
      QPoint MiddlePoint = layer()->mapadapter()->coordinateToDisplay(MiddlePoint_);
      QRect ViewPort(QPoint(MiddlePoint.x() - Size_.width() / 2, MiddlePoint.y() - Size_.height() / 2), Size_);
      QPixmap Pixmap(Size_);
      QPainter Painter(&Pixmap);
      //only draw basemap layers
      foreach(TLayer* Layer, m_Layers) {
         if(Layer->isVisible() && Layer->layertype() == TLayer::MapLayer) {
            Layer->drawImage(&Painter, MiddlePoint, &ViewPort);
         }
      }
      //only draw geometry layers
      foreach(TLayer* Layer, m_Layers) {
         if(Layer->isVisible() && Layer->layertype() == TLayer::GeometryLayer) {
            Layer->drawImage(&Painter, MiddlePoint, &ViewPort);
         }
      }
      //
      ZoomLock.unlock();
      setZoom(OldZoom, false);

      return Pixmap;
   }

   //----------------------------------------------------------------
   void TLayerManager::setZoom(const int ZoomLevel_, const bool ZoomBackground_, const bool MapUpdate_)
   {
   int current_zoom = layer()->mapadapter()->currentZoom();

   if(current_zoom < 0)                zoom(ZoomLevel_, ZoomBackground_, MapUpdate_);
   else if(ZoomLevel_ != current_zoom) zoom(ZoomLevel_ - current_zoom, ZoomBackground_, MapUpdate_);
   }

   //----------------------------------------------------------------
   void TLayerManager::zoomIn(const bool ZoomBackground_, const bool MapUpdate_)
   {
   zoom(1, ZoomBackground_, MapUpdate_);
   }

   //----------------------------------------------------------------
   void TLayerManager::zoomOut(const bool ZoomBackground_, const bool MapUpdate_)
   {
   zoom(-1, ZoomBackground_, MapUpdate_);
   }

   //----------------------------------------------------------------
   void TLayerManager::zoom(const int DeltaZoom_, const bool ZoomBackground_, const bool MapUpdate_)
   {
      TLock ZoomLock(m_ZoomAtomicInt);
      //
      if(DeltaZoom_ != 0)  {
         int CurrentZoom = layer()->mapadapter()->currentZoom();
			int NewZoom;
 			if(CurrentZoom < 0)
 				CurrentZoom = layer()->mapadapter()->minZoom();
 			NewZoom = CurrentZoom + DeltaZoom_;
			if(NewZoom < layer()->mapadapter()->minZoom() || 
				NewZoom > layer()->mapadapter()->maxZoom()) return;

         TImageManager::instance()->abortLoading();

         if(MapUpdate_) {
            m_ZoomImageScroll = QPoint(0,0);
            m_ZoomImage.fill(Qt::white);
         }

         if(MapUpdate_ && CurrentZoom >= 0 && ZoomBackground_ && DeltaZoom_ < 5) {
            QPixmap tmpImg = m_ComposedOffscreenImage.copy(m_ScreenMiddle.x() + m_Scroll.x(),
                                                           m_ScreenMiddle.y() + m_Scroll.y(), 
                                                           m_Size.width(), m_Size.height());
            QPainter painter(&m_ZoomImage);
            painter.translate(m_ScreenMiddle);
            qreal Scale = 1 << abs(DeltaZoom_);
            if(DeltaZoom_ > 0) painter.scale(Scale, Scale);
            else               painter.scale(1.0 / Scale, 1.0 / Scale);
            painter.translate(-m_ScreenMiddle);
            painter.drawPixmap(0, 0, tmpImg);
            }
         else if(CurrentZoom < 0) CurrentZoom = 0;

         foreach(TLayer* Layer, m_Layers) Layer->setZoom(NewZoom);
         setMiddlePoint();
         if(MapUpdate_) emit(currentZoomChanged(NewZoom));
      }
      if(MapUpdate_) {
         newOffscreenImage();
         w_MapControl->update();
      }
   }

   //----------------------------------------------------------------
   TGeometry* TLayerManager::mouseEvent(QMouseEvent* Event_)
   {
   TLock ToolTipLock(m_ToolTipAtomicInt);
   //
   if(Event_->type() == QEvent::MouseButtonPress) {
      m_MousePressedOffscreen = m_ComposedOffscreenImage;
      m_MouseButtonPressed = true;
   }
   if(Event_->type() == QEvent::MouseButtonRelease) {
      m_MouseButtonPressed = false;
   }
   //
   if(Event_->type() == QEvent::MouseMove) m_MouseMoved = true;
   else                                    m_MouseMoved = false;
   //
   clearToolTipText();
   QToolTip::hideText();
   // Go through the list of items in reverse order for the z-order
   TGeometry* GeometryClicked = NULL;
   QListIterator<TLayer*> it(m_Layers);
   it.toBack();
   // check click to geometry and send mouse event to it
   TLayer* Layer;
   while(it.hasPrevious()) {
      Layer = it.previous();
      if(Layer->isVisible()) {
         GeometryClicked = Layer->mouseEvent(Event_, this);
         if(GeometryClicked) break;
	      }
      }
   // clear selected state for all geometries
   if(!GeometryClicked && 
      Event_->button() == Qt::LeftButton && 
      Event_->type() == QEvent::MouseButtonRelease &&
      !m_MouseMoved) {
      //
      if(!(Event_->modifiers() == Qt::ControlModifier)) clearSelected();
      }
   //
   if(GeometryClicked) {
      if(Event_->button() == Qt::LeftButton || Event_->button() == Qt::RightButton) {
         QPointF Point = Layer->mapadapter()->displayToCoordinate(
            QPoint(Event_->x() - m_ScreenMiddle.x() + m_MapMiddle_px.x(),
                   Event_->y() - m_ScreenMiddle.y() + m_MapMiddle_px.y()));
         GeometryClicked->clicked(Point, Event_);
      }
   }
   else if(Event_->button() == Qt::RightButton && Event_->type() == QEvent::MouseButtonRelease) {
      // send message about clicked in map
      QPointF Point = Layer->mapadapter()->displayToCoordinate(
         QPoint(Event_->x() - m_ScreenMiddle.x() + m_MapMiddle_px.x(),
                Event_->y() - m_ScreenMiddle.y() + m_MapMiddle_px.y()));
      emit(geometryClicked(NULL, Point, Event_));
      }

   // set the tooltip position
   if(!m_TooltipText.isEmpty()) m_ToolTipGlobalPos = w_MapControl->mapToGlobal(Event_->pos());
   //
   return GeometryClicked;
   }

   //----------------------------------------------------------------
   void TLayerManager::addToolTipText( const QString& Text_, const QRgb Color_ /*= QRgb(0x0)*/ )
   {
   static unsigned addsCount;
   //
   m_GeometriesUnderMouseCount++;
   //
   if(Text_.isEmpty()) return;
   //
   if(!m_TooltipText.isEmpty()) {
      m_TooltipText += "<br>";
      addsCount++;
      }
   else addsCount = 1;
   //
   if(addsCount < tooltipLinesMaxCount()) {
      m_TooltipText += QString("<font color=\"#%1%2%3\">%4</font>")
         .arg(qRed(Color_),   2, 16, QLatin1Char('0'))
         .arg(qGreen(Color_), 2, 16, QLatin1Char('0'))
         .arg(qBlue(Color_),  2, 16, QLatin1Char('0'))
         .arg(Text_);
   }
   else m_TooltipText = QNetMapTranslator::tr("%1 elements" /* Ru: %1 элементов */).arg(addsCount);
   }

   //----------------------------------------------------------------
   QString TLayerManager::toolTipText(void)
   {
   QString Text;
   if(m_ToolTipAtomicInt.testAndSetAcquire(0, 1)) {
      Text = m_TooltipText;
      m_ToolTipAtomicInt.fetchAndStoreAcquire(0);
      }
   return Text;
   }

   //----------------------------------------------------------------
   void TLayerManager::updateRequest(const QRectF& RectF_)
   {
   if(viewport().intersects(RectF_)) {
	   TMapControl* MapControl = layer()->mapadapter()->mapControl();
	   QPoint Point1 = MapControl->worldToDisplayCoordinate(RectF_.topLeft());
	   QPoint Point2 = MapControl->worldToDisplayCoordinate(RectF_.bottomRight());
	   w_MapControl->updateRequest(QRect(Point1, Point2));
      }
   }

   //----------------------------------------------------------------
   void TLayerManager::updateRequest()
   {
   newOffscreenImage();
   w_MapControl->update();
   }

   //----------------------------------------------------------------
   void TLayerManager::forceRedraw()
   {
   newOffscreenImage();
   w_MapControl->update();
   }

   //----------------------------------------------------------------
   void TLayerManager::removeZoomImage()
   {
   m_ZoomImage.fill(Qt::white);
   forceRedraw();
   }

   //----------------------------------------------------------------
   void TLayerManager::drawGeometries(QPainter* painter)
   {
   foreach(TLayer* Layer, m_Layers) {
      if(Layer->layertype() == TLayer::GeometryLayer && Layer->isVisible()) {
         Layer->drawGeometries(painter, m_MapMiddle_px, layer()->offscreenViewport());
         }
      }
   }

   //----------------------------------------------------------------
   void TLayerManager::drawImage(QPainter* painter)
   {
   painter->drawPixmap(-m_Scroll.x() - m_ScreenMiddle.x(), 
                       -m_Scroll.y() - m_ScreenMiddle.y(), 
                       m_ComposedOffscreenImage);
   }

   //----------------------------------------------------------------
   int TLayerManager::currentZoom() const
   {
   return layer()->mapadapter()->currentZoom();
   }

   //----------------------------------------------------------------
   void TLayerManager::resize(QSize newSize)
   {
   m_Size    = newSize;
   m_OffSize = newSize * 2;
   m_ComposedOffscreenImage  = QPixmap(m_OffSize);
   m_ComposedOffscreenImage2 = QPixmap(m_OffSize);
   m_ZoomImage = QPixmap(newSize);
   m_ZoomImage.fill(Qt::white);

   m_ScreenMiddle = QPoint(newSize.width()/2, newSize.height()/2);

   foreach(TLayer* Layer, m_Layers) Layer->setSize(newSize, m_MapMiddle_px);

   newOffscreenImage();
   w_MapControl->update();
   }

   // -----------------------------------------------------------------------
   TGeometry* TLayerManager::selected(const unsigned Index_) 
   {
	   if(Index_ >= (unsigned)m_SelectedGeometries.size()) return NULL;
	   TSetGeometries::const_iterator it = m_SelectedGeometries.begin();
	   it += Index_;
	   return *it;
   }

   // -----------------------------------------------------------------------
   unsigned TLayerManager::setSelected(const QRectF& RectF_) 
   {
   unsigned SelectedCount = 0;
   foreach(TLayer* Layer, m_Layers) {
      if(Layer->layertype() == TLayer::GeometryLayer && Layer->isVisible()) {
         TGeometryList const& Geometries = Layer->geometries();
         foreach(TGeometry* Geometry, Geometries) {
            if(Geometry->touches(RectF_)) {
               SelectedCount++;
               if(!Geometry->selected()) setSelected(Geometry);
            }
         }
      }
   }
   return SelectedCount;
   }

   // -----------------------------------------------------------------------
   void TLayerManager::geometryModifiedChanged(TGeometry*)
   {
      w_MapControl->setModified();
   }

   //----------------------------------------------------------------
   void TLayerManager::clearSelected(void) 
   {
      foreach(TGeometry* Geometry, m_SelectedGeometries) Geometry->setSelected(false);
      m_SelectedGeometries.clear(); 
   }

   //----------------------------------------------------------------
   unsigned TLayerManager::selectAll(void)
   {
      foreach(TLayer* Layer, m_Layers) {
         if(Layer->layertype() == TLayer::GeometryLayer && Layer->isVisible()) {
            TGeometryList const& Geometries = Layer->geometries();
            foreach(TGeometry* Geometry, Geometries) {
               if(!Geometry->selected()) setSelected(Geometry);
            }
         }
      }
      return m_SelectedGeometries.size();
   }

   // -----------------------------------------------------------------------
   void TLayerManager::setSelected(TGeometry* Geometry_)
   {
      if(Geometry_ && !Geometry_->selected()) {
         Geometry_->setSelected(true);
      }
   }

   // -----------------------------------------------------------------------
   void TLayerManager::setFocused(TGeometry* Geometry_)
   {
      if(Geometry_) Geometry_->setFocused(true);
      else if(w_FocusedGeometry) w_FocusedGeometry->setFocused(false);
   }

   // -----------------------------------------------------------------------
   void TLayerManager::geometrySelectedChanged(TGeometry* Geometry_)
   {
      if(Geometry_->selected()) m_SelectedGeometries.insert(Geometry_);
      else                      m_SelectedGeometries.remove(Geometry_);
   }

   // -----------------------------------------------------------------------
   void TLayerManager::geometryFocusedChanged(TGeometry* Geometry_)
   {
      if(Geometry_) {
         if(Geometry_ == w_FocusedGeometry) {
            w_FocusedGeometry = NULL;
         }
         else {
            if(w_FocusedGeometry) w_FocusedGeometry->setFocused(false);
            w_FocusedGeometry = Geometry_;
         }
      }
   }

   // -----------------------------------------------------------------------
   void TLayerManager::layerGeometryDeleted( TGeometry* Geometry_ )
   {
      if(Geometry_) {
         if(Geometry_ == w_FocusedGeometry)  w_FocusedGeometry = NULL;
         removeSelected(Geometry_);
      }
   }

}

