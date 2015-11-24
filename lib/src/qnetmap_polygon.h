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

#ifndef QNETMAP_POLYGON_H
#define QNETMAP_POLYGON_H

#include "qnetmap_global.h"
#include "qnetmap_geometry.h"

#include <QPolygonF>
#include <QColor>

namespace qnetmap
{
   //! \class TPolygon
   //! \brief реализует класс многоугольника
   class QNETMAP_EXPORT TPolygon : public TGeometry
   {
      Q_OBJECT

   public:
      typedef QVector<QPointF> TPointList;
      typedef TGeometry TParent;

      //! \brief Конструктор
      //! \param TPointList &Points_ - список вершин
      //! \param QString& Name_ - название
      //! \param unsigned FillColor_ - заполняющий цвет
      //! \param unsigned BorderColor_ - цвет бордюра
      //! \param unsigned BorderWidth_ - ширина бордюра
      explicit TPolygon(const TPointList& Points_, const QString& Name_ = QString(),
                        const unsigned FillColor_ = 0, const unsigned BorderColor_ = 0, 
                        const unsigned BorderWidth_ = 0);
      virtual ~TPolygon() {};

      //! \brief Возвращает список вершин многоугольника
      virtual QPolygonF vertices(void) const;
      //! \brief Добавляет вершину в конец списка вершин
      virtual void addVertex(const QPointF& PointF_);
      //! \brief Устанавливает список вершин
      virtual void setVertices(const TPointList& Points_);
      //! \brief метод отрисовки многоугольника
      virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_);
      //! \brief окружающий прямоугольник в географических координатах
      virtual QRectF boundingBoxF(const int Zoom_ = -1);
      //! \brief нахождение точки внутри многоугольника
      virtual bool touches(const QPointF Point_);
      //! \brief возвращает контур рисования (для проверки пересекаемости фигур)
      virtual QPainterPath painterPath(void) const;

   protected:
      TPolygon();

   private:
      Q_DISABLE_COPY(TPolygon)

      //! \var список вершин 
      QPolygonF m_Vertices;
      //! \var заполняющий цвет
      QColor m_FillColor;
      //! \var цвет бордюра
      QColor m_BorderColor;
      //! \var ширина бордюра в пикселах
      unsigned m_BorderWidth;
      //! \var многоугольник в локальных координатах
      QPolygon m_LocalPolygon;
      //! \var смещение локальных координат
      QPoint m_TopLeft;
      //! \var размер описанного прямоугольника
      QSize m_PolygonRectSize;
      //! \var признак изменения количества вершин
      bool m_VerticesChanged;
      };
}
#endif
