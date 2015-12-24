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

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QMessageBox>
   #include <QApplication>
#endif
#include <QFile>
#include <QTextStream>
#include <QDateTime>

namespace qnetmap
{
   QMutex TBase::m_Mutex;
   QAtomicInt TBase::m_MiddlePointAtomicInt = 0;

   // -------------------------------------------------------------
   void TBase::qnetmaplog( const QString& Message_, QObject* Sender_ /*= NULL*/, int Flags_ /*= Consts::ToLog | Consts::ShowClassName*/ )
   {
   QString ClassName;
   if(Sender_ && (Flags_ & Consts::ShowClassName)) ClassName = QString(Sender_->metaObject()->className());
   if(Flags_ & Consts::ShowMessage) {
      QMessageBox::information(NULL, "", QString("%1\n%2").arg(ClassName).arg(Message_));
      }
   if(Flags_ & Consts::ToLog) {
      QMutexLocker Locker(&m_Mutex);
      QFile Log(QApplication::applicationDirPath() + "/qnetmap.log");
      if(Log.open(QIODevice::Append | QIODevice::Text)) {
         QTextStream out(&Log);
         out << QString("%1 : %2 %3\n").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"))
                                  .arg(ClassName)
                                  .arg(Message_);
         Log.close();
         }
      }
   }

   // -----------------------------------------------------------------------
   QPoint TBase::alignedPoint( const QPoint& Point_, const QSize& Size_, const enum TAlignment Alignment_ )
   {
      QPoint TopLeftPoint;

      switch(Alignment_) {
      case Middle:
         TopLeftPoint.setX(Point_.x() - ((Size_.width()  + 1) >> 1));
         TopLeftPoint.setY(Point_.y() - ((Size_.height() + 1) >> 1));
         break;
      case TopLeft:
         TopLeftPoint.setX(Point_.x());
         TopLeftPoint.setY(Point_.y());
         break;
      case TopMiddle:
         TopLeftPoint.setX(Point_.x() - ((Size_.width() + 1) >> 1));
         TopLeftPoint.setY(Point_.y());
         break;
      case TopRight:
         TopLeftPoint.setX(Point_.x() - Size_.width());
         TopLeftPoint.setY(Point_.y());
         break;
      case BottomLeft:
         TopLeftPoint.setX(Point_.x());
         TopLeftPoint.setY(Point_.y() - Size_.height());
         break;
      case BottomMiddle:
         TopLeftPoint.setX(Point_.x() - ((Size_.width() + 1) >> 1));
         TopLeftPoint.setY(Point_.y() - Size_.height());
         break;
      case BottomRight:
         TopLeftPoint.setX(Point_.x() - Size_.width());
         TopLeftPoint.setY(Point_.y() - Size_.height());
         break;
      case RightMiddle:
         TopLeftPoint.setX(Point_.x() - Size_.width());
         TopLeftPoint.setY(Point_.y() - ((Size_.height() + 1) >> 1));
         break;
      case LeftMiddle:
         TopLeftPoint.setX(Point_.x());
         TopLeftPoint.setY(Point_.y() - ((Size_.height() + 1) >> 1));
         break;
      }

      return TopLeftPoint;
   }

   //---------------------------------------------------------------------------
   void TBase::setColorMiddlePixel(const QRgb* j, QRgb* n, const QRgb* k)
   {
      unsigned Red   = (qRed(*j)   + (qRed(*n)   << 1) + qRed(*k))   >> 2;
      unsigned Green = (qGreen(*j) + (qGreen(*n) << 1) + qGreen(*k)) >> 2;
      unsigned Blue  = (qBlue(*j)  + (qBlue(*n)  << 1) + qBlue(*k))  >> 2;
      unsigned Alfa  = (Red + Green + Blue) / 3;
      if(Alfa > 0) *n = qRgba(Red, Green, Blue, Alfa);
   }

   //---------------------------------------------------------------------------
   void TBase::blur(QImage& Image_)
   {
      unsigned Lines   = Image_.height();
      unsigned Columns = Image_.width();
      QRgb *j, *n, *k, *Line;
      // go line by line
      for(unsigned i = 0; i < Lines; ++i) {
         Line = reinterpret_cast<QRgb*>(Image_.scanLine(i));
         j = Line;
         n = j + 1;
         k = n + 1;
         // go along the line
         for(; j < Line + Columns - 2; ++j, ++n, ++k) setColorMiddlePixel(j, n, k);
      }
      // go column by column
      for(unsigned i = 2; i < Lines; ++i) {
         if(i == 2) {
            j = reinterpret_cast<QRgb*>(Image_.scanLine(0));
            n = reinterpret_cast<QRgb*>(Image_.scanLine(1));
         }
         else {
            j = n;
            n = k;
         }
         k = reinterpret_cast<QRgb*>(Image_.scanLine(i));
         // go along the column
         for(unsigned i = 0; i < Columns; ++i, ++j, ++n, ++k) setColorMiddlePixel(j, n, k);
      }
   }
}
