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

#include "qnetmap_utmpoint.h"

namespace qnetmap
{
   // статические переменные
   qreal TUTMPoint::PI = 0;
   qreal TUTMPoint::e;
   qreal TUTMPoint::esq;
   qreal TUTMPoint::e0sq;
   qreal TUTMPoint::e1;
   qreal TUTMPoint::e1sq;
   qreal TUTMPoint::n;
   qreal TUTMPoint::alpha;
   qreal TUTMPoint::beta;
   qreal TUTMPoint::gamma;
   qreal TUTMPoint::delta;
   qreal TUTMPoint::epsilon;
   qreal TUTMPoint::J0;
   qreal TUTMPoint::J1;
   qreal TUTMPoint::J2;
   qreal TUTMPoint::J3;
   qreal TUTMPoint::J4;

   //----------------------------------------------------------------------
   TUTMPoint::TUTMPoint() : m_X(0), m_Y(0), m_UTMZone(0), m_IsSouthLatitude(false)
   {
   if(PI < 3.) {
      // статические члены еще не инициализированы, инициализируем
      PI = acos(-1.0);
      // постоянные коэффициенты
      //------------------------------------------------------
      // eccentricity of the earth's elliptical cross-section
      // e   = sqrt(1 - (b/a)*(b/a))
      // esq = 1 - (b/a)*(b/a)
      esq = 1. - QNetMapConsts::WGS84PolarRadius / QNetMapConsts::WGS84EquatorialRadius * 
                 QNetMapConsts::WGS84PolarRadius / QNetMapConsts::WGS84EquatorialRadius;
      e = sqrt(esq);
      // e0sq = e*e/(1-e*e)
      e0sq = esq / (1. - esq);
      // e1 = (1 - sqrt(1 - e*e))/(1 + sqrt(1 - e*e))]
      e1 = (1. - sqrt(1. - esq)) / (1. + sqrt(1. - esq));
      e1sq = e1 * e1;
      // n = (a-b)/(a+b)
      n = (QNetMapConsts::WGS84EquatorialRadius - QNetMapConsts::WGS84PolarRadius) /
          (QNetMapConsts::WGS84EquatorialRadius + QNetMapConsts::WGS84PolarRadius);
      alpha = ((QNetMapConsts::WGS84EquatorialRadius + QNetMapConsts::WGS84PolarRadius) / 2.0) *
               (1.0 + (n * n / 4.0) + (pow(n, 4.0) / 64.0));
      beta  = (-3.0 * n / 2.0) + (9.0 * pow(n, 3.0) / 16.0) + (-3.0 * pow(n, 5.0) / 32.0);
      gamma = (15.0 * n * n / 16.0) + (-15.0 * pow(n, 4.0) / 32.0);
      delta = (-35.0 * pow(n, 3.0) / 48.0) + (105.0 * pow(n, 5.0) / 256.0);
      epsilon = 315.0 * pow (n, 4.0) / 512.0;
      // J0 = (1 - esq*(1/4 + esq*(3/64 + 5*esq/256)))
      J0 = (1. - esq * (1. / 4. + esq * (3. / 64. + 5. * esq / 256.)));
      // J1 = (3e1/2 - 27e1**3/32 ..)
      J1 = e1 * (3. / 2. - e1sq * 27. / 32.);
      // J2 = (21e1**2/16 - 55e1**4/32 ..)
      J2 = e1sq * (21. / 16. - e1sq * 55. / 32.);
      // J3 = (151e1**3/96 ..)
      J3 = e1 * e1sq * 151. / 96.;
      // J4 = (1097e1**4/512 ..)
      J4 = e1sq * e1sq * 1097. / 512.;
      }
   }

   //----------------------------------------------------------------------
   TUTMPoint::TUTMPoint(const qreal X_, const qreal Y_, const int UTMZone_, const bool IsSouthLatitude)
   {
   TUTMPoint();
   //
   m_X = X_;
   m_Y = Y_;
   m_UTMZone = UTMZone_;
   m_IsSouthLatitude = IsSouthLatitude;
   // проверка на корректность
	if(X_ < 160000 || X_ > 840000)    assert(!"Outside permissible range of easting values"); 
	if(Y_ < 0)                        assert(!"Negative values not allowed");
	if(Y_ > 10000000)                 assert(!"Northing may not exceed 10000000");
   if(UTMZone_ < 1 || UTMZone_ > 60) assert(!"Outside permissible range of UTM zone");
   }

