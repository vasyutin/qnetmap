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

#ifndef QNETMAP_CIRCLEPOINT_H
#define QNETMAP_CIRCLEPOINT_H

#include "qnetmap_global.h"
#include "qnetmap_point.h"

namespace qnetmap
{
    //! Draws a circle into the map
    /*! This is a conveniece class for Point.
     * It configures the pixmap of a Point to draw a circle.
     * A QPen could be used to change the color or line-width of the circle
     */
    class QNETMAP_EXPORT TCirclePoint : public TPoint
    {
    public:
        //!
        /*!
         * @param Point_ QPointF(longitude, latitude)
         * @param name name of the circle point
         * @param alignment alignment (Middle or TopLeft)
         * @param pen QPen for drawing
         */
        TCirclePoint(
           QPointF Point_, const QString& name = QString(), TAlignment alignment = Middle, QPen* pen=0);

        //!
        /*!
         * @param Point_ QPointF(longitude, latitude)
         * @param radius the radius of the circle
         * @param name name of the circle point
         * @param alignment alignment (Middle or TopLeft)
         * @param pen QPen for drawing
         */
        TCirclePoint(QPointF Point_, int radius = 10, const QString& name = QString(), 
                     TAlignment alignment = Middle, QPen* pen=0);
        virtual ~TCirclePoint();

        //! sets the QPen which is used for drawing the circle
        /*!
         * A QPen can be used to modify the look of the drawn circle
         * @param pen the QPen which should be used for drawing
         * @see http://doc.trolltech.com/4.3/qpen.html
         */
        virtual void setPen(QPen* pen);

    private:
        void drawCircle();
    };
}
#endif
