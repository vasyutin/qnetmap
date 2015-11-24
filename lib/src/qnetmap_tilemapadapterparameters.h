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

#ifndef QNETMAP_TILEMAPADAPTERPARAMETERS_H
#define QNETMAP_TILEMAPADAPTERPARAMETERS_H

#include "qnetmap_global.h"
#include "qnetmap_mapadapterparameters.h"

class QString;

namespace qnetmap 
{
   //! \class TTileMapAdapterParameters
   //! \brief управляет параметрами тайловой карты
   class TTileMapAdapterParameters : public TMapAdapterParameters
   {
   public:
      TTileMapAdapterParameters();
      virtual ~TTileMapAdapterParameters() {}

      // методы, возвращающие необходимые свойства
      QString hostTemplate(void) const { return m_HostTemplate; }
      uint    hostNumberMinimum(void) const { return m_HostNumberMinimum; }
      uint    hostNumberMaximum(void) const { return m_HostNumberMaximum; }
      QString queryTemplate(void) const { return m_QueryTemplate; }
      QString defaultMapVersion(void) const { return m_DefaultMapVersion; }
      QString mapVersionUrl(void) const { return m_MapVersionUrl; }
      QString mapVersionPrefix(void) const { return m_MapVersionPrefix; }
      QString mapVersionPostfix(void) const { return m_MapVersionPostfix; }
      uint    coordinatesWidth(void) const { return m_CoordinatesWidth; }
      uint    zoomWidth(void) const { return m_ZoomWidth; }
      // методы, устанавливающие свойства с необходимыми проверками
      bool setHostTemplate(const QString& HostTemplate_);
      bool setHostNumberMinimum(const uint HostNumberMinimum_);
      bool setHostNumberMaximum(const uint HostNumberMaximum_);
      bool setQueryTemplate(const QString& QueryTemplate_);
      bool setDefaultMapVersion(const QString& DefaultMapVersion_);
      bool setMapVersionUrl(const QString& MapVersionUrl_);
      bool setMapVersionPrefix(const QString& MapVersionPrefix_);
      bool setMapVersionPostfix(const QString& MapVersionPostfix_);
      bool setCoordinatesWidth(const uint CoordinatesWidth_);
      bool setZoomWidth(const uint ZoomWidth_);
      //! \brief проверка на правильность ввода данных в класс
      bool isValid(void) const;
      //! \brief возвращает true, если введены данные для получения номера версии карт
      bool checkMapVersion(void) const { return m_CheckMapVersion; }
      //! \brief возвращает true, если строка описания хоста является шаблоном
      bool hostIsTemplate(void) const { return m_HostIsTemplate; }

   private:
      //! \var шаблон имени хоста карты
      QString m_HostTemplate;
      //! \var минимальный номер хоста карты
      uint m_HostNumberMinimum;
      //! \var максимальный номер хоста карты
      uint m_HostNumberMaximum;
      //! \var шаблон запроса тайлов карты
      QString m_QueryTemplate;
      //! \var версия карты по умолчанию
      QString m_DefaultMapVersion;
      //! \var url страницы, содержащей версию карты
      QString m_MapVersionUrl;
      //! \var строка перед версией карты
      QString m_MapVersionPrefix;
      //! \var строка после версии карты
      QString m_MapVersionPostfix;
      //! \var признак ввода необходимых параметров для проверки версии карты
      bool m_CheckMapVersion;
      //! \var признак того, что строка хоста является шаблоном
      bool m_HostIsTemplate;
      //! \var ширина поля в запросе под координату (с лидирующими пробелами)
      uint m_CoordinatesWidth;
      //! \var ширина поля в запросе под масштаб (с лидирующими пробелами)
      uint m_ZoomWidth;
   };

   typedef QList<TTileMapAdapterParameters> TTileMapAdapterParametersList;
}
#endif
