/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl Layer code by Kai Winter
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

#include <QPainter>
#include <QMouseEvent>

#include "qnetmap_layer.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_layermanager.h"
#include "qnetmap_point.h"
#include "qnetmap_geometry.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_mapcontrol.h"

namespace qnetmap
{
   TLayer::TLayer(const QString& layername, TMapAdapter* mapadapter, enum LayerType layertype, bool takeevents)
      : m_IsVisible(true), m_Layername(layername), m_LayerType(layertype), 
        w_MapAdapter(mapadapter), m_TakeEvents(takeevents), m_OffscreenViewport(QRect(0,0,0,0)),
        m_VisibilityGeometriesName(true), w_LayerManager(NULL) {}

   //----------------------------------------------------------------------
   TLayer::~TLayer()
   {
   TImageManager::instance()->abortLoading(w_MapAdapter);
   clearGeometries(false);
   delete w_MapAdapter;
   }

   //----------------------------------------------------------------------
   void TLayer::setSize(QSize Size_, QPoint MapMiddle_px_)
   {
   m_Size = Size_;
   changeMiddlePoint(MapMiddle_px_);
   }

   //----------------------------------------------------------------------
   void TLayer::changeMiddlePoint(QPoint MapMiddle_px_)
   {
   m_ScreenMiddle = QPoint(m_Size.width() >> 1, m_Size.height() >> 1);
   m_OffscreenViewport = QRect(QPoint(MapMiddle_px_.x() - m_ScreenMiddle.x(), 
      MapMiddle_px_.y() - m_ScreenMiddle.y()),
      m_Size);
   }

   //----------------------------------------------------------------------
   void TLayer::setVisible(bool Visible_)
   {
   m_IsVisible = Visible_;
   emit(updateRequest());
   }

