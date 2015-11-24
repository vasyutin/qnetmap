/*
* This file is part of MapsViewer,
* an open-source cross-platform demonstration program for viewing maps, 
* based on the library QNetMap
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
* along with MapsViewer. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAP_CONFIGURATION_H
#define MAP_CONFIGURATION_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QWizard>
   #include <QWizardPage>
   #include <QHBoxLayout>
   #include <QVBoxLayout>
   #include <QLabel>
   #include <QComboBox>
   #include <QFileDialog>
   #include <QPushButton>
   #include <QRadioButton>
#endif
#include <QSettings>
#include <QDir>

#include "map.h"
#include "qnetmap_visual_elments.h"

using namespace qnetmap;

class TMapConfigurationPage;

//==============================================================================================
//! \class TMapConfiguration
//! \brief Реализует мастер формирования заданий на загрузку карт
class TMapConfiguration : public QWizard
{
   Q_OBJECT

public:
   //! \brief Конструктор класса
   //! \param QWidget *Parent_ - указатель на класс-родитель
   TMapConfiguration(QWidget *Parent_ = 0);

   //! \brief Сохраняет настройки в реестре и интерфейсах карт
   void accept(void);

   //! \var Указатель на класс-родитель
   QWidget* m_Parent;

private:
   Q_DISABLE_COPY(TMapConfiguration)

   //! \var Указатель на страницу
   TMapConfigurationPage* m_ConfigurationDialog;
};

//==============================================================================================
//! \class TMapConfigurationPage
//! \brief Реализует страницу путей мастера формирования заданий
class TMapConfigurationPage : public QWizardPage
{
   Q_OBJECT

public:
   friend class TMapConfiguration;

   TMapConfigurationPage(QWidget *Parent_ = 0);
   virtual ~TMapConfigurationPage()
   {
   delete w_MapsStorageTypeButtonGroup;
   }

   //! \brief Проверка правильности ввода на странице
   //! \return bool - true, если страница заполнена правильно
   bool isComplete() const;
   //! \brief Возвращает введенный тип хранилища
   //! \return int - тип хранилища 
   int mapsStorageType(void) const { return m_NewMapsStorageType; }

private:
   Q_DISABLE_COPY(TMapConfigurationPage)

   //! \brief Создает комбобокс и наполняет его сохраненными в реестре данными
   //! \param QString &Name_ - префикс ключа реестра
   //! \param int Count_ - максимальное число считываемых значений
   //! \return QComboBox* - указатель на созданный комбобокс
   QComboBox* createComboBox(const QString &Name_, const int Count_);
   //! \brief Сохраняет данные комбо-бокса в реестре
   //! \param QComboBox* ComboBox_ - указатель на комбобокс
   //! \param QString &Name_ - префикс ключа реестра
   //! \param int Count_ - максимальное число записываемых значений
   void saveComboBox(QComboBox* ComboBox_, const QString &Name_, const int Count_);
   //! \brief Сохраняет тип хранилища в реестре
   //! \param int Type_ - тип хранилища
   //! \param QString &Key_ - ключ реестра
   void saveStorageType(const int Type_, const QString &Key__);
   //! \brief Возвращает тип хранилища из реестра по ключу
   //! \param QString &Key_ - ключ реестра
   //! \return int - тип хранилища
   int getSavedStorageType(const QString &Key_);

   //! \var Указатель на общий лайоут
   QVBoxLayout *m_MainLayout;
   //! \var Указатель класс ввода пути кеша
   TPathField*  m_CachePath;
   //! \var Указатель класс ввода пути хранилища
   TPathField*  m_StoragePath;
   //! \var Указатель на родительский виджет
   QWidget *m_Parent;
   //! \var Указатель на группу радиокнопок
   QButtonGroup *w_MapsStorageTypeButtonGroup;
   //! \var Введенный тип хранилища
   int m_NewMapsStorageType;

public slots:
   //! \brief Запоминает тип хранилища
   //! \param int Type_ - тип хранилища
   void setMapsStorageType(int Type_);
};
#endif // MAP_CONFIGURATION_H
