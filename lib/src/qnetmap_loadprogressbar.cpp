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

#include "qnetmap_loadprogressbar.h"
#include "qnetmap_imagemanager.h"
#include "qnetmap_mapnetwork.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_mapcontrol.h"

namespace qnetmap
{
   //----------------------------------------------------------------------
   TLoadProgressBar::TLoadProgressBar(TMapControl* MapControl_, QWidget *Parent_) 
      : QProgressBar(Parent_),  m_MapControl(MapControl_), 
#ifdef QNETMAP_QT_4
      w_ProgressBarStyle(new QPlastiqueStyle())
#else
      w_ProgressBarStyle(QStyleFactory::create("plastique"))
#endif
   {
   QNM_DEBUG_CHECK(connect(TImageManager::instance()->mapNetwork(), SIGNAL(sizeQueueRequestsChanged(TMapAdapter*, int)),
                  this,                                    SLOT(queueSizeChangeg(TMapAdapter*, int))));
   setMinimum(0);
   setMaximum(0);
   setStyle(w_ProgressBarStyle);
   hide();
   }

   //----------------------------------------------------------------------
   void TLoadProgressBar::queueSizeChangeg(TMapAdapter* MapAdapter_, int Size_)
   {
   if(MapAdapter_ &&
      TMapAdapter::mapAdapterIsActive(MapAdapter_) &&
      m_MapControl == MapAdapter_->mapControl()) {
      if(Size_ == 0) {
         // очередь обнулилась - скрываем
         hide();
         setMaximum(0);
         }
      else {
         if(Size_ > maximum()) setMaximum(Size_); // увеличиваем максимум, если очередь растет
         setValue(maximum() - Size_);             // устанавливаем значение
         show();
         }
      }
   }
}
