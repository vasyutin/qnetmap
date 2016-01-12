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

#ifndef QNETMAP_LAYER_H
#define QNETMAP_LAYER_H

#include "qnetmap_global.h"
#include "qnetmap_interfaces.h"

class QPainter;
class QMouseEvent;

namespace qnetmap
{
   class TMapAdapter;
   class TGeometry;
   class TLayerManager;

   typedef QList<TGeometry*> TGeometryList;

   //! \class TLayer
   //! \brief класс, реализующий слой карты
   class QNETMAP_EXPORT TLayer : public TBase
   {
      Q_OBJECT

   public:
      friend class TLayerManager;

      //! sets the type of a layer, see Layer class doc for further information
      enum LayerType {
         MapLayer,      /*!< uses the MapAdapter to display maps, only gets refreshed when a new offscreen image is needed */
         GeometryLayer  /*!< gets refreshed everytime when a geometry changes */
         };

      TLayer(const QString& layername, TMapAdapter* mapadapter, enum LayerType layertype, bool takeevents=true);
      virtual ~TLayer();

      //! \brief returns the layer's name
      const QString& layername(void) const { return m_Layername; }
      //! \brief returns the layer's MapAdapter
      const TMapAdapter* mapadapter(void) const { return w_MapAdapter; }
      //! \brief Adds a geometry object to the layer. The layout Takes ownership of the geometry.
      void addGeometry(TGeometry* geometry, bool NoUpdate_ = false);
      //! \brief removes the Geometry object from this Layer
      bool removeGeometry(TGeometry* Geometry_, const bool SendUpdate_ = true);
      //! \brief bring to front the Geometry object
      bool bringToFront(TGeometry* Geometry_, const bool SendUpdate_ = true);
      //! \brief send to back the Geometry object
      bool sendToBack(TGeometry* Geometry_, const bool SendUpdate_ = true);
      //! \brief removes all Geometry objects from this Layer
      void clearGeometries(const bool SendUpdate_ = true);
      //! \brief return true if the layer is visible
      bool isVisible(void) const;
      //! \brief returns the LayerType of the Layer
      TLayer::LayerType layertype(void) const;
      //! \brief sets the adapter card for the layer
      void setMapAdapter(TMapAdapter* mapadapter);
      //! \brief sets zoom for layer
      void setZoom(const int ZoomLevel_) const;
      //! \brief returns a list of pointers to the figures, located on layer
      TGeometryList const& geometries(void) { return m_Geometries; }
      //! \brief returns a list of pointers to shape within the specified display point
      void geometries(QPoint const& Point_, /*out*/TGeometryList& GeometryList_);
      //! \brief returns a list of pointers to shape within the specified point
      void geometries(QPointF const& Point_, /*out*/TGeometryList& GeometryList_);
      //! \brief returns the number of figures that are on the layer
      unsigned geometriesCount(void);
      //! \brief returns the number of figures in the specified point
      unsigned geometriesCount(QPoint const& Point_);
      //! \brief returns the number of figures in the specified point
      unsigned geometriesCount(QPointF const& Point_);
      //! \brief returns the pointer to parent layer manager
      TLayerManager* layerManager(void) { return w_LayerManager; }

   private:
      Q_DISABLE_COPY(TLayer)

      void moveWidgets(const QPoint MapMiddle_px) const;
      void drawImage(QPainter* Painter_, const QPoint MapMiddle_px_, QRect* ViewPort_ = NULL) const;
      void drawGeometries(QPainter* Painter_, const QPoint MapMiddle_px_, QRect Viewport_, QPoint* MiddlePoint_ = NULL) const;
      void setSize(QSize size, QPoint MapMiddle_px);
      void changeMiddlePoint(QPoint MapMiddle_px_);
      QRect offscreenViewport(void) const;
      bool takesMouseEvents(void) const;
      //! \return TGeometry* if a geometry is clicked
      TGeometry* mouseEvent(QMouseEvent* Event_, TLayerManager* LayerManager_);
      void zoomIn(void) const;
      void zoomOut(void) const;
      QRect _draw(QPainter* Painter_, const QPoint MapMiddle_px_, QRect* ViewPort_ = NULL) const;
      //! \brief establishes a sign drawing names figures
      void setVisibilityGeometriesName(const bool Visibility_) 
      { 
      if(m_VisibilityGeometriesName != Visibility_) {
         m_VisibilityGeometriesName = Visibility_;
         emit(updateRequest());
         }
      }
      //! \brief returns the sign of drawing the names of figures
      bool visibilityGeometriesName(void) const { return m_VisibilityGeometriesName; }
      void setLayerManager(TLayerManager* LayerManager) { w_LayerManager = LayerManager; }

      bool      m_IsVisible;
      QString   m_Layername;
      LayerType m_LayerType;
      QSize     m_Size;
      QPoint    m_ScreenMiddle;

      TGeometryList  m_Geometries;
      TMapAdapter*   w_MapAdapter;
      bool           m_TakeEvents;
      mutable QRect  m_OffscreenViewport;
      //! \var sign drawing names figures
      bool           m_VisibilityGeometriesName;
      TLayerManager* w_LayerManager;

   signals:
      //! This signal is emitted when a Geometry is clicked
      /*!
      * A Geometry is clickable, if the containing layer is clickable.
      * The layer emits a signal for every clicked geometry
      * @param  geometry The clicked Geometry
      * @param  point The coordinate (in widget coordinates) of the click
      */
      void geometryClicked(TGeometry* geometry, QPointF point, QMouseEvent* Event_);
      //! \brief A Geometry emits this signal, when its selected state gets changed
      //!        Layer translate this signal above
      void selectedChanged(TGeometry*);
      //! \brief A Geometry emits this signal, when its focused state gets changed
      //!        Layer translate this signal above
      void focusedChanged(TGeometry*);
      //! \brief A Geometry emits this signal, when its modified state gets changed
      //!        Layer translate this signal above
      void modifiedChanged(TGeometry*);
      //! \brief 
      void updateRequest(QRectF rect);
      //! \brief 
      void updateRequest(void);
      //! \brief 
      void geometryDeleted(TGeometry*);

   public slots:
      //! if visible is true, the layer is made visible
      void setVisible(bool visible);
   };
}

#endif
