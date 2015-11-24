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

#ifndef QNETMAP_POINT_H
#define QNETMAP_POINT_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QWidget>
#endif

#include "qnetmap_geometry.h"

namespace qnetmap
{
   class TPoint;

   typedef QList<TPoint*> TPointList;
   typedef QListIterator<TPoint*> TPointListIterator;

   //! \class TPoint
   //! \brief A geometric point to draw objects into maps
   class QNETMAP_EXPORT TPoint : public TGeometry
   {
      Q_OBJECT

   public:
      typedef TGeometry TParent;

      friend class TLayer;
      friend class TLineString;

      enum TPointType {
         Point,
         Image,
         Widget
         };

      //! Copy Constructor
      //explicit TPoint(const TPoint&);
      /*!
      * This constructor creates a TPoint with no image or widget.
      * @param Point_ QPointF(longitude, latitude)
      * @param Name_ name of the point
      * @param Color_ color of the point
      * @param Width_ thickness of the lines point
      */
      TPoint(const QPointF Point_, const QString Name_ = QString(), 
             const unsigned Color_ = 0, const unsigned Thickness_ = 0);
      TPoint(const qreal X_, const qreal Y_, const QString Name_ = QString(), 
             const unsigned Color_ = 0, const unsigned Thickness_ = 0);
      /*!
      * This constructor creates a point which will display the given widget.
      * You can set an alignment on which corner the widget should be aligned to the coordinate.
      * You have to set the size of the widget, before adding it to
      * IMPORTANT: You have to set the QMapControl as parent for the widget!
      * @param Point_ QPointF(longitude, latitude)
      * @param widget the widget which should be displayed by this point
      * @param Name_ name of the point
      * @param alignment allignment of the point (Middle or TopLeft)
      * @param DeleteWidgetWithObject_ - remove the widget with the object (defalt false)
      */
      TPoint(const QPointF Point_, QWidget* widget, const QString Name_ = QString(),
             const enum TAlignment alignment = Middle, const bool DeleteWidgetWithObject_ = false);
      TPoint(const qreal X_, const qreal Y_, QWidget* widget, const QString Name_ = QString(),
             const enum TAlignment alignment = Middle, const bool DeleteWidgetWithObject_ = false);
      /*!
      * This constructor creates a point which will display the give pixmap.
      * You can set an alignment on which corner the pixmap should be aligned to the coordinate.
      * @param Point_ QPointF(longitude, latitude)
      * @param pixmap the pixmap which should be displayed by this point
      * @param Name_ name of the point
      * @param alignment allignment of the point (Middle or TopLeft)
      * @param DeletePixmapWithObject_ - remove the image with the object (defalt false)
      */
      TPoint(const QPointF Point_, QPixmap* pixmap, const QString Name_ = QString(),
             const enum TAlignment alignment = Middle, const bool DeletePixmapWithObject_ = false);
      TPoint(const qreal X_, const qreal Y_, QPixmap* pixmap, const QString Name_ = QString(),
             const enum TAlignment alignment = Middle, const bool DeletePixmapWithObject_ = false);
      //
      virtual ~TPoint();

      //! \brief returns the bounding box of the point
      /*!
      * The Bounding contains the coordinate of the point and its size.
      * The size is set, if the point contains a pixmap or a widget
      * @return the bounding box of the point
      */
      virtual QRectF boundingBoxF(const int Zoom_ = -1);

//       //! \brief returns the longitude of the point
//       /*!
//       * @return the longitude of the point
//       */
//       qreal longitude() const;
// 
//       //! \brief returns the latitude of the point
//       /*!
//       * @return the latitude of the point
//       */
//       qreal latitude() const;

      //! \brief returns the coordinate of the point
      /*!
      * The x component of the returned QPointF is the longitude value,
      * the y component the latitude
      * @return the coordinate of a point
      */
      //QPointF coordinate() const;

      virtual TPointList points();

      //! \brief returns the widget of the point
      /*
      @return the widget of the point
      */
      QWidget* widget();

      //! \brief returns the pixmap of the point
      /*!
      * @return the pixmap of the point
      */
      QPixmap* pixmap();

      //! \brief Sets the zoom level on which the point's pixmap gets displayed on full size
      /*!
      * Use this method to set a zoom level on which the pixmap gets displayed with its real size.
      * On zoomlevels below it will be displayed smaller, and on zoom levels thereover it will be displayed larger
      * @see setMinsize, setMaxsize
      * @param zoomlevel the zoomlevel on which the point will be displayed on full size
      */
      void setBaselevel(int zoomlevel);

      ///! \brief sets a minimal size for the pixmap
      /*!
      * When the point's pixmap should change its size on zooming, this method sets the minimal size.
      * @see setBaselevel
      * @param minsize the minimal size which the pixmap should have
      */
      void setMinsize(QSize minsize);

      //! sets a maximal size for the pixmap
      /*!
      * When the point?s pixmap should change its size on zooming, this method sets the maximal size.
      * @see setBaselevel
      * @param maxsize the maximal size which the pixmap should have
      */
      void setMaxsize(QSize maxsize);

      //! \brief returns the alignment
      TAlignment alignment() const;
      //! \brief returns the size displayed on the screen
      virtual QSize displaySize(void) const { return m_DisplaySize; }
      //! \brief Set new pixmap
      virtual bool setPixmap(QPixmap* Pixmap_);

   private:
      TPoint() {};
      Q_DISABLE_COPY(TPoint)

      //! \brief set the point parameters
      void setPointParameters(void);
      //! \brief set the image parameters
      void setImageParameters(const QPixmap* Pixmap_);
      //! \brief set the widget parameters
      void setWidgetParameters(const QWidget* Widget_);
      //! \brief calculates the size displayed on the screen
      void calcDisplaySize(void);

   protected:
      //! var size
      QSize m_Size;

      //! var the pointer to widget
      QWidget* w_Widget;
      //! var attribute you want to delete the widget with the object
      bool m_DeleteWidgetWithObject;
      //! var the pointer to picture
      QPixmap* w_Pixmap;
      //! var attribute you want to delete the picture with the object
      bool m_DeletePixmapWithObject;
      //! var the point alignment
      TAlignment m_Alignment;
      //! var 
      int m_HomeLevel;
      //! var the size displayed on the screen
      mutable QSize m_DisplaySize;
      //! var the minimum size
      QSize m_Minsize;
      //! var the maximum size
      QSize m_Maxsize;
      //! var the point type
      TPointType m_Type;
      //! var the point color
      QColor m_Color;
      //! var the line thickness for point
      unsigned m_Thickness;

      //! \brief draw the geometry
      virtual void drawGeometry(QPainter* painter, const QRect &viewport, const QPoint offset);
      //! \brief draw the widget
      void drawWidget(const QPoint offset);
      //! \brief the top left point, aligned with respect to the anchor point
      QPoint alignedPoint(const QPoint Point_, QSize Size_ = QSize()) const;
      //! \brief the middle point, aligned with respect to the anchor point
      QPoint middlePoint(void) const;

   public slots:
      //! \brief set visibility
      virtual void setVisible(bool visible);
      };
}
#endif
