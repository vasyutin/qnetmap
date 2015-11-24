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

#include <QPainter>

#include "qnetmap_polygon.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TPolygon::TPolygon() : TGeometry(), m_VerticesChanged(true)
   {
   setGeometryType("Polygon");
   }

   TPolygon::TPolygon(const TPointList &Points_, const QString& Name_,
      const unsigned FillColor_, const unsigned BorderColor_, const unsigned BorderWidth_)
      : TGeometry(Name_), m_FillColor(FillColor_),
        m_BorderColor(BorderColor_), m_BorderWidth(BorderWidth_), m_VerticesChanged(true)
   {
   setGeometryType("Polygon");
   setVertices(Points_);
   m_TextPosition = Middle;
   }

   //-------------------------------------------------------------------------------------
   QRectF TPolygon::boundingBoxF(int Zoom_)
   {
   if(!w_MapAdapter) return QRectF();
   //
   int CurrentZoom;
   if(Zoom_ >= 0) CurrentZoom = Zoom_;
   else           CurrentZoom = w_MapAdapter->currentZoom();

   if(m_VerticesChanged || CurrentZoom != m_Zoom || needRecalcBoundingBox()) {
      if(m_VerticesChanged) {
         // перевычисляем глобальные координаты
         m_BoundingBoxF = m_Vertices.boundingRect().normalized();
         m_VerticesChanged = false;
         }
      // перевычисляем пиксельные координаты
      boundingBox(CurrentZoom);
      // перевычисляем пиксельные параметры
      m_LocalPolygon.clear();
      foreach(QPointF Point, m_Vertices) m_LocalPolygon.append(w_MapAdapter->coordinateToDisplay(Point));
      QRect BoundingRect = m_LocalPolygon.boundingRect();
      m_TopLeft = BoundingRect.topLeft();
      QRect PolygonRect = QRect(QPoint(0, 0), BoundingRect.size());
      m_LocalPolygon.translate(-m_TopLeft);
      m_PolygonRectSize = PolygonRect.size();
      //
      m_Zoom = CurrentZoom;
      setNeedRecalcBoundingBox(false);
      }
   //
   return m_BoundingBoxF;
   }

   //-------------------------------------------------------------------------------------
   void TPolygon::addVertex(const QPointF& Point_)
   {
   m_Vertices.push_back(Point_);
   m_VerticesChanged = true;
   m_Zoom = -1;
   }

   //-------------------------------------------------------------------------------------
   QPolygonF TPolygon::vertices(void) const
   {
   return m_Vertices;
   }

   //-------------------------------------------------------------------------------------
   void TPolygon::setVertices(const TPointList& Points_)
   {
   m_Vertices = Points_;
   m_VerticesChanged = true;
   m_Zoom = -1;
   }

   //-------------------------------------------------------------------------------------
   void TPolygon::drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
   Q_UNUSED(Viewport_)
   Q_UNUSED(Offset_)

   if(!visible() || !w_MapAdapter || m_Vertices.count() < 3) return;

   // пересчитываем точки только при изменении зума
   int CurrentZoom = w_MapAdapter->currentZoom();
   if(CurrentZoom != m_Zoom) {
      boundingBoxF(CurrentZoom);
      m_Zoom = CurrentZoom;
      }

   // формируем рисунок с необходимой прозрачностью
   QPixmap TransparentPixmap(m_PolygonRectSize);
   TransparentPixmap.fill(Qt::transparent);
   QPainter Painter(&TransparentPixmap);
   Painter.setCompositionMode(QPainter::CompositionMode_Source);
   Painter.setClipRegion(QRegion(m_LocalPolygon));
   Painter.setPen(Qt::NoPen);
   Painter.setBrush(QBrush(m_FillColor));
   Painter.drawPolygon(m_LocalPolygon);
   Painter.setPen(QPen(m_BorderColor, m_BorderWidth * 2));
   Painter.setBrush(Qt::NoBrush);
   Painter.drawPolygon(m_LocalPolygon);
   Painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
   Painter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, transparencyLevel()));
   Painter.end();
   // рисуем
   Painter_->drawPixmap(m_TopLeft, TransparentPixmap);
   }

   //-------------------------------------------------------------------------------------
   bool TPolygon::touches(const QPointF Point_)
   {
   if(visible()) return m_Vertices.containsPoint(Point_, Qt::OddEvenFill);
   return false;
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TPolygon::painterPath(void) const
   {
   QPainterPath PainterPath;
   PainterPath.addPolygon(m_Vertices);
   PainterPath.closeSubpath();
   return PainterPath;
   }
}
