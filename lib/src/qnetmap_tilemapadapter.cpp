/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl TileMapAdapter code by Kai Winter
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

#include "qnetmap_tilemapadapter.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_mapnetwork.h"

#include <cmath> 
#include <QMultiMap>
#include <QSettings>
#include <QDir>

namespace qnetmap
{
   //////////////////////////////////////////////////////////////////////////
   TTileMapAdapter::TTileMapAdapter(
      const QString& Host_, const QString& QueryTemplate_, 
      const int TileSizeX_, const int TileSizeY_, 
      const int MinZoom_, const int MaxZoom_,
      const int HostNumberMinimum_, const int HostNumberMaximum_)
      : TMapAdapter(TileSizeX_, TileSizeY_, MinZoom_, MaxZoom_),
        m_MapVersion(""), m_TemporaryFileName(""),
        m_QueryTemplate(QueryTemplate_),
        m_HostNumberMinimum(HostNumberMinimum_), m_HostNumberMaximum(HostNumberMaximum_),
        m_CoordinatesWidth(0), m_ZoomWidth(0), m_QuadKeyThere(false)
   {
   setURL(Host_, QueryTemplate_);
   setSourceType(TMapAdapter::Consts::InternetMap);
   //
   m_NumberOfXTiles = xtilesonzoomlevel(m_CurrentZoom);
   m_NumberOfYTiles = ytilesonzoomlevel(m_CurrentZoom);
   m_Locale.setNumberOptions(QLocale::OmitGroupSeparator);
   }

   //////////////////////////////////////////////////////////////////////////
   TTileMapAdapter::TTileMapAdapter(const TTileMapAdapter& Source_) : TMapAdapter(Source_)
   {
   m_Host = Source_.m_Host;
   m_QueryTemplate = Source_.m_QueryTemplate;
   m_MapVersion = Source_.m_MapVersion;
   for(int i = 0; i < 5; ++i) for(int j = 0; j < 2; ++j) m_Order[i][j] = Source_.m_Order[i][j];
   m_TemporaryFileName = Source_.m_TemporaryFileName;
   m_HostNumberMinimum = Source_.m_HostNumberMinimum;
   m_HostNumberMaximum = Source_.m_HostNumberMaximum;
   m_HostIsTemplate = Source_.m_HostIsTemplate;
   m_CoordinatesWidth = Source_.m_CoordinatesWidth;
   m_ZoomWidth = Source_.m_ZoomWidth;
   m_QuadKeyThere = Source_.m_QuadKeyThere;
   }

   //----------------------------------------------------------------------
   void TTileMapAdapter::setURL(const QString& Host_, const QString& QueryTemplate_)
   {
   if(!Host_.isEmpty() && !QueryTemplate_.isEmpty()) {
      m_Host = Host_;
      m_HostIsTemplate = Host_.contains("%1");
      m_QueryTemplate = QueryTemplate_;
      m_QuadKeyThere = QueryTemplate_.contains("%5");
      // установка правильности инициализации адаптера
      setInitialized(Host_.size() && QueryTemplate_.size() && 
                     tileSizeX()>0 && tileSizeY()>0 && minZoom()>=0 && maxZoom()>=0);
      /*
      Initialize the "substring replace engine". First the string replacement
      in getQuery was made by QString().arg() but this was very slow. So this
      splits the servers path into substrings and when calling getQuery the
      substrings get merged with the parameters of the URL.
      Pretty complicated, but fast.
      */
      QMultiMap<int, unsigned> Params;
      Params.insert(m_QueryTemplate.indexOf("%1"), 0);
      Params.insert(m_QueryTemplate.indexOf("%2"), 1);
      Params.insert(m_QueryTemplate.indexOf("%3"), 2);
      Params.insert(m_QueryTemplate.indexOf("%4"), 3);
      Params.insert(m_QueryTemplate.indexOf("%5"), 4);
      unsigned i = 0;
      for(QMultiMap<int, unsigned>::const_iterator it = Params.begin(); it != Params.end(); ++it, ++i) {
         m_Order[i][0] = it.key();
         m_Order[i][1] = it.value();
         }
      }
   }

