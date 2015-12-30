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

#ifndef QNETMAP_BASE_H
#define QNETMAP_BASE_H

#include "qnetmap_global.h"

#include <QObject>
#include <QMutex>
#include <QPoint>
#include <QSize>
#include <QRgb>
#include <QImage>

#include <string>

namespace qnetmap
{
   //! \class The base class for all library
   class QNETMAP_EXPORT TBase: public QObject
   {
      Q_OBJECT

   public:
      //! \struct Consts
      //! \brief actions for handling messages
      struct Consts {
         static const int ShowMessage   = 1; //!< Show message
         static const int ToLog         = 2; //!< Save the log (this will add the date and time)
         static const int ShowClassName = 4; //!< Display name of the class
         };

      explicit TBase(QObject* Parent_ = NULL) : QObject(Parent_) {}
      virtual ~TBase() {}

      //! \brief displays the message and stores it in a log file
      //! \param QString Message_ message
      //! \param int Flags_ - flags (см. Consts)
      //! \param QObject* Sender_ - a pointer to the class that generates the message
      static void qnetmaplog(const QString& Message_, QObject* Sender_ = NULL, int Flags_ = Consts::ToLog | Consts::ShowClassName);
      //! \brief the top left point, aligned with respect to the anchor point
      static QPoint alignedPoint(const QPoint& Point_, const QSize& Size_, const TAlignment Alignment_);
      //! \brief вычисляет цвет и прозрачность среднего пиксела для алгоритма размытия рисунка
      //! \param QRgb* j - указатель на первый пиксел
      //! \param QRgb* n - указатель на второй (средний) пиксел
      //! \param QRgb* k - указатель на третий пиксел
      static void setColorMiddlePixel(const QRgb* j, QRgb* n, const QRgb* k);
      //! \brief blur the picture
      static void blur(QImage& Image_);
      //! \brief
      static QAtomicInt& middlePointAtomicInt(void) { return m_MiddlePointAtomicInt; }

   protected:
      static QAtomicInt m_MiddlePointAtomicInt;

   private:
      Q_DISABLE_COPY(TBase)

      static QMutex m_Mutex;
   };
}

#endif // #ifndef QNETMAP_BASE_H
