/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl CirclePoint code by Kai Winter
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

#include "qnetmap_circlepoint.h"

namespace qnetmap
{
    TCirclePoint::TCirclePoint(QPointF Point_, int radius, const QString& name, TAlignment alignment, QPen* pen)
            : TPoint(Point_, name, alignment)
    {
        m_Size = QSize(radius, radius);
        setPen(pen);
        w_Pixmap = new QPixmap(radius+1, radius+1);
        m_DeletePixmapWithObject = true;
        drawCircle();
    }

    TCirclePoint::TCirclePoint(QPointF Point_, const QString& name, TAlignment alignment, QPen* pen)
            : TPoint(Point_, name, alignment)
    {
        int radius = 10;
        m_Size = QSize(radius, radius);
        setPen(pen);
        w_Pixmap = new QPixmap(radius+1, radius+1);
        m_DeletePixmapWithObject = true;
        drawCircle();
    }

    TCirclePoint::~TCirclePoint()
    {
    }

    void TCirclePoint::setPen(QPen* pen)
    {
        setPen(pen);
        drawCircle();
    }

    void TCirclePoint::drawCircle()
    {
        w_Pixmap->fill(Qt::transparent);
        QPainter painter(w_Pixmap);
//#if !defined Q_WS_MAEMO_5 //FIXME Maemo has a bug - it will antialias our point out of existence
        painter.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);
//#endif
        if (pen()) painter.setPen(*pen());
        painter.drawEllipse(0,0, m_Size.width(), m_Size.height());
    }
}
