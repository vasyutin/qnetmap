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

#ifndef QNETMAP_GEOMETRY_H
#define QNETMAP_GEOMETRY_H

#include "qnetmap_global.h"
#include "qnetmap_basepoint.h"

#include <QRect>
#include <QRectF>
#include <QRgb>
#include <QColor>

class QMouseEvent;
class QPainter;
class QPen;
class QPoint;
class QPointF;
class QPainter;
class QPainterPath;
class QImage;
class QSize;

namespace qnetmap
{
   class TMapAdapter;
   class TLayer;

   //! \class TGeometry
   //! \brief Main class for objects that should be painted in maps
   class QNETMAP_EXPORT TGeometry : public TBase, public TBasePoint
   {
      Q_OBJECT

      friend class TBasePoint;

   public:
      explicit TGeometry(const QString& name = QString(), const QString& Description_ = QString());
      virtual ~TGeometry() { emit(geometryDeleted(this)); }

      //!
      /*! returns true if the given TGeometry is equal to this TGeometry
      *  not implemented yet!
      * @param Geometry_ The TGeometry to be tested
      * @return true if the given TGeometry is equal to this
      */
      //bool Equals(TGeometry* Geometry_);

      //! returns a String representation of this TGeometry
      /*!
      * not implemented yet!
      * @return a String representation of this TGeometry
      */
      //QString toString();

      //! \brief return the name of the geometry
      virtual const QString& name(void) const { return m_Name; }
      //! \brief sets the name of the geometry
      //! \param Name_ the new name of the geometry
      virtual void setName(QString const& Name_);
      //! \brief return the description of the geometry
      virtual const QString& description(void) const { return m_Description; }
      //! \brief sets the description of the geometry
      virtual void setDescription(const QString& Description_) { m_Description = Description_; }
      //! \brief returns the pointer to the adapter card, which draws geometry
      inline TMapAdapter* mapAdapter(void) const { return w_MapAdapter; }
      //! \brief sets the pointer to the adapter card, which draws geometry
      inline void setMapAdapter(TMapAdapter* MapAdapter_) { w_MapAdapter = MapAdapter_; }
      //! \brief the level of transparency (alpha-channel) the geometry
      inline unsigned char transparencyLevel(void) const { return m_TransparencyLevel; }
      //! \brief sets the level of transparency (alpha-channel) the geometry
      inline void setTransparencyLevel(const unsigned char TransparencyLevel_) { m_TransparencyLevel = TransparencyLevel_; update(); }
      //! \brief returns the QPen which is used on drawing
      inline QPen* pen(void) const { return w_Pen; }
      //! \brief set the QPen which is used on drawing
      inline void setPen(QPen* Pen_) { w_Pen = Pen_; }
      //! \brief Layer containing this element
      inline TLayer* layer(void) { return w_Layer; }
      //! \brief 
      inline void setLayer(TLayer* Layer_) { w_Layer = Layer_; }
      //! \brief selection state
      inline bool selected(void) const { return m_IsSelected; }
      //! \brief if Selected_ is true, the geometry is made selected
      inline void setSelected(const bool Selected_, const bool SendUpdate_ = true);
      //! \brief focused state
      inline bool focused(void) const { return m_IsFocused; }
      //! \brief if Focused_ is true, the geometry is made focused
      inline void setFocused(const bool Focused_, const bool SendUpdate_ = true);
      //! \brief modified state
      inline bool modified(void) const { return m_IsModified; }
      //! \brief if Modified_ is true, the geometry is made modified
      inline void setModified(const bool Modified_ = true, const bool EmitSignal_ = true);
      //! \brief visibility
      inline bool visible(void) const { return m_IsVisible; }
		/*obsolete*/ inline bool isVisible(void) const { return m_IsVisible; }