   //---------------------------------------------------------------------
   void TTileMapAdapter::setMapVersion(const QString& Version_, const bool ToSave_)
   {
   QString Version = Version_;
   // Пытаемся получить версию из реестра
   QSettings Settings(QNetMapConsts::Organization, pluginName());
   QString Key(QNetMapConsts::MapVersionKey);
   bool ToSave = !ToSave_;
   QString Value;
   if(Settings.contains(Key)) {
      Value = Settings.value(Key).toString();
      if(ToSave && !Value.isEmpty() && Value > Version) {
         Version = Value;
         ToSave = false;
         }
      }
   if(ToSave || (ToSave_ && Value != Version)) { 
      // Сохраняем в реестре
      Settings.setValue(Key, Version);
      }
   //
   m_MapVersion = Version; 
   }

   //---------------------------------------------------------------------
   void TTileMapAdapter::setMapVersion(
      const QString& DefaultVersion_, const QString& Url_, 
      const QString& VersionPrefix_,  const QString& VersionPostfix_)
   {
   setMapVersion(DefaultVersion_);
   int StorageType = storageType();
   if((StorageType == TImageManager::Consts::InternetAndStorage ||
       StorageType == TImageManager::Consts::InternetOnly) && 
      !Url_.isEmpty()) {
      //
      m_VersionPrefix  = VersionPrefix_;
      m_VersionPostfix = VersionPostfix_;
      //
      int FileNumber = TImageManager::instance()->uniqueNumber();
      m_TemporaryFileName = 
         QDir::tempPath() + 
         QString("/qnetmap%1_%2.tmp").arg(FileNumber).arg(pluginName());
      //
      QNM_DEBUG_CHECK(connect(TImageManager::instance(), SIGNAL(htmlReceived()), this, SLOT(htmlReceived())));
      //
      QFile File(m_TemporaryFileName);
      if(!File.exists() || File.remove()) {
         TImageManager::instance()->mapNetwork()->loadURL(this, QUrl("http://" + Url_), m_TemporaryFileName);
         }
      }
   }

   //----------------------------------------------------------------------
   void TTileMapAdapter::setZoomRestrictions(const int Min_, const int Max_)
   {
   TMapAdapter::setZoomRestrictions(Min_, Max_);
   // установка правильности инициализации адаптера
   setInitialized(m_Host.size() && m_QueryTemplate.size() && 
                  tileSizeX()>0 && tileSizeY()>0 && minZoom()>=0 && maxZoom()>=0);

   }

   //----------------------------------------------------------------------
   QString TTileMapAdapter::query(const int X_, const int Y_, const int Z_) const
   {
   QString X = m_Locale.toString(xoffset(X_));
   QString Y = m_Locale.toString(yoffset(Y_));
   QString Z = m_Locale.toString(zoom(Z_));
   QString QuadKey;
   if(m_QuadKeyThere) QuadKey = quadkey(X_, Y_, Z_);
   QString RetValue = m_QueryTemplate;

   if(m_CoordinatesWidth > (uint)X.length()) { X = X.rightJustified(m_CoordinatesWidth, '0'); }
   if(m_CoordinatesWidth > (uint)Y.length()) { Y = Y.rightJustified(m_CoordinatesWidth, '0'); }
   if(m_ZoomWidth > (uint)Z.length()) { Z = Z.rightJustified(m_ZoomWidth, '0'); }

   QString a[5] = {Z, X, Y, mapVersion(), QuadKey};
   if(m_Order[4][0] > 0) RetValue = RetValue.replace(m_Order[4][0], 2, a[m_Order[4][1]]);
   if(m_Order[3][0] > 0) RetValue = RetValue.replace(m_Order[3][0], 2, a[m_Order[3][1]]);
   if(m_Order[2][0] > 0) RetValue = RetValue.replace(m_Order[2][0], 2, a[m_Order[2][1]]);
   if(m_Order[1][0] > 0) RetValue = RetValue.replace(m_Order[1][0], 2, a[m_Order[1][1]]);
   if(m_Order[0][0] > 0) RetValue = RetValue.replace(m_Order[0][0], 2, a[m_Order[0][1]]);

   return RetValue;
   }

