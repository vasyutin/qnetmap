/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl Point code by Kai Winter
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

#include "qnetmap_point.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   // -----------------------------------------------------------------------
   TPoint::TPoint(const QPointF Point_, const QString Name_, 
                  const unsigned Color_, const unsigned Thickness_)
      : TGeometry(Name_), m_Alignment(Middle),
        m_Color(Color_), m_Thickness(Thickness_)
   {
   setCoordinate(Point_, false);
   setPointParameters();
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const qreal X_, const qreal Y_, const QString Name_, 
                  const unsigned Color_, const unsigned Thickness_)
      : TGeometry(Name_), m_Alignment(Middle),
        m_Color(Color_), m_Thickness(Thickness_)
   {
   setCoordinate(QPointF(X_, Y_), false);
   setPointParameters();
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const QPointF Point_, QWidget* Widget_, const QString Name_,
                  const enum TAlignment Alignment_, const bool DeleteWidgetWithObject_)
      : TGeometry(Name_), w_Widget(Widget_), 
        m_DeleteWidgetWithObject(DeleteWidgetWithObject_), m_Alignment(Alignment_)
   {
   setCoordinate(Point_, false);
   setWidgetParameters(Widget_);
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const qreal X_, const qreal Y_, QWidget* Widget_, const QString Name_, 
                  const enum TAlignment Alignment_, const bool DeleteWidgetWithObject_)
      : TGeometry(Name_), w_Widget(Widget_),
        m_DeleteWidgetWithObject(DeleteWidgetWithObject_), m_Alignment(Alignment_)
   {
   setCoordinate(QPointF(X_, Y_), false);
   setWidgetParameters(Widget_);
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const QPointF Point_, QPixmap* Pixmap_, const QString Name_,
                  const enum TAlignment Alignment_, const bool DeletePixmapWithObject_)
      : TGeometry(Name_), w_Pixmap(Pixmap_), 
        m_DeletePixmapWithObject(DeletePixmapWithObject_), m_Alignment(Alignment_)
   {
   setCoordinate(Point_, false);
   setImageParameters(Pixmap_);
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const qreal X_, const qreal Y_, QPixmap* Pixmap_, const QString Name_, 
                  const enum TAlignment Alignment_, const bool DeletePixmapWithObject_)
      : TGeometry(Name_), w_Pixmap(Pixmap_),
        m_DeletePixmapWithObject(DeletePixmapWithObject_), m_Alignment(Alignment_)
   {
   setCoordinate(QPointF(X_, Y_), false);
   setImageParameters(Pixmap_);
   }

   // -----------------------------------------------------------------------
   TPoint::TPoint(const TPoint& Point_)
      : TGeometry(Point_.name(), Point_.description())
   {
   setCoordinate(QPointF(Point_.longitude(), Point_.latitude()), false);
   setGeometryType(Point_.geometryType());
   setTransparencyLevel(Point_.transparencyLevel());
   setVisible(Point_.visible());
   w_Widget    = Point_.w_Widget;
   w_Pixmap    = Point_.w_Pixmap;
   m_HomeLevel = Point_.m_HomeLevel;
   m_Minsize   = Point_.m_Minsize;
   m_Maxsize   = Point_.m_Maxsize;
   m_Type      = Point_.m_Type;
   m_Color     = Point_.m_Color;
   m_Thickness = Point_.m_Thickness;
   m_Alignment = Point_.m_Alignment;
   m_DisplaySize = Point_.m_DisplaySize;
   m_Size      = Point_.m_Size;
   m_TextPosition = Point_.m_TextPosition;
   }

   // -----------------------------------------------------------------------
   TPoint::~TPoint()
   {
   if(w_Widget && m_DeleteWidgetWithObject) delete w_Widget;
   if(w_Pixmap && m_DeletePixmapWithObject) delete w_Pixmap;
   }

   // -----------------------------------------------------------------------
   TAlignment TPoint::alignment() const
   {
   return m_Alignment;
   }

   // -----------------------------------------------------------------------
   TPointList TPoint::points()
   {
   TPointList Points;
   Points.append(this);
   return Points;
   }

   // -----------------------------------------------------------------------
   QWidget* TPoint::widget()
   {
   return w_Widget;
   }

   // -----------------------------------------------------------------------
   QPixmap* TPoint::pixmap()
   {
   return w_Pixmap;
   }

   // -----------------------------------------------------------------------
   void TPoint::setPointParameters(void)
   {
   setGeometryType("Point");
   m_Type = Point;
   w_Widget    = NULL;
   w_Pixmap    = NULL;
   setVisible(true);
   m_HomeLevel = -1;
   m_Minsize   = QSize(-1, -1);
   m_Maxsize   = QSize(-1, -1);
   m_Size = QSize(12, 12);
   m_TextPosition = RightMiddle;
   calcDisplaySize();
   }

   // -----------------------------------------------------------------------
   void TPoint::setImageParameters(const QPixmap* Pixmap_)
   {
   setGeometryType("Point");
   m_Type = Image;
   w_Widget    = NULL;
   setVisible(true);
   m_Size      = Pixmap_->size();
   m_HomeLevel = -1;
   m_Minsize   = QSize(-1, -1);
   m_Maxsize   = QSize(-1, -1);
   m_TextPosition = BottomMiddle;
   calcDisplaySize();
   }

   // -----------------------------------------------------------------------
   void TPoint::setWidgetParameters(const QWidget* Widget_)
   {
   setGeometryType("Point");
   m_Type = Widget;
   w_Pixmap    = NULL;
   setVisible(true);
   m_Size      = Widget_->size();
   m_HomeLevel = -1;
   m_Minsize   = QSize(-1, -1);
   m_Maxsize   = QSize(-1, -1);
   m_TextPosition = BottomMiddle;
   calcDisplaySize();
   w_Widget->show();
   }

   // -----------------------------------------------------------------------
   void TPoint::setVisible(bool Visible_)
   {
   TParent::setVisible(Visible_);
   if(w_Widget !=0) w_Widget->setVisible(Visible_);
   }

   // -----------------------------------------------------------------------
   void TPoint::setBaselevel(int zoomlevel)
   {
   m_HomeLevel = zoomlevel;
   }

   // -----------------------------------------------------------------------
   void TPoint::setMinsize(QSize minsize)
   {
   this->m_Minsize = minsize;
   }

   // -----------------------------------------------------------------------
   void TPoint::setMaxsize(QSize maxsize)
   {
   this->m_Maxsize = maxsize;
   }

   // -----------------------------------------------------------------------
   QRectF TPoint::boundingBoxF(int Zoom_)
   {
   if(!w_MapAdapter) return QRectF();
   //
   int CurrentZoom;
   if(Zoom_ >= 0) CurrentZoom = Zoom_;
   else           CurrentZoom = w_MapAdapter->currentZoom();

   if(CurrentZoom != m_Zoom || needRecalcBoundingBox()) {
      // перевычисляем глобальные координаты
      switch(m_Type) {
      case Point:
         {
         QPoint MiddlePoint = w_MapAdapter->coordinateToDisplay(coordinate());
         QRect Rect(MiddlePoint - QPoint(6, 6), QSize(11, 11));
         m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(Rect.topLeft()),
                                 w_MapAdapter->displayToCoordinate(Rect.bottomRight())).normalized();
         }
         break;
      case Image:
      case Widget:
         {
         QPoint TopLeft = alignedPoint(w_MapAdapter->coordinateToDisplay(coordinate()));
         QRect Rect(TopLeft, displaySize());
         m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(TopLeft),
                                 w_MapAdapter->displayToCoordinate(Rect.bottomRight())).normalized();
         }
         break;
      default:
         m_BoundingBoxF = QRectF(coordinate(), coordinate()).normalized();
         break;
         }
      // перевычисляем пиксельные координаты
      boundingBox(CurrentZoom);
      //
      m_Zoom = CurrentZoom;
      setNeedRecalcBoundingBox(false);
      }
   //
   return m_BoundingBoxF;
   }

   // -----------------------------------------------------------------------
   void TPoint::drawGeometry(QPainter* Painter_, const QRect &Viewport_, const QPoint Offset_)
   {
   Q_UNUSED(Viewport_)

   if(!visible() || !w_MapAdapter) return;

   calcDisplaySize();

   switch(m_Type) {
      case Image: 
         if(w_Pixmap) {
            // формируем рисунок с необходимой прозрачностью
            QPixmap TransparentPixmap(w_Pixmap->size());
            TransparentPixmap.fill(Qt::transparent);
            QPainter Painter(&TransparentPixmap);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.drawPixmap(0, 0, *w_Pixmap);
            Painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            Painter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, transparencyLevel()));
            Painter.end();
            Painter_->drawPixmap(
               alignedPoint(w_MapAdapter->coordinateToDisplay(coordinate())), TransparentPixmap);
            }
         else assert(!"TPoint::drawGeometry: Pointer to image is null");
         break;
      case Widget: 
         if(w_Widget) drawWidget(Offset_);
         else assert(!"TPoint::drawGeometry: Pointer to widget is null");
         break;
      case Point:
         {
         // формируем рисунок с необходимой прозрачностью
         QImage TransparentPixmap(11, 11, QImage::Format_ARGB32_Premultiplied);
         TransparentPixmap.fill(Qt::transparent);
         QPainter Painter(&TransparentPixmap);
         Painter.setCompositionMode(QPainter::CompositionMode_Source);
         Painter.setPen(QPen(m_Color, m_Thickness));
         Painter.drawLine(QPoint(0, 0),  QPoint(10, 10));
         Painter.drawLine(QPoint(0, 10), QPoint(10, 0));
         Painter.fillRect(2, 2, 7, 7, QBrush(m_Color));
         Painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
         Painter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, transparencyLevel()));
         Painter.end();
         Painter_->drawImage(
            w_MapAdapter->coordinateToDisplay(coordinate()) - QPoint(6, 6), TransparentPixmap);
         }
         break;
      default:
         assert(!"TPoint::drawGeometry: Point type don't known");
         break;
      }
   }

   // -----------------------------------------------------------------------
   void TPoint::drawWidget(const QPoint Offset_)
   {
   if(!w_MapAdapter) return;

   QPoint Point = w_MapAdapter->coordinateToDisplay(coordinate());
   Point -= Offset_;

   QPoint TopLeft = alignedPoint(Point);
   w_Widget->setGeometry(TopLeft.x(), TopLeft.y(), displaySize().width(), displaySize().height());
   }

   // -----------------------------------------------------------------------
   void TPoint::calcDisplaySize(void)
   {
   if(m_HomeLevel > 0) {
      int CurrentZoom = w_MapAdapter->currentZoom();

      int DiffZoom = m_HomeLevel - CurrentZoom;
      int ViewHeight = int(m_Size.height() / (1 << DiffZoom));
      int ViewWidth  = int(m_Size.width()  / (1 << DiffZoom));

      if(m_Minsize.height() != -1 && ViewHeight < m_Minsize.height())      ViewHeight = m_Minsize.height();
      else if(m_Maxsize.height() != -1 && ViewHeight > m_Maxsize.height()) ViewHeight = m_Maxsize.height();

      if(m_Minsize.width()!= -1 && ViewWidth < m_Minsize.width())       ViewWidth = m_Minsize.width();
      else if(m_Maxsize.width() != -1 && ViewWidth > m_Maxsize.width()) ViewWidth = m_Maxsize.width();

      m_DisplaySize = QSize(ViewWidth, ViewHeight);
      }
   else m_DisplaySize = m_Size;
   }

   // -----------------------------------------------------------------------
   QPoint TPoint::alignedPoint(const QPoint Point_, QSize Size_) const
   {
   QSize Size;
   if(Size_.isEmpty()) Size = displaySize();
   else                Size = Size_;
   
   return TBase::alignedPoint(Point_, Size, m_Alignment);
   }

   // -----------------------------------------------------------------------
   QPoint TPoint::middlePoint(void) const
   {
      QSize Size = displaySize();
      QPoint TopLeftPoint = alignedPoint(w_MapAdapter->coordinateToDisplay(coordinate()), Size);

      return TopLeftPoint + QPoint((Size.width() + 1) >> 1, (Size.height() + 1) >> 1);
   }

   // -----------------------------------------------------------------------
   bool TPoint::setPixmap( QPixmap* Pixmap_ )
   {
      bool RetValue = false;
      //
      if(Pixmap_ && w_Pixmap) {
         if(m_DeletePixmapWithObject) delete w_Pixmap;
         w_Pixmap = Pixmap_;
         RetValue = true;
      }
      //
      return RetValue;
   }
}
