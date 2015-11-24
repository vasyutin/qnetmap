/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl Geometry code by Kai Winter
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

#include "qnetmap_geometry.h"
#include "qnetmap_mapadapter.h"

#include <cmath> 
#include <QPainter>

namespace qnetmap
{
   qreal TGeometry::PI = acos(-1.0);

   /////////////////////////////////////////////////////////////////////////////
   TGeometry::TGeometry(const QString& name, const QString& Description_)
      : TBase(), TBasePoint(),
        m_GeometryType("Geometry"), m_Name(name), m_Description(Description_),
        w_Pen(NULL), m_IsVisible(true), m_TransparencyLevel(255), w_MapAdapter(NULL),
        m_NameBoundingBox(), m_Zoom(-1), m_TextPosition(BottomMiddle),
		  m_IsSelected(false), m_IsFocused(false), m_IsModified(false),
        w_UserTag(NULL), m_UserString(""), m_BackPlane(false), m_OldEditMode(false),
        m_TextFontName("Arial"), m_TextFontSize(8), m_TextColor(Qt::black),
        m_IsStatic(false), m_NeedRecalcBoundingBox(true), m_NeedRecalcNameBoundingBox(true),
        m_PenStyle(Qt::SolidLine)
   {
   }

   //---------------------------------------------------------------------------
   inline void TGeometry::setVisible( bool Visible_ )
   {
   bool OldValue = m_IsVisible;
   m_IsVisible = Visible_;
   if(OldValue != m_IsVisible) update();
   }

   //---------------------------------------------------------------------------
   inline void TGeometry::setSelected( const bool Selected_, const bool SendUpdate_ /*= true*/ )
   {
      if(Selected_ != m_IsSelected) {
         m_IsSelected = Selected_;
         emit(selectedChanged(this));
         if(SendUpdate_) update();
      }
   }

   //---------------------------------------------------------------------------
   inline void TGeometry::setFocused( const bool Focused_, const bool SendUpdate_ /*= true*/ )
   {
      if(Focused_ != m_IsFocused) {
         m_IsFocused = Focused_;
         emit(focusedChanged(this));
         if(SendUpdate_) update();
      }
   }

   //---------------------------------------------------------------------------
   inline void TGeometry::setModified( const bool Modified_ /*= true*/, const bool EmitSignal_ /*= true*/ )
   {
      if(Modified_ != m_IsModified) {
         m_IsModified = Modified_;
         if(EmitSignal_) emit(modifiedChanged(this));
      }
   }

   //---------------------------------------------------------------------------
   bool TGeometry::touches(const QPointF Point_)
   {
   if(visible()) return painterPath().contains(Point_);
   return false;
   }

   //---------------------------------------------------------------------------
   bool TGeometry::touches(TGeometry* Geometry_)
   {
   if(visible() && Geometry_->visible()) return Geometry_->painterPath().intersects(painterPath());
   return false;
   }

   //---------------------------------------------------------------------------
   bool TGeometry::touches(const QRectF& RectF_)
   {
   if(visible() && !RectF_.isEmpty()) return painterPath().intersects(RectF_);
   return false;
   }

   //---------------------------------------------------------------------------
   QRect TGeometry::boundingBox( const int Zoom_ /*= -1*/ )
   {
   if(!w_MapAdapter) return QRect(QPoint(0,0), QSize(0,0));
   //
   int CurrentZoom;
   if(Zoom_ >= 0) CurrentZoom = Zoom_;
   else           CurrentZoom = w_MapAdapter->currentZoom();

   if(CurrentZoom != m_Zoom || m_OldBoundingBoxF != m_BoundingBoxF || needRecalcBoundingBox()) {
      // recompute the global coordinates
      if(Zoom_ < 0) boundingBoxF(CurrentZoom);
      // recompute the pixel coordinates
      m_BoundingBox = QRect(w_MapAdapter->coordinateToDisplay(m_BoundingBoxF.topLeft()),
                            w_MapAdapter->coordinateToDisplay(m_BoundingBoxF.bottomRight())).normalized();
      //
      m_Zoom = CurrentZoom;
      m_OldBoundingBoxF = m_BoundingBoxF;
      m_NeedRecalcNameBoundingBox = true;
      //
      setNeedRecalcBoundingBox(false);
      }
   //
   return m_BoundingBox;
   }

