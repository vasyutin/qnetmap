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

#ifndef QNETMAP_RULERSCALING_H
#define QNETMAP_RULERSCALING_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QWidget>
   #include <QVBoxLayout>
   #include <QPushButton>
   #include <QSlider>
#endif
#include <QPainter>

class QEvent;

#include "qnetmap_global.h"

namespace qnetmap
{
	class TMapWidget;

	//----------------------------------------------
   class TPushButton : public QPushButton {
      Q_OBJECT

   public:

   signals: 
      void buttonPressed(void);

   private:
      void mousePressEvent(QMouseEvent *Event_) { emit buttonPressed(); }
   };

   //////////////////////////////////////////////////////////////////////////
   class QNETMAP_EXPORT TRulerScaling : public QWidget
   {
      Q_OBJECT

   public:

      class TRulerScalingSlider : public QSlider 
      {
      public:
         //! \class Слайдер со своим обработчиком событий рисования
         explicit TRulerScalingSlider(Qt::Orientation Orientation_, QWidget *Parent_ = 0) : 
            QSlider(Orientation_, Parent_), w_Parent(Parent_)  {}
         virtual ~TRulerScalingSlider() {}

      private:
         QPainter m_Painter;
         QWidget* w_Parent;

      protected:
         virtual void paintEvent(QPaintEvent* Event_);
      };

      TRulerScaling(QWidget *Parent_ = 0);
      virtual ~TRulerScaling() { if(w_Slider) delete w_Slider; };

      //! \brief Устанавливает границы изменения масштаба
      virtual void setInterval(const int Minimum_, const int Maximum_);
      //! \brief Устанавливает текущее значение масштаба
      virtual void setValue(int Value_) { w_Slider->setValue(Value_); } 
		//! \brief 
		virtual void hideFitToPageButton(void) { m_FitToPageButton.setVisible(false); }
		//! \brief 
		virtual void showFitToPageButton(void) { m_FitToPageButton.setVisible(true); }
		//! \brief 
		virtual TMapWidget* mapWidget(void) const { return w_MapWidget; }

   private:
      Q_DISABLE_COPY(TRulerScaling)

      //! \var Кнопка увеличения
      TPushButton m_ZoomInButton;
      //! \var Кнопка уменьшения
      TPushButton m_ZoomOutButton;
      //! \var Кнопка увеличения
      TPushButton m_FitToPageButton;
      //! \var указатель на слайдер
      TRulerScalingSlider* w_Slider;
      //! \var лайоут, содержащий все виджеты
      QVBoxLayout m_Layout;
      //! \var курсор до попадания на виджет
      QCursor m_OldCursor;
		//! \var 
		TMapWidget* w_MapWidget;

   signals:
      void zoomInClicked(void);
      void zoomOutClicked(void);
      void fitToPageClicked(void);
      void zoomChanged(int);

   public slots:
      //! \brief Устанавливает текущее значение масштаба
      void slotSetValue(int Value_) { w_Slider->setValue(Value_); } 
      void slotZoomInClicked(void)  { w_Slider->setValue(w_Slider->value() + 1); }
      void slotZoomOutClicked(void) { w_Slider->setValue(w_Slider->value() - 1); }

   protected:
      virtual void enterEvent(QEvent* Event_);
      virtual void leaveEvent(QEvent* Event_);
   };
}

#endif // #ifndef QNETMAP_RULERSCALING_H
