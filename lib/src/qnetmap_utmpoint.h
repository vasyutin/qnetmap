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

#ifndef QNETMAP_UTMPOINT_H
#define QNETMAP_UTMPOINT_H

#include "qnetmap_global.h"

#include <QPointF>
#include <cmath> 

namespace qnetmap
{
   //! \class TUTMPoint
   //! \brief A UTM point
   class QNETMAP_EXPORT TUTMPoint
   {
   public:
      TUTMPoint(const QPointF& GeographicPoint_);
      TUTMPoint(const qreal X_, const qreal Y_, const int UTMZone_, const bool IsSouthLatitude = false);
      virtual ~TUTMPoint() {};

      //! \brief 
      qreal x(void) const { return m_X; };
      //! \brief 
      qreal y(void) const { return m_Y; };
      //! \brief 
      int utmZone(void) const { return m_UTMZone; };
      //! \brief 
      bool isSouthLatitude(void) const { return m_IsSouthLatitude; };
      //! \brief 
      QPointF toGeographic(void) const;

   protected:
      TUTMPoint();

      qreal m_X;
      qreal m_Y;
      int   m_UTMZone;
      bool  m_IsSouthLatitude;

      static qreal PI;
      // коэффициенты преобразований координат UTM <-> Geographic
      static qreal e;
      static qreal esq;
      static qreal e0sq;
      static qreal e1;
      static qreal e1sq;
      static qreal n;
      static qreal alpha;
      static qreal beta;
      static qreal gamma;
      static qreal delta;
      static qreal epsilon;
      static qreal J0;
      static qreal J1;
      static qreal J2;
      static qreal J3;
      static qreal J4;
   };
}
#endif