   //----------------------------------------------------------------------
   TUTMPoint::TUTMPoint(const QPointF& GeographicPoint_)
   {
   TUTMPoint();
   // calculate utm zone
	m_UTMZone = 31 + floor(GeographicPoint_.x() / 6);
   //Central meridian of zone
	qreal zcm = (6. * m_UTMZone - 183.) * PI / 180.;
   // Geographic coordinates in radians
   qreal Longitude = GeographicPoint_.x() * PI / 180.;
   qreal Latitude  = GeographicPoint_.y() * PI / 180.;
   // Calculate the Meridional Arc
   qreal M = alpha * (Latitude + (beta    * sin(2.0 * Latitude))
                               + (gamma   * sin(4.0 * Latitude))
                               + (delta   * sin(6.0 * Latitude))
                               + (epsilon * sin(8.0 * Latitude)));
   //
   // Converting Latitude and Longitude to UTM
   // nu=a/((1-(e*SIN(lat))^2)^(1/2))
   qreal nu = QNetMapConsts::WGS84EquatorialRadius / sqrt(1. - pow(e * sin(Latitude), 2));
   /*
   y = northing = K1 + K2p**2 + K3p**4, where
    K1 = Sk0,
    K2 = k0 nu sin(lat)cos(lat)/2 = k0 nu sin(2 lat)/4
    K3 = [k0 nu sin(lat)cos3(lat)/24][(5 - tan2(lat) + 9e'2cos2(lat) + 4e'4cos4(lat)]
   x = easting = K4p + K5p**3, where
    K4 = k0 nu cos(lat)
    K5 = (k0 nu cos3(lat)/6)[1 - tan2(lat) + e'2cos2(lat)]
   */
   qreal K1 = M * QNetMapConsts::UTMK0;
   qreal K2 = (nu * sin(2. * Latitude) / 4.) * QNetMapConsts::UTMK0;
   qreal K3 = nu * sin(Latitude) * pow(cos(Latitude), 3) / 24. * 
              (5. - pow(tan(Latitude), 2) + 9. * e0sq * pow(cos(Latitude), 2) + 
               4. * e0sq * e0sq * pow(cos(Latitude), 4)) * 
              QNetMapConsts::UTMK0;
   qreal K4 = nu * cos(Latitude) * QNetMapConsts::UTMK0;
   qreal K5 = nu * pow(cos(Latitude), 3) / 6. * 
              (1. - pow(tan(Latitude), 2) + e0sq * pow(cos(Latitude), 2)) * QNetMapConsts::UTMK0;

   // Calculate UTM Values
   qreal p = Longitude - zcm;
   qreal psq = p * p;
   /// Easting relative to CM
   m_X = K4 * p + K5 * p * psq;
	m_X += 500000.; // Easting standard 
   /// Northing from equator
   m_Y = K1 + K2 * psq + K3 * psq * psq;
	if(m_Y < 0) { m_Y += 10000000.; m_IsSouthLatitude = true; }
   }

   //----------------------------------------------------------------------
   QPointF TUTMPoint::toGeographic(void) const
   {
   // Calculate the Meridional Arc
   qreal M = m_Y / QNetMapConsts::UTMK0;
   // Calculate Footprint Latitude
   // mu = M/(a*J0);
   qreal mu = M / (QNetMapConsts::WGS84EquatorialRadius * J0);
   // footpoint latitude fp = mu + J1sin(2mu) + J2sin(4mu) + J3sin(6mu) + J4sin(8mu)
   qreal fp = mu + J1 * sin(2. * mu) + J2 * sin(4. * mu)  + J3 * sin(6. * mu) + J4 * sin(8. * mu);
   // Calculate Latitude and Longitude
   // C1 = e0sq*pow(cos(phi1),2)
   qreal C1 = e0sq * pow(cos(fp), 2);
	// T1 = pow(tan(phi1),2)
   qreal T1 = pow(tan(fp), 2);
   // (1-pow(e*sin(phi1),2))
   qreal k = 1. - pow(e * sin(fp), 2);
	// N1 = a/sqrt(1-pow(e*sin(phi1),2))
   qreal N1 = QNetMapConsts::WGS84EquatorialRadius / sqrt(k);
	// R1 = N1*(1-e*e)/(1-pow(e*sin(phi1),2))
   qreal R1 = N1 * (1. - esq) / k;
   // D = (x-500000)/(N1*k0)
   qreal D = (500000. - m_X) / (N1 * QNetMapConsts::UTMK0);
   //
   qreal Q1 = N1 * tan(fp) / R1;
   qreal Q2 = D * D / 2.;
   qreal Q3 = (5. + 3. * T1 + 10. * C1 - 4. * C1 * C1 - 9. * e0sq) * Q2 * Q2 / 6.;
   qreal Q4 = 
      (61. + 90. * T1 + 298. * C1 + 45. * T1 * T1 - 3. * C1 * C1 - 252. * e0sq) * Q2 * Q2 * Q2 / 90.;
   qreal Q5 = D;
   qreal Q6 = (1. + 2. * T1 + C1) * Q2 * D / 3.;
   qreal Q7 = (5. - 2. * C1 + 28. * T1 - 3. * C1 * C1 + 8. * e0sq + 24. * T1 * T1) * Q2 * Q2 * D / 30.;
   //
   qreal Latitude   = (fp - Q1 * (Q2 - Q3 + Q4)) * 180. / PI;
   qreal Longitude0 = 6. * m_UTMZone - 183.;
   qreal Longitude  = (Q5 - Q6 + Q7) / cos(fp) * 180. / PI;
   //
   return QPointF(Longitude0 - Longitude, Latitude);
   }
}
