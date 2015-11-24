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

#ifndef QNETMAP_MAPNETWORK_H
#define QNETMAP_MAPNETWORK_H


#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QDialog>
   #include <QLabel>
   #include <QPushButton>
   #include <QLineEdit>
   #include <QGridLayout>
#endif
#include <QObject>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QVector>
#include <QPixmap>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include <QImageReader>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QCache>
 
#include "qnetmap_checkproxy.h"
#include "qnetmap_pngimage.h"
#include "qnetmap_httpparameters.h"

namespace qnetmap
{
   class TImageManager;
   class TMapAdapter;
   class TMapControl;

   //! \class TReplyParameters
   //! \brief Класс для хранения параметров http-запроса
   class TReplyParameters : public TBase
   {
       Q_OBJECT

   public:
      typedef TBase TParent;
      explicit TReplyParameters(TParent *Parent_ = NULL)
         : TParent(Parent_), m_Reply(NULL), m_MapAdapter(NULL), m_Url(""), m_Path(""),
           m_CacheInMappingStorage(false),
           m_MapControl(NULL),
           m_Finished(false) {}
      TReplyParameters(QNetworkReply* Reply_,
                       TMapAdapter* MapAdapter_, const QString& Url_, const QString& Path_ = "",
                       bool CacheInMappingStorage_ = false,
                       const QString& MapVersion_ = "",
                       TParent *Parent_ = NULL);
      virtual ~TReplyParameters() {}

      //! \brief Получить URL http-запроса
      //! \return URL http-запроса
      QString url(void) const { return m_Url; }
      //! \brief Получить указатель на адаптер карты
      //! \return Указатель на адаптер карты
      TMapAdapter* mapAdapter(void) const { return m_MapAdapter; }
      //! \brief Получить признак сохранения тайла в хранилище
      //! \return Признак сохранения тайла в хранилище
      bool cacheInMappingStorage(void) const { return m_CacheInMappingStorage; }
      //! \brief Получить путь сохранения результата
      //! \return Путь сохранения результата
      QString path(void) const { return m_Path; }
      //! \brief Получить указатель на управляющий класс
      //! \return Указатель на управляющий класс
      TMapControl* mapControl(void) const { return m_MapControl; }
      //! \brief Получить версию карт
      QString mapVersion(void) const { return m_MapVersion; }
      //! \brief Установка признака завершения запроса
      inline void setFinished(void) { m_Finished = true; }

   private:
      Q_DISABLE_COPY(TReplyParameters)

      //! \var Указатель на запрос
      QNetworkReply* m_Reply;
      //! \var Указатель на адаптер карты
      TMapAdapter* m_MapAdapter;
      //! \var URL запроса
      QString m_Url;
      //! \var Путь, по которому необходимо сохранить тайл
      QString m_Path;
      //! \var Признак сохранения тайла в хранилище
      bool m_CacheInMappingStorage;
      //! \var Указатель на управляющий класс
      TMapControl* m_MapControl;
      //! \var Версия карт
      QString m_MapVersion;
      //! \var Признак завершения запроса
      bool m_Finished;

   private slots:
      //! \brief Interrupt a long running query
      void replyAbort() { if(!m_Finished) m_Reply->abort(); }
   };

   //! \class TMapNetwork
   //! \brief Класс, реализующий загрузку тайлов из сети
   class QNETMAP_EXPORT TMapNetwork : public TBase
   {
       Q_OBJECT

   public:
      explicit TMapNetwork(TImageManager* Parent_ = NULL);
      virtual ~TMapNetwork();

