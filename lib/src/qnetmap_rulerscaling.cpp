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

#include <QBitmap>
#include <QEvent>
#include <QPixmap>

#include "qnetmap_rulerscaling.h"
#include "qnetmap_widget.h"
#include "qnetmap_layer.h"
#include "qnetmap_translator.h"

// полученный с помощью программы tohex.exe код с png-изображением
#include "../images/qnetmap_fitto.hex"

namespace qnetmap
{
   ///////////////////////////////////////////////////////////////////////////
   TRulerScaling::TRulerScaling(QWidget *Parent_) : QWidget(Parent_), w_MapWidget(qobject_cast<TMapWidget*>(Parent_))
   {
   setGeometry(0, 0, 33, 200);
   setMinimumWidth(5);
   setMinimumHeight(200);
   setMaximumHeight(200);
   //
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   // устанавливаем полупрозрачный серый фон
   QPalette Palette(palette());
   Palette.setColor(QPalette::Window, QColor(160, 160, 160, 120));
   setPalette(Palette);
   setAutoFillBackground(true);
   // закругляем углы фона с помощью маски
   QBitmap Mask(width(), height());
   Mask.fill(Qt::color0);
   QPainter Painter(&Mask);
   int PenThickness = Painter.pen().width();
   PenThickness = PenThickness == 0 ? 1 : PenThickness;
   Painter.setBrush(Qt::color1);
   Painter.drawRoundedRect(0, 0, width() - PenThickness, height() - PenThickness, 4, 4);
   setMask(Mask);         
   // параметры кнопок
   m_ZoomInButton.setText("+");
	m_ZoomInButton.setToolTip(QNetMapTranslator::tr("Zoom in"));
   m_ZoomOutButton.setText("-");
	m_ZoomOutButton.setToolTip(QNetMapTranslator::tr("Zoom out"));
   m_ZoomInButton.setMaximumWidth(24);
   m_ZoomOutButton.setMaximumWidth(24);
   m_FitToPageButton.setMaximumWidth(24);
   QPixmap Pixmap;
   Pixmap.loadFromData(qnetmap_fitto, sizeof(qnetmap_fitto));
   m_FitToPageButton.setIcon(QIcon(Pixmap));
   m_FitToPageButton.setToolTip(QNetMapTranslator::tr("Fit to objects"));
   
   // создаем слайдер и устанавливаем его параметры
   w_Slider = new TRulerScalingSlider(Qt::Vertical, this);
   w_Slider->setTickInterval(1);
   w_Slider->setTickPosition(QSlider::TicksBothSides);
   w_Slider->setMinimum(0);
   w_Slider->setMaximum(10);
   w_Slider->setTracking(false);
   // изменяем отображение слайдера
   w_Slider->setStyleSheet("QSlider::groove:vertical {"
                           "border: 0px;"
                           "width: 16px;"
                           "background: ;"
                           "}"
                           "QSlider::handle:vertical {"
                           "border: 1px solid #666666;"
                           "height: 8px;"
                           "margin: 0 -2px;"
                           "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #d1d1d1, stop:1 #f4f4f4);"
                           "border-radius: 3px;"
                           "}");
   // лайоут
   m_Layout.setMargin(2);
   m_Layout.setSpacing(0);
   m_Layout.addWidget(&m_ZoomInButton);
   m_Layout.addWidget(w_Slider);
   m_Layout.addWidget(&m_ZoomOutButton);
   m_Layout.addWidget(&m_FitToPageButton);
   m_Layout.setAlignment(&m_ZoomInButton,  Qt::AlignHCenter);
   m_Layout.setAlignment(&m_ZoomOutButton, Qt::AlignHCenter);
   m_Layout.setAlignment(&m_FitToPageButton, Qt::AlignHCenter);
   m_Layout.setAlignment(w_Slider, Qt::AlignHCenter);
   //
   setLayout(&m_Layout);
   // соединяем сигналы со слотами
   QNM_DEBUG_CHECK(connect(&m_ZoomInButton,    SIGNAL(buttonPressed()),   SLOT(slotZoomInClicked())));
   QNM_DEBUG_CHECK(connect(&m_ZoomOutButton,   SIGNAL(buttonPressed()),   SLOT(slotZoomOutClicked())));
   QNM_DEBUG_CHECK(connect(&m_FitToPageButton, SIGNAL(buttonPressed()),   SIGNAL(fitToPageClicked())));
   QNM_DEBUG_CHECK(connect(w_Slider,           SIGNAL(valueChanged(int)), SIGNAL(zoomChanged(int))));
   }

   //---------------------------------------------------------------
   void TRulerScaling::setInterval(const int Minimum_, const int Maximum_) 
   {
   w_Slider->setMinimum(Minimum_);
   w_Slider->setMaximum(Maximum_);
   }

   //----------------------------------------------------------------------
   void TRulerScaling::enterEvent(QEvent* Event_)
   {
   m_OldCursor = cursor();
   setCursor(QCursor(Qt::ArrowCursor));
	w_MapWidget->hideCoordinates();
   Event_->accept();
   }

   //----------------------------------------------------------------------
   void TRulerScaling::leaveEvent(QEvent* Event_)
   {
   setCursor(m_OldCursor);
	w_MapWidget->showCoordinates();
   Event_->accept();
   }

   //---------------------------------------------------------------
   void TRulerScaling::TRulerScalingSlider::paintEvent(QPaintEvent* Event_)
   {
   // кнопки масштаба
   if(w_Parent) {
      TRulerScaling* Parent = qobject_cast<TRulerScaling*>(w_Parent);
      Parent->m_ZoomInButton.setEnabled(value() != maximum());
      Parent->m_ZoomOutButton.setEnabled(value() != minimum());
		if(Parent->mapWidget()->geometryLayer()->geometriesCount()) Parent->showFitToPageButton();
		else Parent->hideFitToPageButton();
      }
   // рисуем деления шкалы
   m_Painter.begin(this);
   m_Painter.setPen(QPen(Qt::white, 2));
   int DashXEnd = width() - 8;
   int StepsCount = maximum() - minimum() + 1;
   qreal DashStep = (qreal)(height() - 9) / (StepsCount - 1);
   for(int i = 0; i < StepsCount; ++i) {
      int Y = DashStep * i + 5;
      m_Painter.drawLine(7, Y, DashXEnd, Y);
      }
   m_Painter.end();
   //
   QSlider::paintEvent(Event_);
   }
}
