/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl ImagePoint code by Kai Winter
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

#ifndef QNETMAP_IMAGEPOINT_H
#define QNETMAP_IMAGEPOINT_H

#include "qnetmap_global.h"
#include "qnetmap_point.h"

namespace qnetmap
{
   //! \class TImagePoint
   //! \brief Draws an image into the map
   class QNETMAP_EXPORT TImagePoint : public TPoint
   {
   Q_OBJECT

   public:
      //! Creates a point which loads and displays the given image file
      /*!
      * Use this contructor to load the given image file and let the point display it.
      * When you want multiple points to display the same image, use the other contructor and pass a pointer to that image.
      * @param Point_ QPointF(longitude, latitude)
      * @param filename the file which should be loaded and displayed
      * @param name the name of the image point
      * @param alignment (Middle or TopLeft)
      */
      TImagePoint(QPointF Point_, const QString& filename, const QString& name = QString(), 
                  enum TAlignment alignment = Middle)
         : TPoint(Point_, new QPixmap(filename), name, alignment, true) {}
      TImagePoint(qreal X_, qreal Y_, const QString& filename, const QString& name = QString(), 
                  enum TAlignment alignment = Middle)
         : TPoint(X_, Y_, new QPixmap(filename), name, alignment, true) {}
      //! Creates a point which displays the given image
      /*!
      * Use this contructor to display the given image.
      * You have to load that image yourself, but can use it for multiple points.
      * @param Point_ QPointF(longitude, latitude)
      * @param pixmap pointer to the image pixmap
      * @param name the name of the image point
      * @param alignment (Middle or TopLeft)
      * @param DeletePixmapWithObject_ - remove the image with the object (defalt false)
      */
      TImagePoint(QPointF Point_, QPixmap* pixmap, const QString& name,
                  enum TAlignment alignment, bool DeletePixmapWithObject_ = false)
         : TPoint(Point_, pixmap, name, alignment, DeletePixmapWithObject_) {}
      TImagePoint(qreal X_, qreal Y_, QPixmap* pixmap, const QString& name = QString(), 
                  enum TAlignment alignment = Middle, bool DeletePixmapWithObject_ = false)
         : TPoint(X_, Y_, pixmap, name, alignment, DeletePixmapWithObject_) {}
      //
      virtual ~TImagePoint() {};

   private:
      Q_DISABLE_COPY(TImagePoint)
   };
}
#endif
