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
#include <QPicture>

#include "qnetmap_arbitraryfigure.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   ///////////////////////////////////////////////////////////////////////////////////////

   TArbitraryFigure::TArbitraryFigure(const QPointF Point_,
                                      const TDrawFunction DrawFigure_,
                                      const QVariant& DrawFunctionParameters_, 
                                      const enum TAlignment Alignment_,
                                      const QString& Name_)
      : TGeometry(Name_), w_DrawFigure(DrawFigure_), m_DrawFunctionParameters(DrawFunctionParameters_),
        m_Alignment(Alignment_), m_IsBoundingBoxInitialised(false)
   {
   setCoordinate(Point_, false);
   setGeometryType("Figure");
   setDisplaySize();
   setBoundingBox();
   }

   // -----------------------------------------------------------------------
   void TArbitraryFigure::setDisplaySize(void)
   {
   QPicture Picture;
   QPainter PicturePainter(&Picture);
   // call a function that draws a figure
   w_DrawFigure(&PicturePainter, m_DrawFunctionParameters);
   //
   PicturePainter.end();
   m_DisplaySize = Picture.boundingRect().size();
   }

   // -----------------------------------------------------------------------
   void TArbitraryFigure::setBoundingBox(void)
   {
   if(!w_MapAdapter) return;

   m_BoundingBox  = QRect(alignedPoint(w_MapAdapter->coordinateToDisplay(coordinate())), m_DisplaySize);
   m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(m_BoundingBox.topLeft()),
                           w_MapAdapter->displayToCoordinate(m_BoundingBox.bottomRight()));
   m_IsBoundingBoxInitialised = true;
   }

   //-------------------------------------------------------------------------------------
   void TArbitraryFigure::drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
   Q_UNUSED(Offset_)
   Q_UNUSED(Viewport_)

   if(!visible() || !w_MapAdapter) return;

   if(!m_IsBoundingBoxInitialised) setBoundingBox();

   // recalculate only when zoomed
   int CurrentZoom = w_MapAdapter->currentZoom();
   if(CurrentZoom != m_Zoom) {
      boundingBox(CurrentZoom);
      m_Zoom = CurrentZoom;
      }

   // form a pattern with the necessary transparency
   QPixmap TransparentPixmap(m_DisplaySize);
   TransparentPixmap.fill(Qt::transparent);
   QPainter PixmapPainter(&TransparentPixmap);
   PixmapPainter.setCompositionMode(QPainter::CompositionMode_Source);
   /// call a function that draws a figure
   w_DrawFigure(&PixmapPainter, m_DrawFunctionParameters);
   /// set transparent
   PixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
   PixmapPainter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, transparencyLevel()));
   PixmapPainter.end();
   // draw
   Painter_->drawPixmap(m_BoundingBox.normalized().topLeft(), TransparentPixmap);
   }

   //---------------------------------------------------------------------------
   QRect TArbitraryFigure::boundingBox(int Zoom_)
   {
   if(!w_MapAdapter) return QRect();
   //
   int CurrentZoom;
   if(Zoom_ >= 0) CurrentZoom = Zoom_;
   else           CurrentZoom = w_MapAdapter->currentZoom();

   if(CurrentZoom != m_Zoom) {
      // recomputes the pixel coordinates
      setBoundingBox();
      m_Zoom = CurrentZoom;
      }
   //
   return m_BoundingBox;
   }

   // -------------------------------------------------------------
   QRectF TArbitraryFigure::boundingBoxF(int Zoom_)
   {
   if(!w_MapAdapter) return QRectF();
   //
   boundingBox(Zoom_);
   //
   return m_BoundingBoxF;
   }

   // -----------------------------------------------------------------------
   QPoint TArbitraryFigure::alignedPoint(const QPoint Point_) const
   {
   QPoint TopLeftPoint;
   
   switch(m_Alignment) {
   case Middle:
      TopLeftPoint.setX(Point_.x() - ((m_DisplaySize.width()  + 1) >> 1));
      TopLeftPoint.setY(Point_.y() - ((m_DisplaySize.height() + 1) >> 1));
      break;
   case TopLeft:
      TopLeftPoint.setX(Point_.x());
      TopLeftPoint.setY(Point_.y());
      break;
   case TopMiddle:
      TopLeftPoint.setX(Point_.x() - ((m_DisplaySize.width() + 1) >> 1));
      TopLeftPoint.setY(Point_.y());
      break;
   case TopRight:
      TopLeftPoint.setX(Point_.x() - m_DisplaySize.width());
      TopLeftPoint.setY(Point_.y());
      break;
   case BottomLeft:
      TopLeftPoint.setX(Point_.x());
      TopLeftPoint.setY(Point_.y() - m_DisplaySize.height());
      break;
   case BottomMiddle:
      TopLeftPoint.setX(Point_.x() - ((m_DisplaySize.width() + 1) >> 1));
      TopLeftPoint.setY(Point_.y() - m_DisplaySize.height());
      break;
   case BottomRight:
      TopLeftPoint.setX(Point_.x() - m_DisplaySize.width());
      TopLeftPoint.setY(Point_.y() - m_DisplaySize.height());
      break;
   case RightMiddle:
      TopLeftPoint.setX(Point_.x() - m_DisplaySize.width());
      TopLeftPoint.setY(Point_.y() - ((m_DisplaySize.height() + 1) >> 1));
      break;
   case LeftMiddle:
      TopLeftPoint.setX(Point_.x());
      TopLeftPoint.setY(Point_.y() - ((m_DisplaySize.height() + 1) >> 1));
      break;
      }
   
   return TopLeftPoint;
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TArbitraryFigure::painterPath(void)
   {                                                                              
   QPainterPath PainterPath;
   PainterPath.addRect(boundingBoxF());
   PainterPath.closeSubpath();
   return PainterPath;
   }
}
