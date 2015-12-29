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

#ifndef QNETMAP_GLOBAL_H
#define QNETMAP_GLOBAL_H

#include <QtCore/qglobal.h>

#if QT_VERSION >= 0x050000
   #include <QtWidgets/QtWidgets>
#else
   #define QNETMAP_QT_4
#endif

#include <QThread>
#include <QPointF>
#include <QDebug>

#include <assert.h>
#ifdef _DEBUG
	#define QNM_DEBUG_CHECK assert
#else
	#define QNM_DEBUG_CHECK(x) x
#endif

#ifdef _DEBUG
namespace qnetmap {
   class QNetMapDebug : public QDebug
   {
   public:
      QNetMapDebug(QIODevice* device, QString const& Function_, unsigned Line_)   : QDebug(device), m_Function(Function_) { ++m_Shift; qDebug() << QString("+%1%2 (%3) ").arg(spaces()).arg(m_Function).arg(Line_); }
      QNetMapDebug(QString* string, QString const& Function_, unsigned Line_)     : QDebug(string), m_Function(Function_) { ++m_Shift; qDebug() << QString("+%1%2 (%3) ").arg(spaces()).arg(m_Function).arg(Line_); }
      QNetMapDebug(QtMsgType type, QString const& Function_, unsigned Line_)      : QDebug(type),   m_Function(Function_) { ++m_Shift; qDebug() << QString("+%1%2 (%3) ").arg(spaces()).arg(m_Function).arg(Line_); }
      QNetMapDebug(const QDebug& other, QString const& Function_, unsigned Line_) : QDebug(other),  m_Function(Function_) { ++m_Shift; qDebug() << QString("+%1%2 (%3) ").arg(spaces()).arg(m_Function).arg(Line_); }
      ~QNetMapDebug() { qDebug() << QString("-%1%2").arg(spaces()).arg(m_Function); --m_Shift; }

      static unsigned shift(void) { return m_Shift; }
      static QString spaces(void) { return QString(m_Shift, ' '); }

   private:
      static unsigned m_Shift;
      QString m_Function;
   };
#define qNetMapDebug() qnetmap::QNetMapDebug NetMapDebug(QtDebugMsg, __FUNCTION__, __LINE__); NetMapDebug 
}
#else
namespace qnetmap {
#define qNetMapDebug() qDebug()
}
#endif

#ifdef QNETMAP_LIB
#define QNETMAP_EXPORT Q_DECL_EXPORT
#else
#define QNETMAP_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUG__
#define GCC_ATTRIBUTE_UNUSED __attribute__ ((unused))
#else
#define GCC_ATTRIBUTE_UNUSED
#endif

//#include "qnetmap_translator.h"
//#include "qnetmap_base.h"

#if defined(WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define QNETMAP_DEBUG_NEW new( _CLIENT_BLOCK, __FILE__, __LINE__ )
#endif

#define SETTINGS_PROPERTY_NAME(Name_) QLatin1String( #Name_ )

#define SELECTION_PADDING  2

#define QNETMAP_VERSION_MAJOR  2
#define QNETMAP_VERSION_MINOR  1
#define QNETMAP_VERSION        0x020000


namespace qnetmap
{
   typedef QList<QPointF> TCoordinatesList;

   enum TAlignment {
      TopLeft,      /*!< Align on TopLeft*/
      TopMiddle,    /*!< Align on TopMiddle*/
      TopRight,     /*!< Align on TopRight*/
      RightMiddle,  /*!< Align on LeftMiddle*/
      LeftMiddle,   /*!< Align on RightMiddle*/
      BottomLeft,   /*!< Align on BottomLeft*/
      BottomMiddle, /*!< Align on BottomMiddle*/
      BottomRight,  /*!< Align on BottomRight*/
      Middle        /*!< Align on Middle*/
      };

   namespace QNetMapConsts {
      static const char *Organization         GCC_ATTRIBUTE_UNUSED = "QNetMap";
      static const char *CachePathKey         GCC_ATTRIBUTE_UNUSED = "CachePath";
      static const char *StoragePathKey       GCC_ATTRIBUTE_UNUSED = "StoragePath";
      static const char *StorageTypeKey       GCC_ATTRIBUTE_UNUSED = "StorageType";
      static const char *FilePathKey          GCC_ATTRIBUTE_UNUSED = "FilePath";
      static const char *NameKey              GCC_ATTRIBUTE_UNUSED = "Name";
      static const char *MapVersionKey        GCC_ATTRIBUTE_UNUSED = "MapVersion";
      static const char *RasterMapFilePathKey GCC_ATTRIBUTE_UNUSED = "Raster Maps/FilePath";
      static const char *RasterMapNameKey     GCC_ATTRIBUTE_UNUSED = "Raster Maps/MapName";
      static const qreal UTMK0 = 0.9996 ;               // scale along central meridian of zone
      static const qreal GoogleMapsSphereRadius = 6378137.;
      static const qreal PopularVisualisationSphereRadius = 6378137.;
      static const qreal WGS84EquatorialRadius = 6378137.;
      static const qreal WGS84PolarRadius = 6356752.3142;
      static const qreal E = 2.71828182845904523536;
   } // namespace QNetMapConsts

   class TQNetMapHelper
   {
   public:
      //! \brief Проверка папки на запись
      static bool checkDirectoryForWritePermissions(const QString &Path_, bool Recreate_ = false);
      //! \brief Возвращает вычисленное значение CRC32 указанных данных
      static quint32 hashCode(const void* Data_, size_t Length_);
   };

   //! \class TSleep
   //! \brief реализация sleep средствами Qt
   class QNETMAP_EXPORT TSleep : public QThread
   {
   public:
      static void  sleep(unsigned long Secs_)  {QThread::sleep(Secs_);}
      static void msleep(unsigned long MSecs_) {QThread::msleep(MSecs_);}
      static void usleep(unsigned long USecs_) {QThread::usleep(USecs_);}
   };

   template <typename T> int sgn(T val) {
      return (T(0) < val) - (val < T(0));
   }

   //! \class TLock
   //! \brief 
   class TLock
   {
   public:
      explicit TLock(QAtomicInt& AtomicInt_, unsigned Milliseconds_ = 5) : m_AtomicInt(AtomicInt_)
      {
         while(!m_AtomicInt.testAndSetAcquire(0, 1)) { TSleep::msleep(Milliseconds_); }
      }
      ~TLock()
      {
         m_AtomicInt.fetchAndStoreAcquire(0);
      }

      void unlock(void)
      {
         m_AtomicInt.fetchAndStoreAcquire(0);
      }
      bool locked(void) const
      {
         return m_AtomicInt == 1;
      }

   private:
      QAtomicInt& m_AtomicInt;
   };
}

#include "qnetmap_translator.h"
#include "qnetmap_base.h"

#endif // QNETMAP_GLOBAL_H
