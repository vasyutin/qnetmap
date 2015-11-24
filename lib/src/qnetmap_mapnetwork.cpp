/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl MapNetwork code by Kai Winter
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

#include "qnetmap_mapnetwork.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_mapcontrol.h"

namespace qnetmap
{
   static bool gProxyIsSet = false;
   static QMutex gJPEGSupportCheckedMutex;
   static bool gJPEGSupportChecked = false;
   THttpParametersList TMapNetwork::m_HttpParametersList;

   //----------------------------------------------------------------------
   TReplyParameters::TReplyParameters(QNetworkReply* Reply_,
                    TMapAdapter* MapAdapter_, const QString& Url_, const QString& Path_, 
                    bool CacheInMappingStorage_,
                    const QString& MapVersion_,
                    TParent *Parent_) 
      : TParent(Parent_), m_Reply(Reply_), m_MapAdapter(MapAdapter_), m_Url(Url_),
        m_Path(Path_), m_CacheInMappingStorage(CacheInMappingStorage_),
        m_MapControl(MapAdapter_->mapControl()),
        m_MapVersion(MapVersion_)
   {
   // если за это время запрос не отработает, он будет прерван
   QTimer::singleShot(3000, this, SLOT(replyAbort()));
   }

   //----------------------------------------------------------------------
   TMapNetwork::TMapNetwork(TImageManager* Parent_)
      : w_Parent(Parent_),
        w_NetworkAccessManager(new QNetworkAccessManager(this)),
        w_NetworkDiskCache(NULL), m_UseProxyAddressList(false),
        m_AllAbortLoading(false)
   {
   m_ContentNotFoundErrorCache.setMaxCost(400);
   QNM_DEBUG_CHECK(connect(w_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*))));
   }

   //----------------------------------------------------------------------
   TMapNetwork::~TMapNetwork()
   {
   for(QList<TProxyAddress*>::const_iterator it = m_ProxyAddressList.begin();
      it != m_ProxyAddressList.end(); ++it) {
      delete *it;
      }
   
   abortLoading();
   if(w_NetworkDiskCache     != NULL) delete w_NetworkDiskCache;
   if(w_NetworkAccessManager != NULL) delete w_NetworkAccessManager;
   }

   //----------------------------------------------------------------------
   void TMapNetwork::loadURL(const TMapAdapter* MapAdapter_, 
                             const QUrl Url_,
                             const QString& FilePath_,
                             const bool CacheInMappingStorage_,
                             const QString& MapVersion_)
   {
   TMapAdapter* MapAdapter = const_cast<TMapAdapter*>(MapAdapter_);
   
   m_ReplyMapMutex.lock();
   // создаем запрос                                                              
   QNetworkRequest Request = QNetworkRequest(Url_);
   unsigned HttpParametersListSize = m_HttpParametersList.size();
   if(HttpParametersListSize) {
      THttpParametersList::const_iterator it = m_HttpParametersList.begin() + rand() % m_HttpParametersList.size();
      Request.setRawHeader("User-Agent", QByteArray(it->userAgent().toStdString().c_str()));
   }
   if(MapAdapter_->updatability() == TMapAdapter::Consts::QueryUpdatabilityDynamic) {
      // устанавливаем работу без кеша
      Request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);
      }
   else {
      // устанавливаем приоритет для кеша
      Request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
      }
   // запрашиваем
   if(!gProxyIsSet) {
      QList<QNetworkProxy> ProxyList = QNetworkProxyFactory::proxyForQuery(QNetworkProxyQuery(Url_));
	  if(ProxyList.size()) { w_NetworkAccessManager->setProxy(ProxyList.at(0)); gProxyIsSet = true; }
   }
   QNetworkReply* Reply = w_NetworkAccessManager->get(Request);
   // запоминаем параметры запроса
   m_NetworkReplies[Reply] = new TReplyParameters(Reply, MapAdapter, Url_.toString(), FilePath_, CacheInMappingStorage_, MapVersion_);
   m_ReplyMapMutex.unlock();
   // сигнал об изменении размера очереди
   emit(sizeQueueRequestsChanged(MapAdapter, queueSize(MapAdapter)));
   }

   //----------------------------------------------------------------------
   void TMapNetwork::requestFinished(QNetworkReply* NetworkReply_)
   {
   static const QByteArray ContentImagePNG("image/png");
   static const QByteArray ContentImageJpg("image/jpg");
   static const QByteArray ContentImageJpeg("image/jpeg");
   static const QByteArray ContentText("text");
   static const QByteArray ChunkName("qNMp");
   //
   QString Url = NetworkReply_->url().toString();
   TReplyParameters* ReplyParameters = m_NetworkReplies[NetworkReply_];
   ReplyParameters->setFinished();
   TMapAdapter* MapAdapter = ReplyParameters->mapAdapter();
   TMapControl* MapControl = ReplyParameters->mapControl();
   // no error received?
   // пишем в лог
   //qnetmaplog(QNetMapTranslator::tr("Url: %1 , TileName: %2").arg(Url).arg(ReplyParameters->path()), this);
   if(NetworkReply_->error() == QNetworkReply::NoError) {
      QByteArray Type = NetworkReply_->header(QNetworkRequest::ContentTypeHeader).toByteArray();
      if(Type == ContentImagePNG || Type == ContentImageJpeg || Type == ContentImageJpg) {
         // получена картинка
         if(ReplyParameters->cacheInMappingStorage()) {
            // сохраняем тайл по указанному пути
            QString FileFullName = ReplyParameters->path();
            if(!FileFullName.isEmpty()) {
               QImageReader ImageReader(NetworkReply_);
               ImageReader.setAutoDetectImageFormat(false);
               if(Type == ContentImagePNG) ImageReader.setFormat("PNG");
               else {
                  ImageReader.setFormat("JPG");
                  gJPEGSupportCheckedMutex.lock();
                  if(!gJPEGSupportChecked) {
                     gJPEGSupportChecked = true;
                     gJPEGSupportCheckedMutex.unlock();
                     QList<QByteArray> SupportedImageFormats = QImageReader::supportedImageFormats();
                     if(SupportedImageFormats.indexOf("jpg", 0) < 0) {
                        QMessageBox::critical(NULL, "Error", "Format JPEG is not supported! Add library imageformats\\qjpeg4.dll!");
                     }
                  }
                  else gJPEGSupportCheckedMutex.unlock();
               }
               //
			   QPixmap Tile(QPixmap::fromImage(ImageReader.read()));
               QFileInfo TileFile(FileFullName);
               QDir Path(TileFile.absolutePath());
               if(!Path.exists()) Path.mkpath(Path.absolutePath());
               QString MapVersion = ReplyParameters->mapVersion();
               if(MapVersion.isEmpty())   Tile.save(FileFullName);
               else {
                  // Дописываем в тайл информацию о версии карт
                  QByteArray PNGImage;
                  QBuffer Buffer(&PNGImage);
                  Buffer.open(QIODevice::WriteOnly);
                  Tile.save(&Buffer, "PNG");
                  TPNGImage Image(PNGImage);
                  Image.addChunk(ChunkName, QByteArray("Map version").append('\0').append(MapVersion));
                  Image.save(FileFullName);
                  }
               // отправляем сигнал
               if(w_Parent != NULL) w_Parent->receivedImage();
               }
            }
         }
      else if(Type.mid(0, 4) == ContentText) {
         // сохраняем content по указанному пути
         QString FileFullName = ReplyParameters->path();
         if(!FileFullName.isEmpty()) {
            QFileInfo HtmlFile(FileFullName);
            QDir Path(HtmlFile.absolutePath());
            if(!Path.exists()) Path.mkpath(Path.absolutePath());
            QFile File(HtmlFile.absoluteFilePath());
            if((!File.exists() || File.remove()) && File.open(QIODevice::WriteOnly)) {
               File.write(NetworkReply_->readAll());
               File.close();
               }
            // отправляем сигнал
            if(w_Parent != NULL) w_Parent->receivedHtml();
            }
         }
      }
   else {
      bool ToLog = QFile::exists(QApplication::applicationDirPath() + "/debug");
      // обработка ошибки отсутствия запрашиваемого тайла
      QString ErrorString = NetworkReply_->errorString();
      QNetworkReply::NetworkError Error = NetworkReply_->error();
      if(ToLog) {
         // пишем в лог
         if(Error != QNetworkReply::OperationCanceledError) {
            qnetmaplog(QNetMapTranslator::tr("NetworkError: %1 %2, TileName: %3").arg(Url).arg(ErrorString).arg(ReplyParameters->path()), this);
         }
      }
      //
      switch(Error) {
      // network layer errors [relating to the destination server] (1-99):
      case QNetworkReply::ConnectionRefusedError:
         break;
      case QNetworkReply::RemoteHostClosedError:
         break;
      case QNetworkReply::HostNotFoundError:
         break;
      case QNetworkReply::TimeoutError:
         break;
      case QNetworkReply::OperationCanceledError:
         break;
      case QNetworkReply::SslHandshakeFailedError:
         break;
      case QNetworkReply::UnknownNetworkError:
         break;
      // proxy errors (101-199)
      case QNetworkReply::ProxyConnectionRefusedError:
         break;
      case QNetworkReply::ProxyConnectionClosedError:
         break;
      case QNetworkReply::ProxyNotFoundError:
         break;
      case QNetworkReply::ProxyTimeoutError:
         break;
      case QNetworkReply::ProxyAuthenticationRequiredError:
         break;
      case QNetworkReply::UnknownProxyError:
         break;
      // content errors (201-299):
      case QNetworkReply::ContentAccessDenied:
         break;
      case QNetworkReply::ContentOperationNotPermittedError:
         break;
      case QNetworkReply::ContentNotFoundError:
         // Заносим запрос в кеш для последующей проверки, хост обрезаем.
         m_ContentNotFoundErrorCache.insert(query(Url), NULL, 1);
         break;
      case QNetworkReply::AuthenticationRequiredError:
         break;
      case QNetworkReply::ContentReSendError:
         break;
      case QNetworkReply::UnknownContentError:
         break;
      // protocol errors
      case QNetworkReply::ProtocolUnknownError:
         break;
      case QNetworkReply::ProtocolInvalidOperationError:
         break;
      case QNetworkReply::ProtocolFailure:
         break;
      default:
         break;
         }
      }

   // We receive ownership of the reply object and therefore need to handle deletion.
   m_ReplyMapMutex.lock();
   m_NetworkReplies.remove(NetworkReply_);
   delete ReplyParameters;
   NetworkReply_->deleteLater();
   m_ReplyMapMutex.unlock();
   // вся очередь пуста?
   if(queueSize() == 0) m_AllAbortLoading = false;
   // размер текущей очереди
   int QueueSize = queueSize(MapAdapter);
   if(QueueSize == 0) {
      if(MapControl && MapControl->abortLoading()) MapControl->setAbortLoading(false);
      else if(w_Parent) w_Parent->loadingQueueEmpty();
      }
   // сигнал об изменении размера очереди
   emit(sizeQueueRequestsChanged(MapAdapter, QueueSize)); 
   }

   //----------------------------------------------------------------------
   void TMapNetwork::abortLoading(TMapAdapter* MapAdapter_)
   {
   TMapControl* MapControl = MapAdapter_ ? MapAdapter_->mapControl() : NULL;
   //
   m_ReplyMapMutex.lock();
   //
   QVector<QNetworkReply*> Replies;
   QMap<QNetworkReply*, TReplyParameters*>::const_iterator it;
   for(it = m_NetworkReplies.begin(); it != m_NetworkReplies.end(); ++it) {
      if(!MapAdapter_ || !MapControl || it.value()->mapControl() == MapControl) {
         if(!MapAdapter_ || !MapControl) m_AllAbortLoading = true;
         else                            MapControl->setAbortLoading(true);
         Replies.push_back(it.key());
         }
      }
   //
   m_ReplyMapMutex.unlock();
   //
   if(Replies.size()) {
      QNetworkReply* Reply;
      foreach(Reply, Replies) Reply->abort();
      }
   }

   //----------------------------------------------------------------------
   bool TMapNetwork::imageIsLoading(const QString& Url_)
   {
   QMutexLocker Locker(&m_ReplyMapMutex);
   //
   QMap<QNetworkReply*, TReplyParameters*>::const_iterator it;
   for(it = m_NetworkReplies.begin(); it != m_NetworkReplies.end(); ++it) {
      // перед сравнением обрезаем начало, ведущее к разным источникам одного типа карт одного провайдера
      QString S1 = it.value()->url();
      int IndexPoint = S1.indexOf(".");
      S1.remove(0, IndexPoint);
      QString S2 = Url_;
      IndexPoint = S2.indexOf(".");
      S2.remove(0, IndexPoint);
      if(S1 == S2) return true;
      }
   return false;
   }

   //----------------------------------------------------------------------
   void TMapNetwork::setProxy( const QString& Host_, const int Port_ )
   {
   QNetworkProxy Proxy(QNetworkProxy::HttpCachingProxy, Host_, Port_);
   w_NetworkAccessManager->setProxy(Proxy);
   gProxyIsSet = true;
   }

   //----------------------------------------------------------------------
   void TMapNetwork::unSetProxy( void )
   {
   gProxyIsSet = false;
   }

   //----------------------------------------------------------------------
   void TMapNetwork::setCacheDir(const QDir& Path_)
   {
   if(!Path_.exists()) Path_.mkpath(Path_.absolutePath());
   if(w_NetworkDiskCache == NULL) {
      w_NetworkDiskCache = new QNetworkDiskCache();
      w_NetworkAccessManager->setCache(w_NetworkDiskCache);
      }
   w_NetworkDiskCache->setCacheDirectory(Path_.absolutePath());
   w_NetworkDiskCache->clear();
   w_NetworkDiskCache->setMaximumCacheSize(1024 * 1024 * 100);
   }

   //----------------------------------------------------------------------
   void TMapNetwork::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
   {
   Q_UNUSED(proxy)

   QDialog dialog;
   QGridLayout layout;
   QLabel username, password;
   username.setText("Username:");
   password.setText("Password:");
   layout.addWidget(&username, 0, 0);
   layout.addWidget(&password, 1, 0);
   QLineEdit user, pass;
   pass.setEchoMode(QLineEdit::Password);
   QNM_DEBUG_CHECK(connect(&user, SIGNAL(returnPressed()), &dialog, SLOT(accept())));
   QNM_DEBUG_CHECK(connect(&pass, SIGNAL(returnPressed()), &dialog, SLOT(accept())));
   layout.addWidget(&user, 0, 1);
   layout.addWidget(&pass, 1, 1);
   QPushButton button;
   button.setText("OK");
   QNM_DEBUG_CHECK(connect(&button, SIGNAL(clicked()), &dialog, SLOT(accept())));
   layout.addWidget(&button, 2, 0, 1, 2, Qt::AlignCenter);
   dialog.setLayout(&layout);
   dialog.exec();
   authenticator->setUser(user.text());
   authenticator->setPassword(pass.text());
   }

   //------------------------------------------------------------
   int TMapNetwork::loadProxyList(const QString& FileName_, const unsigned int MSecs_)
   {
   QFile File(FileName_);
   if(!File.open(QIODevice::ReadOnly | QIODevice::Text))
      return 0;

   QList<TCheckProxy*> ThreadsList;

   QTextStream TextStream(&File);
   while(!TextStream.atEnd()) {
      QString Line = TextStream.readLine();
      Line.replace(" ", "");
      QStringList Words = Line.split(":");
      if(Words.size() == 2) {
         m_ProxyAddressList.push_back(new TProxyAddress(Words.first(), Words.last().toInt()));
         ThreadsList.push_back(new TCheckProxy(m_ProxyAddressList.last(), MSecs_));
         }
      }
   if(ThreadsList.size() > 0) {
      // стартуем все потоки проверки прокси
      foreach(TCheckProxy* Thread, ThreadsList) Thread->start();
      // ждем завершения всех потоков
      while((*ThreadsList.begin())->getThreadsCount()) { TSleep::msleep(200); } 
      // удаляем неработающие прокси
      QMutableListIterator<TProxyAddress*> it(m_ProxyAddressList);
      while(it.hasNext()) {
         if(!(it.next()->isValid)) it.remove();
         }
      // удаляем потоки
      foreach(TCheckProxy* Thread, ThreadsList) delete Thread;
      }
   return m_ProxyAddressList.size();
   }

   //------------------------------------------------------------
   int TMapNetwork::queueSize(TMapAdapter* MapAdapter_) 
   {
   int ReplyCount = 0;
   if(!MapAdapter_ || TMapAdapter::mapAdapterIsActive(MapAdapter_)) {
      QMutexLocker Locker(&m_ReplyMapMutex);
      ReplyCount = m_NetworkReplies.size();
      if(MapAdapter_ && ReplyCount) {
         TMapControl* MapControl = MapAdapter_->mapControl();
         // размер очереди для указанного управляющего класса
         ReplyCount = 0;
         QMap<QNetworkReply*, TReplyParameters*>::const_iterator it;
         for(it = m_NetworkReplies.begin(); it != m_NetworkReplies.end(); ++it) {
            if(TMapAdapter::mapAdapterIsActive(it.value()->mapAdapter()) &&
               it.value()->mapControl() == MapControl) ReplyCount++;
            }
         }
      }
   return ReplyCount;
   }

   //------------------------------------------------------------
   bool TMapNetwork::contentNotFound(QString Url_) 
   { 
   return m_ContentNotFoundErrorCache.contains(query(Url_)); 
   }

   //------------------------------------------------------------
   QString TMapNetwork::query(QString Url_) 
   { 
   if(Url_.isEmpty()) return Url_;
   return Url_.right(Url_.size() - Url_.indexOf('/', 10));
   }

}
