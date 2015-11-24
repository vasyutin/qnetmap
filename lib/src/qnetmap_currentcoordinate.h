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

#ifndef QNETMAP_CURRENTCOORDINATE_H
#define QNETMAP_CURRENTCOORDINATE_H


#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QLabel>
#endif

namespace qnetmap
{
   class TMapControl;

   //! \class TCurrentCoordinate
   //! \brief Виджет, отображающий текущие географические координаты под курсором мыши.
   //!        Его достаточно только поместить в нужное место.
   //!        Виджет сам ловит необходимые сигналы и отображает координаты.
   class QNETMAP_EXPORT TCurrentCoordinate : public QLabel
   {
      Q_OBJECT

   public:
      //! \struct Consts
      //! \brief Константы типов отображения координат
      struct Consts {
         static const int DecimalCoordinates   = 1;  //!< Десятичный вид
         static const int DegreeCoordinates    = 2;  //!< Вид в градусах
         static const int DegreeCoordinatesLat = 3;  //!< Вид в градусах с латинским обозначением широты и долготы
         static const int PixelCoordinates     = 4;  //!< Вид в пикселах
         static const int TileCoordinates      = 5;  //!< Вид в тайлах
         static const int LongitudeLatitude    = 10; //!< Порядок отображения Долгота Широта
         static const int LatitudeLongitude    = 11; //!< Порядок отображения Широта Долгота
      };

      explicit TCurrentCoordinate(TMapControl* MapControl_,
                         const QString& StyleSheet_ = QString(), 
                         QWidget *parent = 0);
      virtual ~TCurrentCoordinate() {}

      //! \brief Изменяет тип отображения координат
      //! \param int View_ - тип отображения (см. Consts)
      static void setView(const int View_);
      static int view(void) { return m_View; }
      //! \brief Изменяет порядок отображения координат
      //! \param int Order_ - порядок отображения (см. Consts)
      static void setOrder(const int Order_);
      static int order(void) { return m_Order; }
      //! \brief возвращает строку с координатами
      //! \param QPointF Point_ - координаты географические
      //! \return QString - строка с координатами указанного вида
      static QString textCoordinates(const QPointF Point_, TMapControl* MapControl_);

   private:
      Q_DISABLE_COPY(TCurrentCoordinate)

      //! \var тип отображения координат
      static int m_View;
      //! \var порядок отображения координат
      static int m_Order;
      //! \var указатель на управляющий класс карты
      TMapControl* w_MapControl;
      //! \var курсор до попадания на виджет
      QCursor m_OldCursor;

   public slots:
      //! \brief Слот обработки сигналов изменения координат
      //! \param TMapControl* MapControl_ - указатель на управляющий класс карты
      //! \param QPointF Point_ - текущие географические координаты
      void coordinateChangeg(const TMapControl* MapControl_, const QPointF Point_);

   protected:
      void mousePressEvent(QMouseEvent* Event_);
      void mouseReleaseEvent(QMouseEvent* Event_);
      void enterEvent(QEvent* Event_);
      void leaveEvent(QEvent* Event_);
   };
}
#endif
