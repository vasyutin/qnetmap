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

#ifndef QNETMAP_TILEMAPADAPTER_H
#define QNETMAP_TILEMAPADAPTER_H

#include "qnetmap_global.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_tilemapadapterparameters.h"
#include "qnetmap_imagemanager.h"

namespace qnetmap
{
   //! \class TTileMapAdapter
   //! \brief Базовый класс для всех карт
   class QNETMAP_EXPORT TTileMapAdapter : public TMapAdapter
   {
      Q_OBJECT

   public:
      virtual ~TTileMapAdapter() {};

      virtual QString host(const bool Random_ = false) const;
      virtual unsigned hostsCount(void) const;
      //! \brief Установить границы масштабирования
      //! \param int Min_ - минимальный масштаб
      //! \param int Max_ - максимальный масштаб
      virtual void setZoomRestrictions(const int Min_, const int Max_);
      //! \brief Установить шаблоны хоста и запроса
      //! \param QString Host_ - шаблон имени хоста
      //! \param QString QueryTemplate_ - шаблон запроса
      virtual void setURL(const QString& Host_, const QString& QueryTemplate_);
      //! \brief Признак привязки карты к системе координат
      virtual bool isGeocoded(void) const { return true; }
      //! \brief Центральная точка карты
      virtual QPointF centerMap(void) const { return QPointF(0, 0); }
      //! \brief Верхний левый угол карты
      virtual QPointF topLeftMap(void) const { return QPointF(-180, 90); }
      //! \brief Нижний правый угол карты
      virtual QPointF bottomRightMap(void) const { return QPointF(180, -90); }
      //! \brief Получить версию карт
      //! \return Версия карт
      virtual QString mapVersion(void) const { return m_MapVersion; }
      //! \brief Устанавливает версию карты
      //! \param MapVersion_ - версия карты 
      //! \param ToSave_ - если true, то обязательно сохранить в реестре
      virtual void setMapVersion(const QString& Version_, const bool ToSave_ = false);
      //! \brief Установить версию карт из указанного интернет-источниа
      //! \param QString DefaultVersion_ - версия карт по умолчанию
      //! \param QString Url_ - источник
      //! \param QString VersionPrefix_ - часть строки непосредственно перед версией
      //! \param QString VersionPostfix_ - часть строки сразу за версией
      virtual void setMapVersion(const QString& DefaultVersion_,
                                 const QString& Url_,
                                 const QString& VersionPrefix_,
                                 const QString& VersionPostfix_);
      //! \brief установка параметров адаптера тайловой карты
      virtual void setParameters(TMapAdapterParameters* Parameters_);
      //! \brief 
      virtual void setImageManagerInstance(TImageManager* Instance_) { TImageManager::setInstance(Instance_); }

   private:
      //! \var Версия карт
      QString m_MapVersion;
      //! \var Имя временного файл для поиска в нем версии карт
      QString m_TemporaryFileName;
      //! \var Префикс версии в ответе интернет-сервера
      QString m_VersionPrefix;
      //! \var Постфикс версии в ответе интернет-сервера
      QString m_VersionPostfix;
      //! \var Шаблон запроса тайлов
      QString m_QueryTemplate;
      //! \var Служебный массив для ускорения формирования запросов
      int m_Order[5][2];
      //! \var Минимальный номер, подставляемый в шаблон имени хоста
      int m_HostNumberMinimum;
      //! \var Максимальный номер, подставляемый в шаблон имени хоста
      int m_HostNumberMaximum;
      //! \var признак того, что имя хоста является шаблоном
      bool m_HostIsTemplate;
      //! \var ширина поля в запросе под координату (с лидирующими пробелами)
      uint m_CoordinatesWidth;
      //! \var ширина поля в запросе под масштаб (с лидирующими пробелами)
      uint m_ZoomWidth;
      //! \var признак наличия в запросе quadkey (нумерация тайлов в virtualearth)
      bool m_QuadKeyThere;

   protected:
      //! \var Шаблон имени хоста с картами
      QString m_Host;

      //! \brief Конструктор класса
      //! \param QString Host_ - хост провайдера карты
      //! \param QString QueryTemplate_ - строка запроса, добавляется к хосту
      //!                              %1 заменяется на номер тайла по x
      //!                              %2 заменяется на номер тайла по y
      //!                              %3 заменяется на масштаб
      //! \param int TileSizeX - размер стороны тайла по оси Х в пикселах
      //! \param int TileSizeY - размер стороны тайла по оси Y в пикселах
      //! \param int MinZoom_ - минимальный масштаб карты
      //! \param int MaxZoom_ - максимальный масштаб карты
      TTileMapAdapter(
         const QString& Host_, const QString& QueryTemplate_, 
         const int TileSizeX, const int TileSizeY, 
         const int MinZoom_ = 0, const int MaxZoom_ = 12,
         const int HostNumberMinimum_ = 0, const int HostNumberMaximum_ = 0);
      TTileMapAdapter(const TTileMapAdapter& Source_);

      //! \brief Округление числа
      //! \param qreal Number_ - число
      //! \return int - округленное число
      int round(const qreal Number_) const;
      //! \brief Гиперболический арктангенс
      //! \param qreal - аргумент
      //! \return qreal - гиперболический арктангенс
      qreal arcTanh(const qreal) const;
      //! \brief Формирование строки для запроса тайла 
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      virtual QString query(const int x, const int y, const int z) const;
      //! \brief Формирование номера тайла в бинарном дереве
      //! \param int x - номер тайла по x
      //! \param int y - номер тайла по y
      //! \param int z - масштаб
      virtual QString quadkey(const int x, const int y, const int z) const;
      //! \brief Приведение масштаба
      //! \param int Z_ - масшаб
      //! \return int - приведенный масштаб
      virtual int zoom(int z) const;

   private slots:
      //! \brief Ловит событие получения html страницы с версией карты
      void htmlReceived(void);
   };
}

#endif
