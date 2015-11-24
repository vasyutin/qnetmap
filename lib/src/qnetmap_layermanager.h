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

#ifndef QNETMAP_LAYERMANAGER_H
#define QNETMAP_LAYERMANAGER_H

#include "qnetmap_global.h"
#include "qnetmap_geometry.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMutex>
#include <QAtomicInt>

namespace qnetmap
{
   class TLayer;
   class TMapAdapter;
   class TMapControl;

   typedef QSet<TGeometry*> TSetGeometries;
   typedef QList<TLayer*> TLayers;

   //! \class TLayerManager
   //! \brief Handles Layers and viewport related settings
   /*!
   * This class handles internally all layers which were added to the TLayerManager.
   * It also stores values for scrolling.
   * It initiates the creation of a new offscreen image and on zooming the zoom images gets here created.
   *	@author Kai Winter <sergey.shumeyko@gmail.com>
   */
   class QNETMAP_EXPORT TLayerManager : public TBase
   {
      Q_OBJECT

   public:
      TLayerManager(TMapControl*, const QSize&);
      virtual ~TLayerManager();

      //! returns the coordinate of the center of the map
      /*!
      * @return returns the coordinate of the middle of the screen
      */
      QPointF currentCoordinate() const;

      //! returns the current offscreen image
      /*!
      * @return the current offscreen image
      */
      inline QPixmap getImage() const;

      //! returns the layer with the given name
      /*!
      * @param  layername name of the wanted layer
      * @return the layer with the given name
      */
      TLayer* layer(const QString&) const;

      //! returns the base layer
      /*!
      * This will return the base layer of the TLayerManager.
      * The base layer is the one which is used to do internal coordinate calculations.
      * @return the base layer
      */
      TLayer* layer() const;

      //! returns the names of all layers
      /*!
      * @return returns a QList with the names of all layers
      */
      QList<QString> layerNames() const;

      //! returns the list with the pointers to all layers
      /*!
      * @return returns a QList with the pointers to all layers
      */
      TLayers const& TLayerManager::layers();

      //! sets the middle of the map to the given coordinate
      /*!
      * @param  coordinate the coordinate which the view?s middle should be set to
      */
      void setView(const QPointF& coordinate, const bool MapUpdate_ = true);

      //! sets the view, so all coordinates are visible
      /*!
      * @param  coordinates the Coorinates which should be visible
      */
      void setView(const QList<QPointF>& coordinates);

      //! sets the view and zooms in, so all coordinates are visible
      /*!
      * The code of setting the view to multiple coordinates is "brute force" and pretty slow.
      * Have to be reworked.
      * @param  coordinates the Coorinates which should be visible
      */
      //void setViewAndZoomIn(const QList<QPointF>& coordinates);

      //! zooms in one step
      inline void zoomIn(const bool ZoomBackground_ = true, const bool MapUpdate_ = true);

      //! zooms out one step
      inline void zoomOut(const bool ZoomBackground_ = true, const bool MapUpdate_ = true);

      //! sets the given zoomlevel
      /*!
      * @param zoomlevel the zoomlevel
      */
      void setZoom(const int zoomlevel, const bool ZoomBackground_ = true, const bool MapUpdate_ = true);

      //! The Viewport of the display
      /*!
      * Returns the visible viewport in world coordinates
      * @return the visible viewport in world coordinates
      */
      QRectF viewport(void) const;

      //! The Viewport of the display
      /*!
      * Returns the visible viewport in pixeles
      * @return the visible viewport in pixeles
      */
      QRect viewportInPixeles(void) const;

      //! scrolls the view
      /*!
      * Scrolls the view by the given value in pixels and in display coordinates
      * @param  offset the distance which the view should be scrolled
      */
      void scrollView(const QPoint& Point_);
      //! \brief
	   void changeMiddlePoint(const QPoint& point);
      //! \brief
      void scrollOffscreenImage(const QPoint& offset);

      //! returns the middle of the map in projection coordinates
      /*!
      * @return the middle of the map in projection coordinates
      */
      QPoint mapmiddle_px(void);
      //! \brief 
      void forceRedraw();
      //! \brief 
      void removeZoomImage();
      //! adds a layer
      /*!
      * If multiple layers are added, they are painted in the added order.
      * @param layer the layer which should be added
      */
      bool addLayer(TLayer* layer);
      //! \brief returns the current zoom level
      int currentZoom() const;
      //! \brief
      void drawGeometries(QPainter* painter);
      //! \brief
      void drawImage(QPainter* painter);
      //! \brief
      inline QSize ComposedOffscreenImageSize(void) { return m_OffSize; }
      //! \brief
      void addToolTipText(const QString& Text_, const QRgb Color_ = QRgb(0x0));
      //! \brief
      inline unsigned geometriesUnderMouseCount(void) const { return m_GeometriesUnderMouseCount; }
      //! \brief
      QString toolTipText(void);
      //! \brief
      inline void clearToolTipText(void) { m_TooltipText.clear(); m_GeometriesUnderMouseCount = 0; }
      //! \brief
      inline QPoint toolTipGlobalPos(void) const { return m_ToolTipGlobalPos; }
      //! \brief
	   inline TGeometry* focused(void) const { return w_FocusedGeometry; }
      //! \brief
	   void setFocused(TGeometry* Geometry_);
      //! \brief
	   void setSelected(TGeometry* Geometry_);
      //! \brief
	   unsigned setSelected(const QRectF& RectF_);
      //! \brief
	   inline bool removeSelected(TGeometry* Geometry_) { return m_SelectedGeometries.remove(Geometry_); }
      //! \brief
      unsigned selectAll(void);
      //! \brief
      void clearSelected(void);
      //! \brief
	   inline unsigned selectedCount(void) const { return m_SelectedGeometries.size(); }
      //! \brief
	   TGeometry* selected(const unsigned Index_);
      //! \brief
      inline TMapControl* mapControl(void) const { return w_MapControl; }
      //! \brief
      inline unsigned tooltipLinesMaxCount(void) const { return m_TooltipLinesMaxCount; }
      //! \brief
      inline void setTooltipLinesMaxCount(const unsigned Count_) { m_TooltipLinesMaxCount = Count_; }
      //! \brief
      QPixmap getMapFragment(const QPointF& MiddlePoint_, const QSize& Size_, const int Zoom_);

