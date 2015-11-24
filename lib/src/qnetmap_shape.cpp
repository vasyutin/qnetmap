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

#include "qnetmap_shape.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TShape::TShape() : TGeometry()
   {
   setGeometryType("Shape");
   }

   TShape::TShape(const enum TShapeType ShapeType_, const QRectF &Rect_, const QString& Name_,
      const unsigned FillColor_, const unsigned BorderColor_, const unsigned BorderWidth_)
      : TGeometry(Name_), m_ShapeType(ShapeType_), m_FillColor(FillColor_),
        m_BorderColor(BorderColor_), m_BorderWidth(BorderWidth_)
   {
   setGeometryType("Shape");
   m_BoundingBoxF = Rect_.normalized();
   m_TextPosition = Middle;
   }

   //-------------------------------------------------------------------------------------
   void TShape::drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
   Q_UNUSED(Offset_)
   Q_UNUSED(Viewport_)

   if(!visible() || !w_MapAdapter) return;

   // пересчитываем только при изменении зума
   int CurrentZoom = w_MapAdapter->currentZoom();
   if(CurrentZoom != m_Zoom) {
      boundingBox(CurrentZoom);
      m_Zoom = CurrentZoom;
      }

   // формируем рисунок с необходимой прозрачностью
   QRect Rect(QPoint(0, 0), m_BoundingBox.normalized().size());

   QPixmap TransparentPixmap(Rect.size());
   TransparentPixmap.fill(Qt::transparent);
   QPainter Painter(&TransparentPixmap);
   Painter.setCompositionMode(QPainter::CompositionMode_Source);
   //
   switch(m_ShapeType) {
   case Rectangle:
      {
      Painter.setPen(Qt::NoPen);
      Painter.setBrush(QBrush(m_FillColor));
      Painter.drawRect(Rect);
      Painter.setPen(QPen(m_BorderColor, m_BorderWidth * 2));
      Painter.setBrush(Qt::NoBrush);
      Painter.drawRect(Rect);
      break;
      }
   case Ellipse:
      {
      QRectF RectF(Rect.x(), Rect.y(), Rect.width(), Rect.height());
      QPainterPath Path;
      Path.addEllipse(RectF);
      Painter.setClipPath(Path);
      //
      Painter.setPen(Qt::NoPen);
      Painter.setBrush(QBrush(m_FillColor));
      Painter.drawEllipse(RectF);
      Painter.setPen(QPen(m_BorderColor, m_BorderWidth * 2));
      Painter.setBrush(Qt::NoBrush);
      Painter.drawEllipse(RectF);
      break;
      }
   default:
      assert(!"qnetmap::TShape: Shape type don't known.");
      break;
      }
   // устанавливаем прозрачность
   Painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
   Painter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, transparencyLevel()));
   //
   Painter.end();
   // рисуем
   Painter_->drawPixmap(m_BoundingBox.normalized().topLeft(), TransparentPixmap);
   }

   // -------------------------------------------------------------
   QRectF TShape::boundingBoxF(int Zoom_)
   {
   if(!w_MapAdapter) return QRectF();
   //
   int CurrentZoom;
   if(Zoom_ >= 0) CurrentZoom = Zoom_;
   else           CurrentZoom = w_MapAdapter->currentZoom();

   if(CurrentZoom != m_Zoom || needRecalcBoundingBox()) {
      // перевычисляем пиксельные координаты
      boundingBox(CurrentZoom);
      //
      m_Zoom = CurrentZoom;
      setNeedRecalcBoundingBox(false);
      }
   //
   return m_BoundingBoxF;
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TShape::painterPath(void)
   {
   QPainterPath PainterPath;
   //
   switch(m_ShapeType) {
   case Ellipse:
      PainterPath.addEllipse(m_BoundingBoxF);
      break;
   case Rectangle:
   default:
      PainterPath = TParent::painterPath();
      break;
      }
   PainterPath.closeSubpath();
   //
   return PainterPath;
   }
}
