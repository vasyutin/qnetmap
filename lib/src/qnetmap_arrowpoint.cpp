/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on ArrowPoint code by Jeffery MacEachern
* Based on CirclePoint code by Kai Winter
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

#include "qnetmap_arrowpoint.h"

namespace qnetmap
{
   TArrowPoint::TArrowPoint(
      QPointF Point_, int sideLength, qreal heading, const QString& name, TAlignment alignment, QPen* pen)
      : TPoint(Point_, name, alignment)
   {
   m_Size = QSize(sideLength, sideLength);
   m_Heading = heading;
   setPen(pen);
   w_Pixmap = new QPixmap(sideLength, sideLength);
   m_DeletePixmapWithObject = true;
   drawArrow();
   }

   TArrowPoint::~TArrowPoint()
   {
   }

   void TArrowPoint::setHeading(qreal heading)
   {
   m_Heading = heading;
   drawArrow();
   }

   qreal TArrowPoint::getHeading() const
   {
   return m_Heading;
   }

   void TArrowPoint::setPen(QPen* pen)
   {
   setPen(pen);
   drawArrow();
   }

   void TArrowPoint::drawArrow()
   {
   w_Pixmap->fill(Qt::transparent);
   QPainter painter(w_Pixmap);
   //#if !defined Q_WS_MAEMO_5  //FIXME Maemo has a bug - it will antialias our point out of existence
   painter.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);
   //#endif

   if(pen()) {
      painter.setPen(*pen());
      painter.setBrush(QBrush(pen()->color()));
      }
   else {
      painter.setBrush(QBrush(painter.pen().color()));
      }

   painter.setWindow(-(m_Size.width() / 2), -(m_Size.height() / 2), m_Size.width(), m_Size.height());
   QTransform transform;
   transform.rotate(-m_Heading);
   transform.scale(0.4, 0.75);
   painter.setWorldTransform(transform);

   QPolygon arrow;
   arrow << QPoint(0, -(m_Size.height() / 2));
   arrow << QPoint(-(m_Size.width() / 2), +(m_Size.height() / 2));
   arrow << QPoint(0, 0);
   arrow << QPoint(+(m_Size.width() / 2), +(m_Size.height() / 2));

   painter.drawPolygon(arrow);
   }
}