      //! \brief Загрузить url из интернет и сохранить по указанному пути
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты
      //! \param QString Url_ - запрос
      //! \param QString FilePath_ - путь хранения
      //! \param bool CacheInMappingStorage_ - признак сохранения тайла в хранилище
      //! \param QString MapVersion_ - версия карт
      void loadURL(const TMapAdapter* MapAdapter_, const QUrl Url_,
                   const QString& FilePath_ = "",
                   const bool CacheInMappingStorage_ = false,
                   const QString& MapVersion_ = "");
      /*!
       * checks if the given url is already loading
       * @param url the url of the image
       * @return boolean, if the image is already loading
       */
      bool imageIsLoading(const QString& url);
      //! \brief Прервать все загрузки для управляющего класса, содержащего указанную карту
      //         This is useful when changing the zoom-factor, though newly needed images loads faster
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты,
      //!                                   если NULL - прерываются все загрузки
      void abortLoading(TMapAdapter* MapAdapter_ = NULL);
      //! \brief Установить текущий прокси для запросов
      //! \param QString host - хост прокси
      //! \param int port - порт прокси
      void setProxy(const QString& Host_, const int Port_);
      void unSetProxy(void);
      //! \brief Установить путь нахождения локального кеша
      //! \param QDir Path_ - Путь локального кеша
      void setCacheDir(const QDir& Path_);
      //! \brief Добавить прокси в список 
      //! \param QString host - хост прокси
      //! \param int port - порт прокси
      //void addProxy(const QString& host, const int port) 
      //   { m_ProxyAddressList.push_back(new TProxyAddress(host, port)); }
      //! \brief Загрузка из файла списка прокси-серверов с проверкой их работоспособности
      //! \param QString FilePath_ - текстовый файл со списком серверов
      //!                            в формате сервер:порт построчно
      //!                            сервер может быть как IP-адресом, так и доменным именем
      //! \param unsigned int MSecs_ - ожидание в миллисекундах, ответ в течении которых 
      //!                              включает сервер в список
      //! \return int - количество сохраненных работающих прокси
      int loadProxyList(const QString& FileName_, const unsigned int MSecs_ = 1000);
      //! \brief Изменить признак использования списка прокси
      //! \param bool Use_ - Признак использования списка прокси
      void useProxyAddressList(const bool Use_ = false) { m_UseProxyAddressList = Use_; }
      //! \brief Получить признак использования списка прокси 
      //! \return bool - Признак использования списка прокси
      bool usedProxyAddressList(void) const { return m_UseProxyAddressList; }
      //! \brief Получить указатель на класс QNetworkAccessManager
      //! \return QNetworkAccessManager* - Указатель на класс QNetworkAccessManager
      QNetworkAccessManager* networkAccessManager(void) const { return w_NetworkAccessManager; }
      //! \brief Получить указатель на класс NetworkDiskCache
      //! \return QNetworkAccessManager* - Указатель на класс NetworkDiskCache
      QNetworkDiskCache* networkDiskCache(void) const { return w_NetworkDiskCache; }
      //! \brief Была ошибка 203?
      bool contentNotFound(QString Url_);
      //! \brief 
      static THttpParametersList& httpParametersList(void) { return m_HttpParametersList; }

   private:
      Q_DISABLE_COPY(TMapNetwork)

      //! \brief Получить размер очереди http-запросов для управляющего класса,
      //!        который управляет указанным адаптером карты.
      //!        Если адаптер не указан, возвращает размер всей очереди.
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты
      //! \return int - Размер очереди http-запросов
      int queueSize(TMapAdapter* MapAdapter_ = NULL);
      //! \brief Выделяет из url запрос (без хоста)
      QString query(QString Url_);

      //! \var Родитель
      TImageManager* w_Parent;
      //! \var Указатель на класс, обеспечивающий доступ в сеть
      QNetworkAccessManager* w_NetworkAccessManager;
      //! \var Указатель на класс дискового кеша
      QNetworkDiskCache* w_NetworkDiskCache;
      //! \var Список параметров запросов, отсортированный по действующим запросам
      QMap<QNetworkReply*, TReplyParameters*> m_NetworkReplies;
      //! \var Мьютекс работы со списками запросов
      QMutex m_ReplyMapMutex;
      //! \var Список проверенных действующих прокси-серверов
      QList<TProxyAddress*> m_ProxyAddressList;
      //! \var Признак использования списка прокси-серверов
      bool m_UseProxyAddressList;
      //! \var Признак процесса прерывания всех загрузок
      bool m_AllAbortLoading;
      //! \var Список запросов, закончившихся ошибкой ContentNotFoundError
      QCache<QString, int> m_ContentNotFoundErrorCache;
      //! \var 
      static THttpParametersList m_HttpParametersList;

   signals:
      //! \brief Сигнал изменения очереди загрузки тайлов
      //! \param TMapAdapter* MapAdapter_ - указатель на адаптер карты
      //! \param int QueueSize_ - размер очереди
      void sizeQueueRequestsChanged(TMapAdapter* MapAdapter_, int QueueSize_);

   private slots:
      //! \brief Показ диалога запроса логина и пароля для регистрации на прокси
      void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator *authenticator);
      //! \brief Обработка сигнала завершения загрузки тайла
      //! \param QNetworkReply* - указатель на запрос
      void requestFinished(QNetworkReply*);
   };
}
#endif