      //! \brief Must be call when geometry is clicked
      virtual void clicked(const QPointF Point_, QMouseEvent* Event_);
      //! \brief finding a point inside a figure
      virtual bool touches(const QPointF Point_);
      //! \brief intersection with the other figures
      virtual bool touches(TGeometry* Geometry_);
      //! \brief intersection with the rectangle
      virtual bool touches(const QRectF& RectF_);
      //! \brief calculates the geographic distance in meters to a point on the Google sphere
      virtual qreal distance(const QPointF Point_);
      //! \brief calculates the geographic distance in meters to the other figures on the Google sphere
      virtual qreal distance(TGeometry* Geometry_);
      //! \brief figure type
      inline const QString& geometryType(void) const { return m_GeometryType; }
      //! \brief returns outline drawing (to check touches figures)
      virtual QPainterPath painterPath(void);
      //! \brief surrounding rectangle in geographical coordinates
      virtual QRectF boundingBoxF(const int Zoom_ = -1) = 0;
      //! \brief surrounding rectangle in pixel coordinates on a given scale
      virtual QRect boundingBox(const int Zoom_ = -1);
      //! \brief draws the object
      virtual void draw(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
      //! \brief draws the geometry
      virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
      //! \brief draws the halo, indicating the geometry selection
      virtual void drawSelection(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
      //! \brief draws the back plane, indicating the state of the geometry
      virtual void drawBackPlane(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
      //! \brief draws the name of the object on the map
      virtual void drawName(QPainter* Painter_, const QRect& Viewport_);
      //! \brief 
      virtual void update(void);
      //! \brief 
      inline void* userTag(void) const { return w_UserTag; }
      //! \brief 
      inline void setUserTag(void* Tag_) { w_UserTag = Tag_; }
      //! \brief 
      inline const QString& userString(void) { return m_UserString; }
      //! \brief 
      inline void setUserString(const QString& String_) { m_UserString = String_; }
      //! \brief 
      inline bool backPlane(void) const { return m_BackPlane; }
      //! \brief 
      inline void setBackPlane(const bool BackPlane_ = true) { m_BackPlane = BackPlane_; }
      //! \brief 
      inline const QString& textFontName(void) const { return m_TextFontName; }
      //! \brief 
      inline void setTextFontName(const QString& FontName_) { m_TextFontName = FontName_; }
      //! \brief 
      inline unsigned textFontSize(void) const { return m_TextFontSize; }
      //! \brief 
      inline void setTextFontSize(const unsigned FontSize_) { m_TextFontSize = FontSize_; }
      //! \brief 
      inline QColor textColor(void) const { return m_TextColor; }
      //! \brief 
      inline void setTextColor(const QColor& Color_) { m_TextColor = Color_; }
      //! \brief 
      inline bool isStatic(void) const { return m_IsStatic; }
      //! \brief 
      inline void setStatic(const bool Static_) { m_IsStatic = Static_; }
      //! \brief returns the description color for tooltip
      virtual QRgb descriptionColor(void) const { return QRgb(0xff000000); }
      //! \brief 
      virtual QRect nameBoundingBox(void);
      //! \brief 
      virtual QRectF nameBoundingBoxF(void);
      //! \brief 
      inline Qt::PenStyle penStyle(void) const { return m_PenStyle; }
      //! \brief 
      inline void setPenStyle(Qt::PenStyle PenStyle_) { m_PenStyle = PenStyle_; }

      //! \var number PI
      static qreal PI;

   private:
      Q_DISABLE_COPY(TGeometry)

      //! \var geometry type
      QString  m_GeometryType;
      //! \var geometry name
      QString  m_Name;
      //! \var geometry description
      QString  m_Description;
      //! \var QPen which is used on drawing
      QPen*    w_Pen;
      //! \var geometry visibility
      bool     m_IsVisible;
	   //! \var selection state
	   bool	   m_IsSelected;
      //! \var focused state
      bool	   m_IsFocused;
      //! \var modified state
      bool	   m_IsModified;
      //! \var transparency level
      unsigned char m_TransparencyLevel;
      //! \var user tag
      void*	   w_UserTag;
      //! \var user string
      QString  m_UserString;
      //! \var 
      bool     m_BackPlane;
      //! \var 
      QString  m_TextFontName;
      //! \var 
      unsigned m_TextFontSize;
      //! \var 
      QColor   m_TextColor;
      //! \var 
      bool     m_IsStatic;
      //! \var 
      bool     m_NeedRecalcBoundingBox;
      //! \var 
      bool     m_NeedRecalcNameBoundingBox;
      //! \var 
      Qt::PenStyle m_PenStyle;

   protected:
      //! \brief устанавливает тип фигуры
      inline void setGeometryType(const QString Type_) { m_GeometryType = Type_; }
      //! \brief top left corner of the rectangle surrounding the text
      //! \param QSize& Size_ - size of the rectangle surrounding the text
      virtual QPoint topLeftTextRect(const QSize& Size_);
      //! \brief 
      virtual void drawText(QPainter* Painter_, QString const& Text_);
      //! \brief 
      inline bool needRecalcBoundingBox(void) const { return m_NeedRecalcBoundingBox; }
      //! \brief 
      inline void setNeedRecalcBoundingBox(const bool NeedRecalcBoundingBox_ = true) { m_NeedRecalcBoundingBox = NeedRecalcBoundingBox_; }

      //! \var указатель на адаптер карты, на которой рисуется фигура
      TMapAdapter* w_MapAdapter;
      //! \var окружающий прямоугольник в географических координатах
      QRectF m_BoundingBoxF;
      //! \var окружающий прямоугольник в географических координатах,
      //!      сохраняется методом boundingBox для своих нужд
      QRectF m_OldBoundingBoxF;
      //! \var окружающий прямоугольник имени в пиксельных координатах
      QRect m_NameBoundingBox;
      //! \var окружающий прямоугольник в пиксельных координатах
      QRect m_BoundingBox;
      //! \var текущий масштаб 
      int m_Zoom;
      //! \var позиция надписи относительно фигуры
      TAlignment m_TextPosition;
      //! \var Layer containing this element
      TLayer* w_Layer;
      //! \var 
      bool m_OldEditMode;

   signals:
      void updateRequest(TGeometry*);
      void updateRequest(QRectF);
      void updateRequest(void);
      void geometryDeleted(TGeometry*);
      //! This signal is emitted when a TGeometry is clicked
      /*!
      * A Geometry is clickable, if the containing layer is clickable.
      * The objects emits a signal if it gets clicked
      * @param  geometry The clicked TGeometry
      * @param  point -unused-
      */
      void geometryClicked(TGeometry* Geometry_, QPointF Point_, QMouseEvent* Event_);

      //! A Geometry emits this signal, when its position gets changed
      /*!
      * @param the TGeometry
      */
      void positionChanged(TGeometry*);
      //! \brief A Geometry emits this signal, when its selected state gets changed
      void selectedChanged(TGeometry*);
      //! \brief A Geometry emits this signal, when its focused state gets changed
      void focusedChanged(TGeometry*);
      //! \brief A Geometry emits this signal, when its modified state gets changed
      void modifiedChanged(TGeometry*);

   public slots:
      //! \brief set the point anchor
      void setCoordinate(QPointF const& Point_, const bool SetModified_ = true);
      //! \brief if Visible_ is true, the geometry is made visible
      inline void setVisible(bool Visible_);
   };
}
#endif
