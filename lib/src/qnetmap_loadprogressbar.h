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

#ifndef QNETMAP_LOADPOGRESSBAR_H
#define QNETMAP_LOADPOGRESSBAR_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QProgressBar>
   #include <QPlastiqueStyle>
#endif

namespace qnetmap
{
   class TMapAdapter;
   class TMapControl;

   //! \class TLoadProgressBar
   //! \brief Класс прогресс-бара, отображающего процесс загрузки тайлов.
   //!        Его достаточно только поместить в нужное место и задать размер.
   //!        Прогресс-бар ловит сигналы и отображает полосу загрузки.
   class QNETMAP_EXPORT TLoadProgressBar : public QProgressBar
   {
      Q_OBJECT

   public:
      explicit TLoadProgressBar(TMapControl* MapControl_, QWidget *parent = 0);
      virtual ~TLoadProgressBar() {
         delete w_ProgressBarStyle;
         }

   private:
      Q_DISABLE_COPY(TLoadProgressBar)

      TMapControl* m_MapControl;
      QStyle *w_ProgressBarStyle;

   public slots:
      //! \brief Слот обработки сигналов изменения очереди загрузки тайлов
      //! \param TMapAdapter* MapAdapter_ - указатель на адаптер карты
      //! \param int Size_ - размер очереди
      void queueSizeChangeg(TMapAdapter* MapAdapter_, int Size_);
   };
}
#endif
