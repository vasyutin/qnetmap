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

#ifndef QNETMAP_SHAPE_H
#define QNETMAP_SHAPE_H

#include "qnetmap_global.h"
#include "qnetmap_geometry.h"

#include <QColor>

namespace qnetmap
{
   //! \class TShape
   //! \brief Реализует класс геометрической фигуры
   class QNETMAP_EXPORT TShape : public TGeometry
   {
      Q_OBJECT

   public:
      typedef TGeometry TParent;

      enum TShapeType {
         Rectangle,     // Прямоугольник
         Ellipse        // Эллипс
         };

      //! \brief Конструктор
      //! \param enum TShapeType ShapeType_ - тип геометрической фигуры
      //! \param QRectF &Rect_ - окружающий прямоугольник
      //! \param QString& Name_ - название
      //! \param unsigned FillColor_ - заполняющий цвет
      //! \param unsigned BorderColor_ - цвет бордюра
      //! \param unsigned BorderWidth_ - ширина бордюра
      TShape(const enum TShapeType ShapeType_, const QRectF &Rect_, const QString& Name_ = QString(),
         const unsigned FillColor_ = 0, const unsigned BorderColor_ = 0, const unsigned BorderWidth_ = 0);
      virtual ~TShape() {};

      //! \brief метод отрисовки геометрической фигуры
      virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_);
      //! \brief окружающий прямоугольник в географических координатах
      virtual QRectF boundingBoxF(const int Zoom_ = -1);
      //! \brief возвращает контур рисования (для проверки пересекаемости фигур)
      virtual QPainterPath painterPath(void);

   protected:
      TShape();

   private:
      Q_DISABLE_COPY(TShape)

      //! \var тип геометрической фигуры
      TShapeType m_ShapeType;
      //! \var заполняющий цвет
      QColor m_FillColor;
      //! \var цвет бордюра
      QColor m_BorderColor;
      //! \var ширина бордюра
      unsigned m_BorderWidth;
   };
}
#endif
