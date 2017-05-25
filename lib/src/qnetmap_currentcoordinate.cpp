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

#include "qnetmap_currentcoordinate.h"
#include "qnetmap_mapcontrol.h"
#include "qnetmap_layermanager.h"
#include "qnetmap_layer.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   int TCurrentCoordinate::m_View  = TCurrentCoordinate::Consts::DecimalCoordinates;
   int TCurrentCoordinate::m_Order = TCurrentCoordinate::Consts::LongitudeLatitude;

   //----------------------------------------------------------------------
   TCurrentCoordinate::TCurrentCoordinate(TMapControl* MapControl_,
                                          const QString& StyleSheet_, QWidget *Parent_) 
      : QLabel(Parent_), w_MapControl(MapControl_)
   {
   setFrameStyle(QFrame::Plain | QFrame::Box);
   setLineWidth(0);
   if(!StyleSheet_.isEmpty()) setStyleSheet(StyleSheet_);
   hide();
   setToolTip(QNetMapTranslator::tr("To switch the view, click the left mouse button"
		/* Russian: Для переключения вида щелкните левой кнопкой мыши */));

   // соединяем с источником сигналов
   QNM_DEBUG_CHECK(connect(MapControl_, SIGNAL(mouseMoveEventCoordinate(const TMapControl*, const QPointF)),
                  this,          SLOT(coordinateChangeg(const TMapControl*, const QPointF))));
   }

   //----------------------------------------------------------------------
   QString TCurrentCoordinate::textCoordinates(const QPointF Point_, TMapControl* MapControl_)
   {
   if(m_Order != Consts::LatitudeLongitude && m_Order != Consts::LongitudeLatitude) return QString();
   //
   QString RetText;
   QString SignX = (Point_.x() < 0.0) ? "-" : "";
   QString SignY = (Point_.y() < 0.0) ? "-" : "";
   qreal x = std::abs(Point_.x()), y = std::abs(Point_.y());
   int g1, g2, m1, m2;
   g1 = int(x); x = (x - g1) * 60;
   m1 = int(x); x = (x - m1) * 60;
   g2 = int(y); y = (y - g2) * 60;
   m2 = int(y); y = (y - m2) * 60;
   x = int(x * 1000) / 1000.;
   y = int(y * 1000) / 1000.;
   //
   QString Template;
   switch(m_View) {
   case Consts::DecimalCoordinates: {
      Template = m_Order == Consts::LongitudeLatitude ? 
         QString(" %1, %2 "): QString(" %2, %1 ");
      RetText = Template.arg(Point_.x(), 0, 'g', 8).arg(Point_.y(), 0, 'g', 8);
      break;
      }
   case Consts::DegreeCoordinates:
      Template = m_Order == Consts::LongitudeLatitude ? 
         QString(" SIGNX%1") + QChar(0xB0) + QString(" %2' %3\", SIGNY%4") + QChar(0xB0) + QString(" %5' %6\" "): 
         QString(" SIGNY%4") + QChar(0xB0) + QString(" %5' %6\", SIGNX%1") + QChar(0xB0) + QString(" %2' %3\" ");
      RetText = Template.arg(g1, 1).arg(m1, 1).arg(x, 0, 'g', 5).arg(g2, 1).arg(m2, 1).arg(y, 0, 'g', 5);
      RetText.replace("SIGNX", SignX);
      RetText.replace("SIGNY", SignY);
      break;
   case Consts::DegreeCoordinatesLat: {
      Template = m_Order == Consts::LongitudeLatitude ? 
         QString(" %1") + QChar(0xB0) + QString(" %2' %3\" %4, %5") + QChar(0xB0) + QString(" %6' %7\" %8 "):
         QString(" %5") + QChar(0xB0) + QString(" %6' %7\" %8, %1") + QChar(0xB0) + QString(" %2' %3\" %4 ");
			QString EW = Point_.x() >= 0 ? QNetMapTranslator::tr("E") : QNetMapTranslator::tr("W");
      QString NS = Point_.y() >= 0 ? QNetMapTranslator::tr("N") : QNetMapTranslator::tr("S");
      RetText = Template.arg(std::abs(g1)).arg(std::abs(m1)).arg((qreal)std::abs(x), 0, 'g', 5).arg(EW)
                        .arg(std::abs(g2)).arg(std::abs(m2)).arg((qreal)std::abs(y), 0, 'g', 5).arg(NS);
      break;
      }
   case Consts::PixelCoordinates: {
      Template = QString(" x=%1, y=%2, z=%3 ");
      RetText = Template.arg(MapControl_->layerManager()->layer()->mapadapter()->coordinateToDisplay(Point_).x())
                        .arg(MapControl_->layerManager()->layer()->mapadapter()->coordinateToDisplay(Point_).y())
                        .arg(MapControl_->currentZoom());
      break;
      }
   case Consts::TileCoordinates: {
      Template = QString(" x=%1, y=%2, z=%3 ");
      RetText = Template.arg(MapControl_->layerManager()->layer()->mapadapter()->coordinateToTile(Point_).x())
                        .arg(MapControl_->layerManager()->layer()->mapadapter()->coordinateToTile(Point_).y())
                        .arg(MapControl_->currentZoom());
      break;
      }
      }
   return RetText;
   }

   //----------------------------------------------------------------------
   void TCurrentCoordinate::coordinateChangeg(const TMapControl* MapControl_, const QPointF Point_)
   {
   if(w_MapControl == MapControl_ && !isHidden()) setText(textCoordinates(Point_, w_MapControl));
   }
   
   //----------------------------------------------------------------------
   void TCurrentCoordinate::setView(const int View_)
   {
   switch(View_) {
   case Consts::DecimalCoordinates:
   case Consts::DegreeCoordinates:
   //case Consts::DegreeCoordinatesRus:
   case Consts::DegreeCoordinatesLat:
   case Consts::PixelCoordinates:
   case Consts::TileCoordinates:
      m_View = View_;
      break;
      }
   }
   
   //----------------------------------------------------------------------
   void TCurrentCoordinate::setOrder(const int Order_)
   {
   switch(Order_) {
   case Consts::LongitudeLatitude:
   case Consts::LatitudeLongitude:
      m_Order = Order_;
      break;
      }
   }

   //----------------------------------------------------------------------
   void TCurrentCoordinate::mousePressEvent(QMouseEvent* Event_)
   {
   switch(Event_->button()) {
      case Qt::LeftButton:
         m_View++;
         if(m_View > Consts::TileCoordinates) m_View = Consts::DecimalCoordinates;
         break;
      default:
         break;
      }
   Event_->accept();
   }

   //----------------------------------------------------------------------
   void TCurrentCoordinate::mouseReleaseEvent(QMouseEvent* Event_)
   {
   Event_->accept();
   }

   //----------------------------------------------------------------------
   void TCurrentCoordinate::enterEvent(QEvent* Event_)
   {
   m_OldCursor = cursor();
   setCursor(QCursor(Qt::ArrowCursor));
   Event_->accept();
   }

   //----------------------------------------------------------------------
   void TCurrentCoordinate::leaveEvent(QEvent* Event_)
   {
   setCursor(m_OldCursor);
   Event_->accept();
   }
}
