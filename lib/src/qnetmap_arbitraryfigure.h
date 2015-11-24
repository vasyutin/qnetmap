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

#ifndef QNETMAP_FIGURE_H
#define QNETMAP_FIGURE_H

#include "qnetmap_global.h"
#include "qnetmap_geometry.h"

#include <QVariant>

namespace qnetmap
{
   //! \class TArbitraryFigure
   //! \brief Implements the class of arbitrary geometry
   class QNETMAP_EXPORT TArbitraryFigure : public TGeometry
   {
      Q_OBJECT

   public:
      typedef TGeometry TParent;
      typedef void (*TDrawFunction)(QPainter*, const QVariant&);

      //! \brief Сonstructor
      //! \param Point_ - anchor point
      //! \param DrawFigure_ - pointer to the function, which draws the figure
      //! \param DrawFunctionParameters_ - parameter of the function, which draws the figure
      //! \param Alignment_ - alignment
      //! \param QString& Name_ - name
      TArbitraryFigure(const QPointF Point_,
              const TDrawFunction DrawFigure_,
              const QVariant& DrawFunctionParameters_ = QVariant(), 
              const enum TAlignment Alignment_ = TopLeft,
              const QString& Name_ = QString());
      virtual ~TArbitraryFigure() {};

      //! \brief Method of drawing a geometric figure
      virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_);
      //! \brief Bounding box in pixel coordinates on the specified scale
      virtual QRect boundingBox(const int Zoom_ = -1);
      //! \brief Bounding box in coordinates
      virtual QRectF boundingBoxF(const int Zoom_ = -1);
      //! \brief возвращает контур рисования (для проверки пересекаемости фигур)
      virtual QPainterPath painterPath(void);

   protected:
      //! var pointer to a function draws a figure
      const TDrawFunction w_DrawFigure;
      //! var parameter of the function, which draws the figure
      QVariant m_DrawFunctionParameters;
      //! var size
      QSize m_DisplaySize;
      //! var alignment
      TAlignment m_Alignment;

   private:
      Q_DISABLE_COPY(TArbitraryFigure)

      //! \brief point, aligned with respect to the binding
      QPoint alignedPoint(const QPoint point) const;
      //! \brief set bounding box
      void setBoundingBox(void);
      //! \brief set figure display size
      void setDisplaySize(void);
      //! brief flag of the initialization bounding box
      bool m_IsBoundingBoxInitialised;
   };
}
#endif