   //----------------------------------------------------------------------
   void TLayer::addGeometry(TGeometry* Geometry_, bool NoUpdate_ /*= false*/)
   {
   Geometry_->setMapAdapter(w_MapAdapter);
   Geometry_->setLayer(this);
   m_Geometries.append(Geometry_);
   if(!NoUpdate_) emit(updateRequest(Geometry_->boundingBoxF()));
   //a geometry can request a redraw all, e.g. when its position has been changed and exists linked points
   if(!connect(Geometry_, SIGNAL(geometryDeleted(TGeometry*)), SIGNAL(geometryDeleted(TGeometry*)))) {
      assert(!"Connection not established!");
   }
   if(!connect(Geometry_, SIGNAL(updateRequest()), SIGNAL(updateRequest()))) {
      assert(!"Connection not established!");
   }
   //a geometry can request a redraw, e.g. when its position has been changed
   if(!connect(Geometry_, SIGNAL(updateRequest(QRectF)), SIGNAL(updateRequest(QRectF)))) {
      assert(!"Connection not established!");
      }
   // layer sends a signal element to parent
   if(!connect(Geometry_, SIGNAL(geometryClicked(TGeometry*, QPointF, QMouseEvent*)), 
                          SIGNAL(geometryClicked(TGeometry*, QPointF, QMouseEvent*)))) {
      assert(!"Connection not established!");
      }
   // layer sends a signal element to parent
   if(!connect(Geometry_, SIGNAL(selectedChanged(TGeometry*)), SIGNAL(selectedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }
   // layer sends a signal element to parent
   if(!connect(Geometry_, SIGNAL(focusedChanged(TGeometry*)), SIGNAL(focusedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }
   // layer sends a signal element to parent
   if(!connect(Geometry_, SIGNAL(modifiedChanged(TGeometry*)), SIGNAL(modifiedChanged(TGeometry*)))) {
      assert(!"Connection not established!");
      }
   layerManager()->mapControl()->setModified();
   }

   //----------------------------------------------------------------------
   bool TLayer::removeGeometry( TGeometry* Geometry_, const bool SendUpdate_ /*= true*/ )
   {
   if(Geometry_) {
      bool RetValue = m_Geometries.removeOne(Geometry_);
      if(SendUpdate_) {
         QRectF Rect = Geometry_->boundingBoxF(), NameRect = Geometry_->nameBoundingBoxF();
         if(!NameRect.isEmpty()) Rect = Rect.united(NameRect);
         emit(updateRequest(Rect));
      }
      delete Geometry_;
      layerManager()->mapControl()->setModified();
      return RetValue;
      }
   return false;
   }

   //----------------------------------------------------------------------
   bool TLayer::bringToFront( TGeometry* Geometry_, const bool SendUpdate_ /*= true*/ )
   {
   if(Geometry_) {
      if(m_Geometries.removeOne(Geometry_)) {
         m_Geometries.push_back(Geometry_);
         if(SendUpdate_) emit(updateRequest(Geometry_->boundingBoxF()));
         return true;
         }
      }
   return false;
   }

   //----------------------------------------------------------------------
   bool TLayer::sendToBack( TGeometry* Geometry_, const bool SendUpdate_ /*= true*/ )
   {
      if(Geometry_) {
         if(m_Geometries.removeOne(Geometry_)) {
            m_Geometries.push_front(Geometry_);
            if(SendUpdate_) emit(updateRequest(Geometry_->boundingBoxF()));
            return true;
         }
      }
      return false;
   }

   //----------------------------------------------------------------------
   void TLayer::clearGeometries( const bool SendUpdate_ /*= true*/ )
   {
   foreach(TGeometry *Geometry, m_Geometries) removeGeometry(Geometry, SendUpdate_);
   m_Geometries.clear();
   }

   //----------------------------------------------------------------------
   bool TLayer::isVisible() const
   {
   return m_IsVisible;
   }

   //----------------------------------------------------------------------
   void TLayer::setZoom(const int ZoomLevel_) const
   {
   w_MapAdapter->setZoom(ZoomLevel_);
   }

   //----------------------------------------------------------------------
   void TLayer::zoomIn() const
   {
   w_MapAdapter->zoom_in();
   }

   //----------------------------------------------------------------------
   void TLayer::zoomOut() const
   {
   w_MapAdapter->zoom_out();
   }

   //----------------------------------------------------------------------
   unsigned TLayer::geometriesCount( QPointF const& Point_ )
   {
      unsigned RetValue = 0;
      foreach(TGeometry *Geometry, m_Geometries) {
         if(!Geometry->isStatic() && Geometry->visible() && Geometry->touches(Point_)) ++RetValue;
      }
      return RetValue;
   }

   //----------------------------------------------------------------------
   unsigned TLayer::geometriesCount( QPoint const& Point_ )
   {
      return geometriesCount(mapadapter()->displayToCoordinate(Point_));
   }

   //----------------------------------------------------------------------
   unsigned TLayer::geometriesCount( void )
   {
      return m_Geometries.count();
   }

   //----------------------------------------------------------------------
   void TLayer::geometries(QPointF const& Point_, TGeometryList& GeometryList_)
   {
      foreach(TGeometry *Geometry, m_Geometries) {
         if(!Geometry->isStatic() && Geometry->visible() && Geometry->touches(Point_)) GeometryList_.append(Geometry);
      }
   }

   //----------------------------------------------------------------------
   void TLayer::geometries( QPoint const& Point_, /*out*/TGeometryList& GeometryList_ )
   {
      geometries(w_MapAdapter->displayToCoordinate(Point_), GeometryList_);
   }


   //----------------------------------------------------------------------
   TGeometry* TLayer::mouseEvent(QMouseEvent* Event_, TLayerManager* LayerManager_)
   {
   TGeometry* RetValue = NULL;
   QPoint MapMiddle_px = LayerManager_->mapmiddle_px();
   bool EditMode = LayerManager_->mapControl()->editMode();

   if(takesMouseEvents()) {
      Qt::MouseButton Button = Event_->button();
      //
      if(Button == Qt::NoButton && Event_->type() == QEvent::MouseMove) {
         // set tooltip text for touches geometries
         QPointF Point = w_MapAdapter->displayToCoordinate(
            QPoint(Event_->x() - m_ScreenMiddle.x() + MapMiddle_px.x(),
                   Event_->y() - m_ScreenMiddle.y() + MapMiddle_px.y()));
         foreach(TGeometry *Geometry, m_Geometries) {
            if((EditMode || !Geometry->isStatic()) && Geometry->visible() && Geometry->touches(Point)) {
               LayerManager_->addToolTipText(Geometry->description(), Geometry->descriptionColor());
               }
            }
         }
      else if((Button == Qt::LeftButton || Button == Qt::RightButton)) {
         // check for collision
         QPointF Point = w_MapAdapter->displayToCoordinate(
            QPoint(Event_->x() - m_ScreenMiddle.x() + MapMiddle_px.x(),
                   Event_->y() - m_ScreenMiddle.y() + MapMiddle_px.y()));
		   // Go through the list of items in reverse order for the z-order
         /// first check all the not static GEOMETRIES
		   QListIterator<TGeometry*> it(m_Geometries);
		   it.toBack();
		   while(it.hasPrevious()) {
			   TGeometry* Geometry = it.previous();
            if(!Geometry->isStatic() && 
               Geometry->geometryType() != "Area" && 
               Geometry->geometryType() != "PolyLine" && 
               Geometry->visible() && Geometry->touches(Point)) {
               //
               RetValue = Geometry;
				   // The top item is found. Not be pursued further.
               break;
				   }
			   }
         /// second check all the not static POLYLINES
         if(!RetValue) {
            QListIterator<TGeometry*> it(m_Geometries);
            it.toBack();
            while(it.hasPrevious()) {
               TGeometry* Geometry = it.previous();
               if(!Geometry->isStatic() && 
                  (Geometry->geometryType() == "Area" || 
                   Geometry->geometryType() == "PolyLine") && 
                  Geometry->visible() && Geometry->touches(Point)) {
                  //
                  RetValue = Geometry;
                  // The top item is found. Not be pursued further.
                  break;
                  }
               }
            }
         /// check all the other static geometries
         if(!RetValue) {
            QListIterator<TGeometry*> it(m_Geometries);
            it.toBack();
            while(it.hasPrevious()) {
               TGeometry* Geometry = it.previous();
               if(EditMode && Geometry->isStatic() && Geometry->visible() && Geometry->touches(Point)) {
                  //
                  RetValue = Geometry;
                  // The top item is found. Not be pursued further.
                  break;
                  }
               }
            }
         }
      }

   return RetValue;
   }

   //----------------------------------------------------------------------
   bool TLayer::takesMouseEvents() const
   {
   return m_TakeEvents;
   }

   //----------------------------------------------------------------------
   void TLayer::drawImage(QPainter* Painter_, const QPoint MapMiddle_px_, QRect* ViewPort_ /*= NULL*/) const
   {
   QRect Viewport;
   if(ViewPort_) Viewport = *ViewPort_; 
   else Viewport = m_OffscreenViewport;
   QPoint ViewPortMiddle;
   if(ViewPort_) ViewPortMiddle = QPoint(ViewPort_->center().x() - ViewPort_->x(), ViewPort_->center().y() - ViewPort_->y());
   else          ViewPortMiddle = m_ScreenMiddle;
   if(m_LayerType == MapLayer) { Viewport = _draw(Painter_, MapMiddle_px_, ViewPort_); }
   drawGeometries(Painter_, QPoint(MapMiddle_px_.x() - ViewPortMiddle.x(), 
                                   MapMiddle_px_.y() - ViewPortMiddle.y()), Viewport, &ViewPortMiddle);
   }

   //----------------------------------------------------------------------
   void TLayer::drawGeometries(QPainter* Painter_, const QPoint MapMiddle_px_, QRect Viewport_, QPoint* MiddlePoint_) const
   {
   QPoint MiddlePoint = m_ScreenMiddle;
   QPoint Offset = MapMiddle_px_ - MiddlePoint;
   if(MiddlePoint_) { MiddlePoint = *MiddlePoint_; Offset = MapMiddle_px_; }

   Painter_->translate(-Offset);
   // draw objects
   /// first draw all the static lines
   foreach(TGeometry *Geometry, m_Geometries) {
      if(Geometry->isStatic() && 
         (Geometry->geometryType() == "Area" || 
          Geometry->geometryType() == "PolyLine") && 
         Viewport_.intersects(Geometry->boundingBox())) {
         //
         Geometry->draw(Painter_, Viewport_, Offset);
      }
   }
   /// second draw all the other static geometry
   foreach(TGeometry *Geometry, m_Geometries) {
      if(Geometry->isStatic() && 
         Geometry->geometryType() != "Area" && 
         Geometry->geometryType() != "PolyLine" && 
         Viewport_.intersects(Geometry->boundingBox())) {
         //
         Geometry->draw(Painter_, Viewport_, Offset);
      }
   }
   /// third draw all the non static lines
   foreach(TGeometry *Geometry, m_Geometries) {
      if(!Geometry->isStatic() && 
         (Geometry->geometryType() == "Area" || 
          Geometry->geometryType() == "PolyLine") && 
         Viewport_.intersects(Geometry->boundingBox())) {
         //
         Geometry->draw(Painter_, Viewport_, Offset);
      }
   }
   /// draw all the other non static geometry
   foreach(TGeometry *Geometry, m_Geometries) {
      if(!Geometry->isStatic() && 
         Geometry->geometryType() != "Area" && 
         Geometry->geometryType() != "PolyLine" && 
         Viewport_.intersects(Geometry->boundingBox())) {
         //
         Geometry->draw(Painter_, Viewport_, Offset);
      }
   }
   // draw labels to objects (must not be obstructed by other objects, and paint them separately)
   if(m_VisibilityGeometriesName) {
      foreach(TGeometry *Geometry, m_Geometries) {
         if(Viewport_.intersects(Geometry->boundingBox())) Geometry->drawName(Painter_, Viewport_);
      }
   }
   //
   Painter_->translate(Offset);
   }

   //----------------------------------------------------------------------
   QRect TLayer::_draw( QPainter* Painter_, const QPoint MapMiddle_px_, QRect* ViewPort_ /*= NULL*/ ) const
   {
   if(!w_MapAdapter->isInitialized()) return QRect();
   QPoint ViewPortMiddle;
   if(ViewPort_) ViewPortMiddle = QPoint(ViewPort_->center().x() - ViewPort_->x(), ViewPort_->center().y() - ViewPort_->y());
   else          ViewPortMiddle = m_ScreenMiddle;
   //
   int TileSizeX = w_MapAdapter->tileSizeX();
   int TileSizeY = w_MapAdapter->tileSizeY();
   if(TileSizeX && TileSizeY) {
      QPoint TopLeftPoint     = QPoint(MapMiddle_px_.x() - ViewPortMiddle.x(), MapMiddle_px_.y() + ViewPortMiddle.y());
      QPoint BottomRightPoint = QPoint(MapMiddle_px_.x() + ViewPortMiddle.x(), MapMiddle_px_.y() - ViewPortMiddle.y());
      QPoint TopLeftTile      = w_MapAdapter->coordinateToTile(w_MapAdapter->displayToCoordinate(TopLeftPoint));
      QPoint BottomRightTile  = w_MapAdapter->coordinateToTile(w_MapAdapter->displayToCoordinate(BottomRightPoint));
      //
      QPoint MapMiddleTile = w_MapAdapter->coordinateToTile(w_MapAdapter->displayToCoordinate(MapMiddle_px_));
      int mapmiddle_tile_x = MapMiddleTile.x();
      int mapmiddle_tile_y = MapMiddleTile.y();
      // position on middle tile
      QPoint LocalCoordinates = w_MapAdapter->coordinateToTileLocal(w_MapAdapter->displayToCoordinate(MapMiddle_px_));
      int cross_x = LocalCoordinates.x(); 
      int cross_y = LocalCoordinates.y();
      // for the EmptyMapAdapter no tiles should be loaded and painted.
      if(!w_MapAdapter->isEmptyMap()) {
         int X, Y;
         // рисуем
         for(int i = TopLeftTile.x(); i <= BottomRightTile.x(); i++) {
            for(int j = BottomRightTile.y(); j <= TopLeftTile.y(); j++) {
               if(w_MapAdapter->isValid(i, j)) {
                  if(ViewPort_) { 
                     X = (i - mapmiddle_tile_x) * TileSizeX - cross_x + ViewPort_->width()  / 2;
                     Y = (j - mapmiddle_tile_y) * TileSizeY - cross_y + ViewPort_->height() / 2;
                     unsigned DX = 0, DY = 0;
                     if(X < 0) { DX = -X; X = 0; }
                     if(Y < 0) { DY = -Y; Y = 0; }
                     Painter_->drawPixmap(QPoint(X, Y), TImageManager::instance()->getImage(w_MapAdapter, i, j),
                                          QRect(QPoint(DX, DY), QSize(TileSizeX - DX, TileSizeY - DY)));
                     }
                  else {
                     X = (i - mapmiddle_tile_x) * TileSizeX - cross_x + m_Size.width();
                     Y = (j - mapmiddle_tile_y) * TileSizeY - cross_y + m_Size.height();
                     Painter_->drawPixmap(X, Y, TImageManager::instance()->getImage(w_MapAdapter, i, j));
                     }
                  }
               }
            }
         // предварительная выборка из Интернета
         if(w_MapAdapter->sourceType() == TMapAdapter::Consts::InternetMap && 
            w_MapAdapter->storageType() == TImageManager::Consts::InternetAndStorage) {
            int top    = TopLeftTile.y() + 1;
            int right  = BottomRightTile.x() + 1;
            int left   = TopLeftTile.x() - 1;
            int bottom = BottomRightTile.y() - 1;

            for(int i = left; i <= right; i++) {
               if(w_MapAdapter->isValid(i, top))    TImageManager::instance()->getImage(w_MapAdapter, i, top);
               if(w_MapAdapter->isValid(i, bottom)) TImageManager::instance()->getImage(w_MapAdapter, i, bottom);
               }
            for(int j = top + 1; j <= bottom - 1; j++) {
               if(w_MapAdapter->isValid(left, j))   TImageManager::instance()->getImage(w_MapAdapter, left, j);
               if(w_MapAdapter->isValid(right, j))  TImageManager::instance()->getImage(w_MapAdapter, right, j);
               }
            }
         }
      //
      if(w_MapAdapter->currentZoom() > 0) {
         if(!w_MapAdapter->isValid(TopLeftTile.x(), TopLeftTile.y())) {
            int NumberOfXTiles = 1 << w_MapAdapter->currentZoom();
            int NumberOfYTiles = 1 << w_MapAdapter->currentZoom();
            if(TopLeftTile.x() < 0) TopLeftTile.setX(0);
            if(TopLeftTile.y() < 0) TopLeftTile.setY(0);
            if(TopLeftTile.x() >= NumberOfXTiles) TopLeftTile.setX(NumberOfXTiles - 1);
            if(TopLeftTile.y() >= NumberOfYTiles) TopLeftTile.setY(NumberOfYTiles - 1);
            }
         if(!w_MapAdapter->isValid(BottomRightTile.x(), BottomRightTile.y())) {
            int NumberOfXTiles = 1 << w_MapAdapter->currentZoom();
            int NumberOfYTiles = 1 << w_MapAdapter->currentZoom();
            if(BottomRightTile.x() < 0) BottomRightTile.setX(0);
            if(BottomRightTile.y() < 0) BottomRightTile.setY(0);
            if(BottomRightTile.x() >= NumberOfXTiles) BottomRightTile.setX(NumberOfXTiles - 1);
            if(BottomRightTile.y() >= NumberOfYTiles) BottomRightTile.setY(NumberOfYTiles - 1);
            }
         return QRect(
            QPoint(TopLeftTile.x() * TileSizeX, BottomRightTile.y() * TileSizeY),
            QPoint((BottomRightTile.x() + 1) * TileSizeX - 1, (TopLeftTile.y() + 1) * TileSizeY - 1));
         }
      }
   return QRect();
   }

   //----------------------------------------------------------------------
   QRect TLayer::offscreenViewport() const
   {
   return m_OffscreenViewport;
   }

   //----------------------------------------------------------------------
   void TLayer::moveWidgets(const QPoint MapMiddle_px_) const
   {
   foreach(TGeometry *Geometry, m_Geometries) {
      if(Geometry->geometryType() == "Point") {
         if(((TPoint*)Geometry)->widget() != 0) {
            QPoint topleft_relative = QPoint(MapMiddle_px_ - m_ScreenMiddle);
            ((TPoint*)Geometry)->drawWidget(topleft_relative);
            }
         }
      }
   }

   //----------------------------------------------------------------------
   TLayer::LayerType TLayer::layertype(void) const
   {
   return m_LayerType;
   }

   //----------------------------------------------------------------------
   void TLayer::setMapAdapter(TMapAdapter* Mapadapter_)
   {
   w_MapAdapter = Mapadapter_;
   }
}