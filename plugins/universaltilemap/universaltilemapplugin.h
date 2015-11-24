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

#ifndef TUNIVERSALTILESMAPPLUGIN_H
#define TUNIVERSALTILESMAPPLUGIN_H

#include <QObject>

#include <qnetmap_universaltilemapadapter.h>

#include <qnetmap_mapplugin.h>

using namespace qnetmap;

class TUniversalTileMapPlugin : public TMapPlugin<TUniversalTileMapAdapter>
{
   Q_OBJECT;
   Q_PLUGIN_METADATA(IID IMapAdapterInterface_IID)
   Q_INTERFACES(qnetmap::IMapAdapterInterface)

public:
   TUniversalTileMapPlugin();
   TUniversalTileMapPlugin(const TUniversalTileMapPlugin& Source_) 
      : TMapPlugin<TUniversalTileMapAdapter>(Source_) { setPlugin(this); }
   virtual ~TUniversalTileMapPlugin() {};

   //! \brief Создает новый объект класса плагина
   //!        Каждый плагин обязан реализовать этот метод
   //! \return QObject* - указатель на интерфейс плагина,
   //!                    для преобразования надо использовать qobject_cast<IMapAdapterInterface *>
   QObject* createInstance() { return new TUniversalTileMapPlugin(*this); }
};

#endif
