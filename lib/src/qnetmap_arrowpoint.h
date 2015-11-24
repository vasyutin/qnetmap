/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl ArrowPoint code by Jeffery MacEachern
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

#ifndef QNETMAP_ARROWPOINT_H
#define QNETMAP_ARROWPOINT_H

#include <QBrush>

#include <math.h>

#include "qnetmap_global.h"
#include "qnetmap_point.h"

namespace qnetmap
{
    //! Draws a directed arrow (showing orientation) into the map
    /*! This is a convenience class for Point.
     * It configures the pixmap of a Point to draw an arrow in a specific direction.
     * A QPen could be used to change the color or line-width of the arrow
     */
    class QNETMAP_EXPORT TArrowPoint : public TPoint
    {
    public:
        //!
        /*!
         * @param Point_ QPointF(longitude, latitude)
         * @param sideLength side length of the arrow's bounding box (square)
         * @param heading compass heading determining direction that arrow faces, measured in degrees clockwise from North
         * @param name name of the arrow point
         * @param alignment alignment (Middle or TopLeft)
         * @param pen QPen for drawing
         */
        TArrowPoint(QPointF Point_, int sideLength, qreal heading, 
                    const QString& name = QString(), TAlignment alignment = Middle, QPen* pen=0);
        virtual ~TArrowPoint();

        //! sets the QPen which is used for drawing the arrow
        /*!
         * A QPen can be used to modify the look of the drawn arrow
         * @param pen the QPen which should be used for drawing
         * @see http://doc.trolltech.com/4.3/qpen.html
         */
        virtual void setPen(QPen* pen);

        //! sets the heading of the arrow and redraws it in the new orientation
        /*!
         * @param heading new heading
         */
        void setHeading(qreal heading);

        //! gets the current heading of the arrow
        qreal getHeading() const;
    private:
        void drawArrow();

        // Heading
        qreal m_Heading;

        // Brush to fill the arrow with - solid colour, same as pen
        //QBrush m_Brush;
    };
}

#endif
