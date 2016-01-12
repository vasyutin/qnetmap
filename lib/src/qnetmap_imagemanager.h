/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl ImageManager code by Kai Winter
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

#ifndef QNETMAP_IMAGEMANAGER_H
#define QNETMAP_IMAGEMANAGER_H

#include <QObject>
#include <QPixmapCache>
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QFileInfo>
#include <QPainter>

#include "qnetmap_global.h"

namespace qnetmap
{
   class TMapNetwork;
   class TMapAdapter;

   //! \class TImageManager
   //! \brief Класс, управляющий получением тайлов
   class QNETMAP_EXPORT TImageManager: public TBase
   {
   Q_OBJECT

   public:
      typedef TBase TParent;
      //! \struct Consts
      //! \brief Константы типов источников тайлов
      struct Consts {
         static const int InternetOnly       = 1; //!< только интернет
         static const int StorageOnly        = 2; //!< только хранилище
         static const int InternetAndStorage = 3; //!< хранилище и интернет
         static const int RasterMap          = 4; //!< растровая карта
      };

      //! \brief Получить указатель на единственную копию класса
      //! \return TImageManager* - указатель на единственную копию класса
      static TImageManager* instance()
      {
      if(!w_Instance) w_Instance = new TImageManager; // создаем, если еще не создан
      return w_Instance;
      }
      //! \brief 
      static void setInstance(TImageManager* Instance_);

      virtual ~TImageManager();

      //! \brief Получить рисунок
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты
      //! \param int X_ - номер тайла по х
      //! \param int Y_ - номер тайлп по y
      //!                 масштаб берется текущий
      //! \return QPixmap - Полученный тайл (если осуществляется запрос, возвращается пустой QPixmap())
      QPixmap getImage(const TMapAdapter* MapAdapter_, const int X_, const int Y_);
      //! \brief Получить рисунок
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты
      //! \param int X_ - номер тайла по х
      //! \param int Y_ - номер тайлп по y
      //! \param int Z_ - масштаб
      //! \return QPixmap - Полученный тайл (если осуществляется запрос, возвращается пустой QPixmap())
      QPixmap getImage(const TMapAdapter* MapAdapter_, const int X_, const int Y_, const int Z_);
      //! \brief Выдача сообщения о получении тайла из интернет
      void receivedImage(void);
      //! \brief Выдача сообщения о получении html из интернет
      void receivedHtml(void);
      //! \brief Прервать все загрузки на указанной карте
      //         This is useful when changing the zoom-factor, though newly needed images loads faster
      //! \param TMapAdapter* MapAdapter_ - указатель на класс - адаптер карты
      void abortLoading(TMapAdapter* MapAdapter_ = NULL);
      /*!
      * This method is called by MapNetwork, after all images in its queue were loaded.
      * The ImageManager emits a signal, which is used in MapControl to remove the zoom image.
      * The zoom image should be removed on Tile Images with transparency.
      * Else the zoom image stay visible behind the newly loaded tiles.
      */
      void loadingQueueEmpty(void);
      //! sets the proxy for HTTP connections
      /*!
      * This method sets the proxy for HTTP connections.
      * This is not provided by the current Qtopia version!
      * @param host the proxy?s hostname or ip
      * @param port the proxy?s port
      */
      void setProxy(const QString& Host_, int Port_);
      void unSetProxy(void);

      //! sets the cache directory for persistently saving map tiles
      /*!
      * @param path the path where map tiles should be stored
      * @todo add maximum size
      */
      void setCacheDir(const QDir& path);
      //! \brief уникальный номер
      int uniqueNumber(void) { return m_UniqueNumber.fetchAndAddOrdered(1); }

      //! \brief Получить указатель на класс, осуществляющий сетевой обмен
      //! \return TMapNetwork* - указатель на класс, осуществляющий сетевой обмен
      TMapNetwork* mapNetwork(void) { return w_MapNetwork; }

	  //! \brief Возвращает признак загрузки тайлов
	  bool loadTiles(void) const { return m_LoadTiles; }
	  //! \brief Устанавливает признак загрузки тайлов
	  void setLoadTiles(const bool LoadTiles_ = true) { m_LoadTiles = LoadTiles_; }

   private:
      explicit TImageManager(TParent* Parent_ = NULL);
      Q_DISABLE_COPY(TImageManager)

      //! \var Пустой QPixmap
      QPixmap m_EmptyPixmap;
      //! \var Указатель на класс, осуществляющий сетевой обмен
      TMapNetwork* w_MapNetwork;

      //! \var Указатель на единственную копию данного класса
      static TImageManager* w_Instance;
      //! \var уникальный номер
      QAtomicInt m_UniqueNumber;
	  //! \var Признак, отечающий за загрузку тайлов
	  bool m_LoadTiles;

   signals:
      //! \brief Сигнал о получении тайла из интернет
      void imageReceived();
      //! \brief Сигнал о получении html из интернет
      void htmlReceived();
      //! \brief Сигнал о завершении всех текущих загрузок
      void loadingFinished();
   };
}
#endif