   // -----------------------------------------------------------------------
   QRectF TGeometry::nameBoundingBoxF(void)
   {
      if(!w_MapAdapter) return QRectF(QPoint(0,0), QSize(0,0));
      //
      QRect Rect(nameBoundingBox());
      if(Rect.isNull()) return QRectF(QPoint(0,0), QSize(0,0));
      //
      return QRectF(w_MapAdapter->displayToCoordinate(Rect.topLeft()),
                    w_MapAdapter->displayToCoordinate(Rect.bottomRight())).normalized();
   }

   // -----------------------------------------------------------------------
   QRect TGeometry::nameBoundingBox( void )
   {
      if(!w_MapAdapter) return QRect(QPoint(0,0), QSize(0,0));
      //
      if(!m_Name.isEmpty()) {
         if(w_MapAdapter->currentZoom() != m_Zoom || needRecalcBoundingBox()) {
            QImage TmpImage(QSize(2000, 200), QImage::Format_ARGB32_Premultiplied);
            QPainter Painter(&TmpImage);
            QFont Font(textFontName(), textFontSize());
            Font.setBold(false);
            Painter.setFont(Font);
            QRect RectText = Painter.boundingRect(QRect(), Qt::AlignLeft, m_Name);
            m_NameBoundingBox = QRect(topLeftTextRect(RectText.size()), RectText.size());
            m_NeedRecalcNameBoundingBox = false;
         }
         return m_NameBoundingBox;
      }
      return QRect(QPoint(0,0), QSize(0,0));
   }

   //---------------------------------------------------------------------------
   QPoint TGeometry::topLeftTextRect( const QSize& Size_ )
   {
   QPoint RetValue;
   QRect BoundingBox = boundingBox().normalized();
   //
   TAlignment TextPosition = m_TextPosition;
   // that the inscription does not overdrive the small figures change the position of the text
   if(TextPosition == Middle && (BoundingBox.height() / 3) < Size_.height()) TextPosition = BottomMiddle;
   //
   switch(TextPosition) {
   case Middle:
      RetValue = BoundingBox.center() - QPoint(Size_.width() >> 1, 0);
      break;
   case RightMiddle:
      RetValue = BoundingBox.center() + QPoint((BoundingBox.width() >> 1) + 2, -((Size_.height() + 1) >> 1));
      break;
   case BottomMiddle:
   default:
      RetValue = BoundingBox.center() + QPoint(-(Size_.width() >> 1), (BoundingBox.height() + 1) >> 1);
      break;
      }
   return RetValue;
   }

   //---------------------------------------------------------------------------
   QPainterPath TGeometry::painterPath(void)
   {
   QPainterPath PainterPath;
   PainterPath.addRect(m_BoundingBoxF);
   PainterPath.closeSubpath();
   return PainterPath;
   }

   //---------------------------------------------------------------------------
   qreal TGeometry::distance(TGeometry* Geometry_)
   {
   return distance(Geometry_->boundingBoxF().center());
   }

   //---------------------------------------------------------------------------
   qreal TGeometry::distance(const QPointF Point_)
   {
   QPointF Point = boundingBoxF().center();
   // translate to radians
   qreal c = PI / 180.0;
   qreal a1 = Point.x()  * c;
   qreal b1 = Point.y()  * c;
   qreal a2 = Point_.x() * c;
   qreal b2 = Point_.y() * c;
   // compute
   return acos(cos(a1) * cos(b1) * cos(a2) * cos(b2) + 
          cos(a1) * sin(b1) * cos(a2) * sin(b2) + 
          sin(a1) * sin(a2)) * QNetMapConsts::GoogleMapsSphereRadius;
   }

   //---------------------------------------------------------------------------
	void TGeometry::clicked(const QPointF Point_, QMouseEvent* Event_)
	{
		emit(geometryClicked(this, Point_, Event_));
	}

   //---------------------------------------------------------------------------
   void TGeometry::draw(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
      Painter_->setRenderHint(QPainter::Antialiasing);
      // halo, indicating the geometry selection
      if(selected() || focused()) drawSelection(Painter_, Viewport_, Offset_);
      // back plane, indicating the state of the geometry
      if(backPlane()) drawBackPlane(Painter_, Viewport_, Offset_);
      //
      drawGeometry(Painter_, Viewport_, Offset_);
      drawName(Painter_, Viewport_);
   }

