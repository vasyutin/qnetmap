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

#ifndef QNETMAP_CHECKPROXY_H
#define QNETMAP_CHECKPROXY_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

#include "qnetmap_global.h"

namespace qnetmap
{
   //! \class TProxyAddress
   //! \brief Структура, содержащая параметры прокси-сервера
   struct QNETMAP_EXPORT TProxyAddress 
   {
   QString host;
   int     port;
   bool    isValid;

   TProxyAddress(const QString& Host_, const int Port_, const bool IsValid_ = false)
      : host(Host_), port(Port_), isValid(IsValid_) {};
   };

   //! \class TCheckProxy
   //! \brief Проверка списка прокси-серверов на валидность в несколько потоков
   class QNETMAP_EXPORT TCheckProxy : public QThread
   {
   public: 
      //! \brief Конструктор
      //! \param TProxyAddress* ProxyAddress_ - параметры прокси
      //! \param unsigned int MSecs_ - время ожидания ответа в милисекундах
      explicit TCheckProxy(TProxyAddress* ProxyAddress_, unsigned int MSecs_ = 1000) 
         : m_ProxyAddress(ProxyAddress_), m_MSecsWaitForConnected(MSecs_) { incrementThreadsCount(); } 
      virtual ~TCheckProxy() {}

      //! \brief Получить количество потоков
      int getThreadsCount() { QMutexLocker Locker(&m_Mutex); return m_ThreadsCount; }

   private:
      //! \brief Запуск
      void run();
      //! \brief Увеличить количество потоков на 1
      void incrementThreadsCount() { QMutexLocker Locker(&m_Mutex); m_ThreadsCount++; }
      //! \brief Уменьшить количество потоков на 1
      void decrementThreadsCount() { QMutexLocker Locker(&m_Mutex); m_ThreadsCount--; }

      //! \var Параметры прокси
      TProxyAddress* m_ProxyAddress;
      //! \var Количество работающих потоков
      static int m_ThreadsCount;
      //! \var Мьютекс доступа к свойству m_ThreadsCount
      static QMutex m_Mutex;
      //! \var Время ожидания ответа в милисекундах
      unsigned int m_MSecsWaitForConnected;
   };
}
#endif