      //! forwards mouseevents to the layers
      /*!
      * This method is invoked by the TMapControl which receives Mouse Events.
      * These events are forwarded to the layers, so they can check for clicked geometries.
      * @param  evnt the mouse event
      * @retun TGeometry* if a geometry is clicked
      */
      TGeometry* mouseEvent(QMouseEvent* evnt);
      //! \brief
      inline bool mouseMoved(void) const { return m_MouseMoved; }
      //! \brief
      inline bool mouseButtonPressed(void) const { return m_MouseButtonPressed; }
		//! \brief 
		TSetGeometries& selectedGeometries(void) { return m_SelectedGeometries; }
		//! \brief 
		unsigned selectedGeometriesCount(void) const { return m_SelectedGeometries.size(); }

   private:
      Q_DISABLE_COPY(TLayerManager)

      //! This method have to be invoked to draw a new offscreen image
      /*!
      * @param clearImage if the current offscreeen image should be cleared
      * @param showZoomImage if a zoom image should be painted
      */
      void newOffscreenImage(bool ClearImage_=true, bool ShowZoomImage_=true);
      //! \brief
      inline bool checkOffscreen() const;
      //! \brief
      inline bool containsAll(const QList<QPointF>& coordinates) const;
      //! \brief
      inline void moveWidgets();
      //! \brief
      inline void setMiddle(const QList<QPointF>& Coordinates_, const bool MapUpdate_ = true);
      //! \brief
      void zoom(const int Type_, const bool ZoomBackground_ = true, const bool MapUpdate_ = true);
      //! \brief
      void setMiddlePoint(void);

      TMapControl* w_MapControl;
      QPoint m_ScreenMiddle;    // middle of the screen
      QPoint m_Scroll;          // scrollvalue of the offscreen image
      QPoint m_ZoomImageScroll; // scrollvalue of the zoom image

      QSize m_Size;    // widget size
      QSize m_OffSize; // size of the offscreen image

      QPixmap m_ComposedOffscreenImage;
      QPixmap m_ComposedOffscreenImage2;
      QPixmap m_MousePressedOffscreen;
      QPixmap m_ZoomImage;
      QPainter m_ComposedOffscreenImage2Painter;

      TLayers	m_Layers;

      QPoint  m_MapMiddle_px; // projection-display coordinates
      QPointF m_MapMiddle;    // world coordinate

      QPoint  m_WhileNewScroll;

      QString    m_TooltipText;
      unsigned   m_GeometriesUnderMouseCount;  
      QAtomicInt m_ToolTipAtomicInt;
      QPoint     m_ToolTipGlobalPos;

	   TGeometry*	   w_FocusedGeometry;
	   TSetGeometries m_SelectedGeometries;

      bool m_MouseMoved;
      bool m_MouseButtonPressed;
      unsigned m_TooltipLinesMaxCount;

      QAtomicInt m_OffscreenImageAtomicInt;
      QAtomicInt m_ZoomAtomicInt;

   signals:
      //! \brief A Layer emits this signal, when a geometry selected state gets changed
      //!        A LayerManager translate this signal above
      void selectedChanged(TGeometry*);
      //! \brief A Layer emits this signal, when a geometry focused state gets changed
      //!        A LayerManager translate this signal above
      void focusedChanged(TGeometry*);
      //! \brief A Layer emits this signal, when a geometry modified state gets changed
      //!        A LayerManager translate this signal above
      void modifiedChanged(TGeometry*);
      //! \brief A Layer emits this signal, when a geometry clicked
      //!        A LayerManager translate this signal above
      void geometryClicked(TGeometry*, QPointF, QMouseEvent*);
      //! \brief 
      void geometryDeleted(TGeometry*);
      //! \brief 
      void currentZoomChanged(int Zoom_);

   public slots:
      void layerGeometryClicked(TGeometry* Geometry_, QPointF Point_, QMouseEvent* Event_);
      void layerGeometryDeleted(TGeometry* Geometry_);
      void geometrySelectedChanged(TGeometry* Geometry_);
      void geometryFocusedChanged(TGeometry* Geometry_);
      void geometryModifiedChanged(TGeometry* Geometry_);
      void updateRequest(const QRectF& Rect_);
      void updateRequest();
      void resize(QSize newSize);
   };
}
#endif