   //---------------------------------------------------------------------------
   void TGeometry::drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
   }

	//---------------------------------------------------------------------------
	void TGeometry::drawSelection(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
	{
	}

   //---------------------------------------------------------------------------
   void TGeometry::drawBackPlane(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
   }

   //---------------------------------------------------------------------------
   void TGeometry::drawName(QPainter* Painter_, const QRect& Viewport_)
   {
   Q_UNUSED(Viewport_)

   if(name().isEmpty() || (m_GeometryType != "Point" && m_GeometryType != "Area")) return;
   //
   drawText(Painter_, m_Name);
   }

   //---------------------------------------------------------------------------
   void TGeometry::drawText(QPainter* Painter_, QString const& Text_)
   {
      QFont OldFont = Painter_->font();
      QFont Font(m_TextFontName, m_TextFontSize);
      Font.setBold(false);
      Painter_->setFont(Font);
      QRect RectText = Painter_->boundingRect(QRect(), Qt::AlignLeft, Text_);
      // upper left corner of the text
      QPoint Point = topLeftTextRect(RectText.size());
      // temporary pixmap, which draw the text
      QImage TmpImage(RectText.size() + QSize(6, 6), QImage::Format_ARGB32_Premultiplied);
      TmpImage.fill(0);
      QPainter Painter(&TmpImage);
      Painter.setCompositionMode(QPainter::CompositionMode_Source);
      //
      RectText.setWidth(RectText.width() + 2); // that do not cut the right to sign
      Painter.setFont(Font);
      // first draw a white background for better text selection
      Painter.setRenderHint(QPainter::TextAntialiasing);
      Painter.setPen(QPen(Qt::white));
      RectText.moveTopLeft(QPoint(2, 2));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      RectText.moveTopLeft(QPoint(3, 3));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      RectText.moveTopLeft(QPoint(4, 2));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      RectText.moveTopLeft(QPoint(2, 4));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      RectText.moveTopLeft(QPoint(4, 4));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      //
      blur(TmpImage);
      //
      RectText.moveTopLeft(QPoint(3, 3));
      Painter.setPen(QPen(textColor()));
      Painter.drawText(RectText, Qt::AlignLeft, Text_);
      Painter.end();
      Painter_->setFont(OldFont);
      //
      Painter_->drawImage(Point - QPoint(3, 3), TmpImage);
   }

   // -----------------------------------------------------------------------
   void TGeometry::setCoordinate(QPointF const& Point_, const bool SetModified_ /*= true*/)
   {
      if(coordinate() != Point_) {
         QRectF OldNameRect(nameBoundingBoxF()), OldBoundingBox(boundingBoxF());
         if(!OldNameRect.isEmpty()) OldBoundingBox = OldBoundingBox.united(OldNameRect);
         //
         TBasePoint::setCoordinate(Point_);
         setNeedRecalcBoundingBox();
         if(SetModified_) setModified();
         //
         QRectF NewNameRect(nameBoundingBoxF()), NewBoundingBox(boundingBoxF());
         if(!NewNameRect.isEmpty()) NewBoundingBox = NewBoundingBox.united(NewNameRect);
         //
         if(NewBoundingBox.intersects(OldBoundingBox)) emit(updateRequest(NewBoundingBox.united(OldBoundingBox)));
         else {
            emit(updateRequest(NewBoundingBox));
            emit(updateRequest(OldBoundingBox));
         }
         emit(positionChanged(this));
         if(countLinkedPoint()) emit(updateRequest());
      }
   }

   // -----------------------------------------------------------------------
   void TGeometry::update(void)
   {
      QRectF Rect(boundingBoxF()), NameRect(nameBoundingBoxF());
      if(!NameRect.isNull()) Rect = Rect.united(NameRect);
      if(!Rect.isNull()) emit(updateRequest(Rect.normalized()));
   }

   // -----------------------------------------------------------------------
   void TGeometry::setName(QString const& Name_)
   {
      QRectF Rect;
      if(m_Name != Name_) setModified();
      m_Name = Name_; 
      setNeedRecalcBoundingBox();
      Rect = nameBoundingBoxF();
      if(!Rect.isNull()) emit(updateRequest(Rect));
   }

}
