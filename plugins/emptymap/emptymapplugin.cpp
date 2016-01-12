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

#include "emptymapplugin.h"

TEmptyMapPlugin::TEmptyMapPlugin()
   : TMapPlugin<TEmptyMapAdapter>(TImageManager::Consts::StorageOnly, false)
{
setPluginName(tr("Empty Map"));
setTransparency(TMapAdapter::Consts::TransparentMap);
setMapType(Consts::TypeEmptyMap);
setDescription(QNetMapTranslator::tr("Empty Map"));
}

#ifdef QNETMAP_QT_4
   Q_EXPORT_PLUGIN2(emptymapplugin, TEmptyMapPlugin)
#endif
