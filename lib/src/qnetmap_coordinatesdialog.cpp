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

#include "qnetmap_coordinatesdialog.h"

namespace qnetmap
{
   TCoordinatesDialog::TCoordinatesDialog(QPointF* Point_, QWidget* Parent_)
      : QDialog(Parent_), w_Point(Point_)
   {
   setWindowTitle(QNetMapTranslator::tr("Add reference point" 
		/* Russian: Добавить точку привязки */));
   setMinimumSize(QSize(250, 100));
   QGridLayout* GridLayout = new QGridLayout;
   QLabel* Hint = new QLabel(QNetMapTranslator::tr("Set selected point's coordinates"
		/* Russian: Задайте координаты выбранной точки */));
   GridLayout->addWidget(Hint, 0, 0, 1, 5, Qt::AlignLeft);
   // поля ввода
   /// широта
   QLabel* Latitude = new QLabel(QNetMapTranslator::tr("Latitude" /* Russian: Широта */));
   Latitude->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   GridLayout->addWidget(Latitude, 1, 0, Qt::AlignRight);
   w_LatDegrees = new QSpinBox;
   w_LatDegrees->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LatDegrees->setRange(0, 90);
   w_LatDegrees->setValue(0);
   w_LatDegrees->setSuffix(QChar(0xB0)); // gradus
   GridLayout->addWidget(w_LatDegrees, 1, 1, Qt::AlignCenter);
   w_LatMinutes = new QSpinBox;
   w_LatMinutes->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LatMinutes->setRange(0, 59);
   w_LatMinutes->setValue(0);
   w_LatMinutes->setSuffix("\"");
   GridLayout->addWidget(w_LatMinutes, 1, 2, Qt::AlignCenter);
   w_LatSeconds = new QDoubleSpinBox;
   w_LatSeconds->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LatSeconds->setRange(0, 59.9999);
   w_LatSeconds->setValue(0);
   w_LatSeconds->setDecimals(4);
   w_LatSeconds->setSuffix("'");
   GridLayout->addWidget(w_LatSeconds, 1, 3, Qt::AlignCenter);
   w_NorthSouth = new QComboBox;
   w_NorthSouth->addItem(QNetMapTranslator::tr("N" /* Russian: с.ш. */));
   w_NorthSouth->addItem(QNetMapTranslator::tr("S" /* Russain: ю.ш. */));
   w_NorthSouth->setEditable(false);
   GridLayout->addWidget(w_NorthSouth, 1, 4, Qt::AlignLeft);
   /// долгота
   QLabel* Longitude = new QLabel(QNetMapTranslator::tr("Longitude" /* Russian: Долгота */));
   Longitude->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   GridLayout->addWidget(Longitude, 2, 0, Qt::AlignRight);
   w_LonDegrees = new QSpinBox;
   w_LonDegrees->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LonDegrees->setRange(0, 180);
   w_LonDegrees->setValue(0);
   w_LonDegrees->setSuffix(QChar(0xB0)); // gradus
   GridLayout->addWidget(w_LonDegrees, 2, 1, Qt::AlignCenter);
   w_LonMinutes = new QSpinBox;
   w_LonMinutes->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LonMinutes->setRange(0, 59);
   w_LonMinutes->setValue(0);
   w_LonMinutes->setSuffix("\"");
   GridLayout->addWidget(w_LonMinutes, 2, 2, Qt::AlignCenter);
   w_LonSeconds = new QDoubleSpinBox;
   w_LonSeconds->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   w_LonSeconds->setRange(0, 59.9999);
   w_LonSeconds->setValue(0);
   w_LonSeconds->setDecimals(4);
   w_LonSeconds->setSuffix("'");
   GridLayout->addWidget(w_LonSeconds, 2, 3, Qt::AlignCenter);
   w_EastWest = new QComboBox;
   w_EastWest->addItem(QNetMapTranslator::tr("E" /* Russian: в.д. */));
   w_EastWest->addItem(QNetMapTranslator::tr("W" /* Russain: з.д.*/));
   w_EastWest->setEditable(false);
   GridLayout->addWidget(w_EastWest, 2, 4, Qt::AlignLeft);
   // Кнопки подтверждения / отмены
   QDialogButtonBox* ButtonBox = new QDialogButtonBox;
   ButtonBox->setMinimumSize(QSize(0, 30));
   ButtonBox->setOrientation(Qt::Horizontal);
   ButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
   /// находим кнопку ОК
   QList<QAbstractButton*> ListButtons = ButtonBox->buttons();
   foreach(QAbstractButton* Button, ListButtons) {
      if(ButtonBox->buttonRole(Button) == QDialogButtonBox::AcceptRole) {
         w_ButtonOK = Button;
         break;
         }
      }
   GridLayout->addWidget(ButtonBox, 3, 0, 1, 5, Qt::AlignRight);
   //
   GridLayout->setColumnStretch(5, 1);
   GridLayout->setRowStretch(6, 1);
   GridLayout->setVerticalSpacing(18);
   setLayout(GridLayout);
   // сигналы
   QNM_DEBUG_CHECK(connect(ButtonBox, SIGNAL(accepted()), this, SLOT(ok())));
   QNM_DEBUG_CHECK(connect(ButtonBox, SIGNAL(rejected()), this, SLOT(reject())));
   //
   QNM_DEBUG_CHECK(connect(w_LatDegrees, SIGNAL(valueChanged(int)),    this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_LonDegrees, SIGNAL(valueChanged(int)),    this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_LatMinutes, SIGNAL(valueChanged(int)),    this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_LonMinutes, SIGNAL(valueChanged(int)),    this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_LatSeconds, SIGNAL(valueChanged(double)), this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_LonSeconds, SIGNAL(valueChanged(double)), this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_NorthSouth, SIGNAL(currentIndexChanged(int)), this, SLOT(buttonOK())));
   QNM_DEBUG_CHECK(connect(w_EastWest,   SIGNAL(currentIndexChanged(int)), this, SLOT(buttonOK())));
   }

   // -------------------------------------------------------------------------------
   qreal TCoordinatesDialog::latitude(void)
   {
   qreal Latitude  = w_LatDegrees->value() + 
                     w_LatMinutes->value() / 60. +
                     w_LatSeconds->value() / 3600.;
   if(w_NorthSouth->currentText() == QNetMapTranslator::tr("S" /* Russian: ю.ш. */)) 
		Latitude = -Latitude;
   return Latitude;
   }

   // -------------------------------------------------------------------------------
   qreal TCoordinatesDialog::longitude(void)
   {
   qreal Longitude = w_LonDegrees->value() + 
                     w_LonMinutes->value() / 60. +
                     w_LonSeconds->value() / 3600.;
   if(w_EastWest->currentText() == QNetMapTranslator::tr("W" /* Russian: з.д. */)) 
		Longitude = -Longitude;
   return Longitude;
   }

   // -------------------------------------------------------------------------------
   void TCoordinatesDialog::ok(void)
   {
   w_Point->setX(longitude());
   w_Point->setY(latitude());
   //
   accept();
   }

   // -------------------------------------------------------------------------------
   void TCoordinatesDialog::buttonOK(void)
   {
   qreal Latitude  = latitude();
   qreal Longitude = longitude();
   w_ButtonOK->setEnabled((Latitude  >= -90  && Latitude  <= 90) &&
                          (Longitude >= -180 && Longitude <= 180));
   }
}