   //----------------------------------------------------------------------
   QString TTileMapAdapter::quadkey(const int X_, const int Y_, const int Z_) const
   {
   QString QuadKey;

   for (int i = Z_; i > 0; i--) {
      char digit = '0';
      int mask = 1 << (i - 1);
      if ((X_ & mask) != 0) digit++;
      if ((Y_ & mask) != 0) { digit++; digit++; }
      QuadKey += digit;
      }

   return QuadKey;
   }

   //----------------------------------------------------------------------
   int TTileMapAdapter::zoom(int z) const
   {
   return z + zoomShift();
   }

   //----------------------------------------------------------------------
   qreal TTileMapAdapter::arcTanh(const qreal Arg_) const
   {
   return log((1.0 + Arg_) / (1.0 - Arg_)) * 0.5;
   }

   //----------------------------------------------------------------------
   int TTileMapAdapter::round(const qreal Number_) const
   {
   return (Number_ >= 0) ? (int)(Number_ + 0.5) : (int)(Number_ - 0.5);
   }

   //----------------------------------------------------------------------
   QString TTileMapAdapter::host(const bool Random_) const
   {
   static int Number = m_HostNumberMinimum;
   if(m_HostIsTemplate) {
      if(Random_) {
         return m_Host.arg((char)(m_HostNumberMinimum + rand() % hostsCount()));
         }
      else {
         if(Number > m_HostNumberMaximum) Number = m_HostNumberMinimum;
         return m_Host.arg((char)(++Number));
         }
      }
   else
      return m_Host;
   }

   //----------------------------------------------------------------------
   unsigned TTileMapAdapter::hostsCount(void) const
   {
   if(m_HostIsTemplate) return m_HostNumberMaximum - m_HostNumberMinimum + 1;
   else return 1;
   }

   //--------------------------------------------------------------------------------------
   void TTileMapAdapter::htmlReceived(void)
   {
   if(!m_TemporaryFileName.isEmpty()) {
      QFile File(m_TemporaryFileName);
      if(File.exists() && File.open(QIODevice::ReadWrite)) {
         m_TemporaryFileName = "";
         QByteArray Content = File.readAll();
#ifdef QNETMAP_QT_4
         QByteArray Begin(m_VersionPrefix.toAscii()); // строка, предваряющая номер версии
#else
         QByteArray Begin(m_VersionPrefix.toLatin1()); // строка, предваряющая номер версии
#endif
         int VersionBegin = -1;
         do {
            VersionBegin = Content.indexOf(Begin, VersionBegin + 1);
            if(VersionBegin >=0) {
               VersionBegin += Begin.size();
#ifdef QNETMAP_QT_4
               int VersionEnd = Content.indexOf(m_VersionPostfix.toAscii(), VersionBegin);
#else
               int VersionEnd = Content.indexOf(m_VersionPostfix.toLatin1(), VersionBegin);
#endif
               if(VersionEnd > 0 && VersionEnd - VersionBegin < 12) {
                  QByteArray Version(Content.mid(VersionBegin, VersionEnd - VersionBegin));
                  setMapVersion(QString(Version), true);
                  VersionBegin = -1;
                  }
               }
            } while(VersionBegin >= 0);
         //
         File.close();
         File.remove();
         disconnect(TImageManager::instance(), SIGNAL(htmlReceived()), this, SLOT(htmlReceived()));
         }
      }
   }

   //--------------------------------------------------------------------------------------
   void TTileMapAdapter::setParameters(TMapAdapterParameters* Parameters_)
   {
   TMapAdapter::setParameters(Parameters_);
   TTileMapAdapterParameters* Parameters = static_cast<TTileMapAdapterParameters*>(Parameters_);
   //
   setSourceType(Consts::InternetMap);
   setURL(Parameters->hostTemplate(), Parameters->queryTemplate());
   m_HostNumberMinimum = Parameters->hostNumberMinimum();
   m_HostNumberMaximum = Parameters->hostNumberMaximum();
   m_CoordinatesWidth = Parameters->coordinatesWidth();
   m_ZoomWidth = Parameters->zoomWidth();
   setMapVersion(Parameters->defaultMapVersion(), Parameters->mapVersionUrl(),
                 Parameters->mapVersionPrefix(),  Parameters->mapVersionPostfix());
   }
}
