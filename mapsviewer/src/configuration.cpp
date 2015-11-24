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

#include "configuration.h"
#include "qnetmap_imagemanager.h"
#include "mapsviewer_translator.h"

using namespace qnetmap;

//==============================================================================================
TMapConfiguration::TMapConfiguration(QWidget *Parent_) : QWizard(Parent_), m_Parent(Parent_)
{
// модифицируем элементы визарда
setOptions(QWizard::NoBackButtonOnLastPage);
setButtonText(QWizard::FinishButton, MapsViewerTranslator::tr("Save"));
setButtonText(QWizard::CancelButton, MapsViewerTranslator::tr("Cancel"));

// создаем страницу с параметрами конфигурации
m_ConfigurationDialog = new TMapConfigurationPage(this);
addPage(m_ConfigurationDialog);
//
setWindowTitle(MapsViewerTranslator::tr("Settings"));
}

//----------------------------------------------------------------------------------------------
void TMapConfiguration::accept(void)
{
// сохраняем строки настройки в реестре
m_ConfigurationDialog->saveStorageType(m_ConfigurationDialog->mapsStorageType(), QNetMapConsts::StorageTypeKey);
// конфигурируем карты
TMap* Map = qobject_cast<TMap*>(m_Parent);
Map->setMapsConfiguration(m_ConfigurationDialog->m_StoragePath->currentText(),
                          m_ConfigurationDialog->mapsStorageType());
reject();
}

//==============================================================================================
TMapConfigurationPage::TMapConfigurationPage(QWidget *Parent_)
   : QWizardPage(Parent_), m_Parent(Parent_)
{
// кеш
m_CachePath = new TPathField(
   MapsViewerTranslator::tr("Queries cache directory"), Consts::Organization, 
   Consts::Application, QNetMapConsts::CachePathKey);
// хранилище
m_StoragePath = new TPathField(
   MapsViewerTranslator::tr("Maps data storage directory"), 
   Consts::Organization, Consts::Application, QNetMapConsts::StoragePathKey);
// тип
QLabel* TypeLabel = new QLabel(MapsViewerTranslator::tr("Maps source"));
TypeLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
//
QRadioButton *Radio1 = new QRadioButton(MapsViewerTranslator::tr("Internet only"));
QRadioButton *Radio2 = new QRadioButton(MapsViewerTranslator::tr("Internet and storage"));
QRadioButton *Radio3 = new QRadioButton(MapsViewerTranslator::tr("storage only"));
//
QHBoxLayout *TypeLayout = new QHBoxLayout;
TypeLayout->addWidget(Radio1);
TypeLayout->addWidget(Radio2);
TypeLayout->addWidget(Radio3);
TypeLayout->addStretch(1);
//
m_NewMapsStorageType = getSavedStorageType(QNetMapConsts::StorageTypeKey);
switch(m_NewMapsStorageType) {
case qnetmap::TImageManager::Consts::InternetOnly:
   Radio1->setChecked(true);
   break;
case qnetmap::TImageManager::Consts::InternetAndStorage:
   Radio2->setChecked(true);
   break;
case qnetmap::TImageManager::Consts::StorageOnly:
   Radio3->setChecked(true);
   break;
default:
   m_NewMapsStorageType = qnetmap::TImageManager::Consts::InternetOnly;
   Radio1->setChecked(true);
   break;
   }
w_MapsStorageTypeButtonGroup = new QButtonGroup(this);
w_MapsStorageTypeButtonGroup->addButton(Radio1, qnetmap::TImageManager::Consts::InternetOnly);
w_MapsStorageTypeButtonGroup->addButton(Radio2, qnetmap::TImageManager::Consts::InternetAndStorage);
w_MapsStorageTypeButtonGroup->addButton(Radio3, qnetmap::TImageManager::Consts::StorageOnly);
// Main Layout
m_MainLayout = new QVBoxLayout;
m_MainLayout->addWidget(m_CachePath);
m_MainLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
m_MainLayout->addWidget(m_StoragePath);
m_MainLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
m_MainLayout->addWidget(TypeLabel);
m_MainLayout->addLayout(TypeLayout);
//
setLayout(m_MainLayout);
// сигналы
QNM_DEBUG_CHECK(connect(w_MapsStorageTypeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setMapsStorageType(int))));
QNM_DEBUG_CHECK(connect(m_CachePath, SIGNAL(editTextChanged(const QString&)), this, SIGNAL(completeChanged())));
QNM_DEBUG_CHECK(connect(m_StoragePath, SIGNAL(editTextChanged(const QString&)), this, SIGNAL(completeChanged())));
}

//------------------------------------------------------------------
bool TMapConfigurationPage::isComplete() const
{
if(!m_CachePath->currentText().isEmpty()       &&  // путь кеша введен
   !m_StoragePath->currentText().isEmpty()     &&  // путь хранилища введен
   QDir(m_CachePath->currentText()).exists()   &&  // путь кеша существует
   QDir(m_StoragePath->currentText()).exists())    // путь хранилища существует
   {
   return true;
   }
else
   return false;
}

//------------------------------------------------------------------
void TMapConfigurationPage::saveStorageType(const int Type_, const QString &Key_)
{
QSettings Settings(Consts::Organization, Consts::Application);
Settings.setValue(Key_, Type_);
}

//------------------------------------------------------------------
int TMapConfigurationPage::getSavedStorageType(const QString &Key_)
{
QSettings Settings(Consts::Organization, Consts::Application);
if(Settings.contains(Key_)) {
   return Settings.value(Key_).toInt();
   }
return 0;
}

//------------------------------------------------------------------
void TMapConfigurationPage::setMapsStorageType(int Type_)
{
m_NewMapsStorageType = Type_;
}
