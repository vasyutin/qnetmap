/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl LineString code by Kai Winter
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

#include "qnetmap_linestring.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TLineString::TLineString() : TGeometry()
   {
   setGeometryType("LineString");
   }

   TLineString::TLineString(const TPointList& Points_, const QString& name, QPen* pen) : TGeometry(name)
   {
   setPen(pen);
   setGeometryType("LineString");
   setPoints(Points_);
   }

   TLineString::~TLineString()
   {
   foreach(TPoint* Point, m_Vertices) delete Point;
   }

   // TGeometry TLineString::Clone(){}
   // TPoint TLineString::EndPoint(){}
   // TPoint TLineString::StartPoint(){}
   // TPoint TLineString::Value(){}

   //-------------------------------------------------------------------------------------
   void TLineString::addPoint(TPoint* Point_)
   {
   Point_->setParentGeometry(this);
   m_Vertices.append(Point_);
   }

   //-------------------------------------------------------------------------------------
   TPointList TLineString::points()
   {
   return m_Vertices;
   }

   //-------------------------------------------------------------------------------------
   void TLineString::setPoints(const TPointList& ListPoints_)
   {
   foreach(TPoint* Point, ListPoints_) Point->setParentGeometry(this);
   m_Vertices = ListPoints_;
   }

   //-------------------------------------------------------------------------------------
   void TLineString::drawGeometry(QPainter* Painter_, const QRect &Viewport_, const QPoint Offset_)
   {
   if(!visible() || !w_MapAdapter) return;

   QPolygon Polygon = QPolygon();

   for(int i = 0; i < m_Vertices.size(); i++) {
      Polygon.append(w_MapAdapter->coordinateToDisplay(m_Vertices[i]->coordinate()));
      }

   Painter_->save();
   if(pen()) Painter_->setPen(*pen());
   Painter_->drawPolyline(Polygon);
   Painter_->restore();

   for(int i = 0; i < m_Vertices.size(); i++) m_Vertices[i]->draw(Painter_, Viewport_, Offset_);
   }

   //-------------------------------------------------------------------------------------
   int TLineString::pointsCount() const
   {
   return m_Vertices.count();
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TLineString::painterPath(void) const
   {
   QPainterPath PainterPath;
   // линия
   QPolygonF Polygon;
   // добавляем вершины в прямом проходе
   foreach(TPoint* Point, m_Vertices) Polygon.append(Point->coordinate());
   // добавляем вершины в обратном проходе
   TPointListIterator it(m_Vertices);
   it.toBack();
   while(it.hasPrevious()) Polygon.append(it.previous()->coordinate());
   //
   PainterPath.addPolygon(Polygon);
   PainterPath.closeSubpath();
   // точки
   foreach(TPoint* Point, m_Vertices) {
      PainterPath.addPath(Point->painterPath());
      PainterPath.closeSubpath();
      }
   //
   return PainterPath;
   }

   //-------------------------------------------------------------------------------------
   bool TLineString::hasPoints() const
   {
   return m_Vertices.size() > 0;
   }

   //-------------------------------------------------------------------------------------
   QRectF TLineString::boundingBoxF(int Zoom_)
   {
   QRectF Rect;
   foreach(TPoint* Point, m_Vertices) Rect = Rect.united(Point->boundingBoxF(Zoom_));
   return Rect;
   }
}
