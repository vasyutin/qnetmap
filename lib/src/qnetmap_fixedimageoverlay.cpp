/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl FixedImageOverlay code by Kai Winter
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

#include "qnetmap_fixedimageoverlay.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   TFixedImageOverlay::TFixedImageOverlay(
      qreal x_upperleft,  qreal y_upperleft, 
      qreal x_lowerright, qreal y_lowerright, 
      const QString& filename, const QString& name)
      : TImagePoint(QPointF(x_upperleft, y_upperleft), filename, name, TopLeft),
        x_lowerright(x_lowerright), y_lowerright(y_lowerright) {}

   TFixedImageOverlay::TFixedImageOverlay(
      qreal x_upperleft,  qreal y_upperleft, 
      qreal x_lowerright, qreal y_lowerright, 
      QPixmap* pixmap, const QString& name, bool DeletePixmapWithObject_)
      : TImagePoint(QPointF(x_upperleft, y_upperleft), pixmap, name, TopLeft, DeletePixmapWithObject_),
        x_lowerright(x_lowerright), y_lowerright(y_lowerright) {}

   void TFixedImageOverlay::drawGeometry(QPainter* painter, const QRect &, const QPoint)
   {
   if (!visible() && !w_MapAdapter) return;

   if (w_Pixmap !=0) {
      QPoint topleft    = w_MapAdapter->coordinateToDisplay(coordinate());
      QPoint lowerright = w_MapAdapter->coordinateToDisplay(QPointF(x_lowerright, y_lowerright));

      painter->drawPixmap(
         topleft.x(), topleft.y(), lowerright.x()-topleft.x(), lowerright.y()-topleft.y(), *w_Pixmap);
      }
   }
}
