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

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QCheckBox>
   #include <QMessageBox>
#endif
#include <QTextStream>
#include <QDate>

#include "qnetmap_loaddialogwizard.h"

namespace qnetmap
{
   //==============================================================================================
   TLoadDialogWizard::TLoadDialogWizard(QPointF PointF1_, QPointF PointF2_,
                                        IMapAdapterInterfaces MapInterfaces_,
                                        QString Organization_, QString Application_,
                                        QWidget *Parent_) 
      : QWizard(Parent_), m_PointF1(PointF1_), m_PointF2(PointF2_), m_MapInterfaces(MapInterfaces_),
        m_Organization(Organization_), m_Application(Application_)
   {
   // модифицируем элементы визарда
   // Russian: Далее
   setButtonText(QWizard::NextButton,   QNetMapTranslator::tr("Next"));
   // Russian: Завершить
   setButtonText(QWizard::FinishButton, QNetMapTranslator::tr("Finish"));
   // Russian: Отмена
   setButtonText(QWizard::CancelButton, QNetMapTranslator::tr("Cancel"));
   // создаем страницы мастера
   m_LoadDialogWizardMapsPage       = new TLoadDialogWizardMapsPage(this);
   m_LoadDialogWizardParametersPage = new TLoadDialogWizardParametersPage(this);
   m_LoadDialogWizardPathPage       = new TLoadDialogWizardPathPage(this);
   //
   addPage(m_LoadDialogWizardMapsPage);
   addPage(m_LoadDialogWizardParametersPage);
   addPage(m_LoadDialogWizardPathPage);
   //
   // Russian: Создать задания для загрузки выбранной области карты
   setWindowTitle(QNetMapTranslator::tr("Create jobs to download the selected area"));
   }

   //----------------------------------------------------------------------------------------------
   void TLoadDialogWizard::accept(void)
   {
   QString FilePrefix = m_LoadDialogWizardPathPage->m_JobsPrefix->currentText();
   QString Directory  = m_LoadDialogWizardPathPage->m_SavePath->currentText();

   // делим область на участки в соответствии с количеством задач
   QList<QPair<QPointF, QPointF> > Areas;
   if(m_LoadDialogWizardParametersPage->m_CountJobs->value() == 1) 
      Areas.append(qMakePair(m_PointF1, m_PointF2));
   else {
      // делим на вертикальные полосы
      qreal Y1 = m_PointF1.y();
      qreal Y2 = m_PointF2.y();
      qreal X  = m_PointF1.x();
      qreal DeltaX = 
         (m_PointF2.x() - m_PointF1.x()) / m_LoadDialogWizardParametersPage->m_CountJobs->value();
      for(int i = 1; i < m_LoadDialogWizardParametersPage->m_CountJobs->value(); i++) {
         Areas.append(qMakePair(QPointF(X, Y1), QPointF(X + DeltaX, Y2)));
         X += DeltaX;
         }
      Areas.append(qMakePair(QPointF(X, Y1), QPointF(m_PointF2.x(), Y2)));
      }
   // проходим по списку с формированием отдельных файлов заданий
   int i = 0;
   QPair<QPointF, QPointF> PointsPair;
   foreach(PointsPair, Areas) {
      QString FileNameTemplate = Directory + "/" + FilePrefix + "%1.xml";
      QFile File(FileNameTemplate.arg(i++));
      if(!File.exists()) {
         File.open(QIODevice::WriteOnly | QIODevice::Text);
         QTextStream out(&File);
         // начало документа
         out << QString("<?xml version=\"1.0\"?>\n<%1 %2=\"%3\">\n")
                   .arg(SPIDER_TAG_JOBDOCUMENT)
                   .arg(SPIDER_PARAMETER_REQUESTSCOUNT)
                   .arg(m_LoadDialogWizardParametersPage->m_SimultaneousRequestsCount->value());
         // проходим по картам
         for(QMap<QString, TMapDetails>::const_iterator it = m_LoadDialogWizardMapsPage->m_Maps.begin();
             it != m_LoadDialogWizardMapsPage->m_Maps.end(); ++it) {
            if(it.value().g_CheckBox->isChecked()) {
               IMapAdapterInterface* MapInterface = it.value().g_MapInterface;
               int FromZoom = it.value().g_FromZoom->value();
               int ToZoom   = it.value().g_ToZoom->value();
               // начинаем карту
               out << QString("\t<%1 %2=\"%3\">\n")
                         .arg(SPIDER_TAG_MAP)
                         .arg(SPIDER_PARAMETER_PLUGINNAME)
                         .arg(MapInterface->pluginName());
               // проходим по областям с масштабами
               for(int i = FromZoom; i <= ToZoom; i++) {
                  if(i >= MapInterface->minZoom() && i <= MapInterface->maxZoom()) {
                     // начинаем область
                     out << QString("\t\t<%1 %2=\"%3\">\n")
                               .arg(SPIDER_TAG_AREA)
                               .arg(SPIDER_PARAMETER_ZOOM)
                               .arg(i);
                     // пишем точки углов выделенной области
                     out << QString("\t\t\t<%1 %2=\"%3\" %4=\"%5\"/>\n")
                               .arg(SPIDER_TAG_POINT)
                               .arg(SPIDER_PARAMETER_LONGITUDE)
                               .arg(PointsPair.first.x())
                               .arg(SPIDER_PARAMETER_LATITUDE)
                               .arg(PointsPair.first.y());
                     out << QString("\t\t\t<%1 %2=\"%3\" %4=\"%5\"/>\n")
                               .arg(SPIDER_TAG_POINT)
                               .arg(SPIDER_PARAMETER_LONGITUDE)
                               .arg(PointsPair.second.x())
                               .arg(SPIDER_PARAMETER_LATITUDE)
                               .arg(PointsPair.second.y());
                     // завершаем область
                     out << QString("\t\t</%1>\n").arg(SPIDER_TAG_AREA);
                     }
                  }
               // завершаем карту
               out << QString("\t</%1>\n").arg(SPIDER_TAG_MAP);
               }
            }
         // завершаем документ
         out << QString("</%1>\n").arg(SPIDER_TAG_JOBDOCUMENT);
         File.close();
         reject();
         }
      else {
         QMessageBox::critical(0, QNetMapTranslator::tr("Error"), 
            // Russian: Файл '%1' уже существует.
            QNetMapTranslator::tr("File '%1' already exists.").arg(File.fileName()));
         }
      }
      // 
      QMessageBox::information(0, QNetMapTranslator::tr("Success"),
         // Russian: Задания для загрузки выбранной области карты были успешно созданы.
         QNetMapTranslator::tr("The jobs for downloading the map's area were successfully created."));
   }
   
   //==============================================================================================
   TLoadDialogWizardMapsPage::TLoadDialogWizardMapsPage(QWidget *Parent_) : QWizardPage(Parent_)
   {
   setTitle(QNetMapTranslator::tr("Maps"));
   //
   m_MapsVerticalLayout = new QVBoxLayout;
   m_MapsVerticalLayout->setSpacing(5);
   m_MapsVerticalLayout->setContentsMargins(10, 5, 20, 5);
   //
   m_Label = new QLabel(
      // Russian: Выберите карты и диапазон масштабов (от и до) для загрузки.
      QNetMapTranslator::tr("Select the maps and the scales range (from/to) to download.\n"));
   m_Label->setWordWrap(true);
   m_MapsVerticalLayout->addWidget(m_Label);
   // если карт много, необходимо разделить их на несколько колонок
   QHBoxLayout* InterfacesLayout = new QHBoxLayout;
   // количество отображаемых интерфейсов (только статические интернет-карты)
   uint InterfacesCount = 0;
   IMapAdapterInterface* MapInterface;
   foreach(MapInterface, qobject_cast<TLoadDialogWizard*>(Parent_)->m_MapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap &&
         MapInterface->updatability() == TMapAdapter::Consts::QueryUpdatabilityStatic) {
         //
         ++InterfacesCount;
         }
      }
   // количество колонок и строк
   uint ColumnsCount = InterfacesCount / 20;
   if((InterfacesCount % 20) > 0) ColumnsCount++;
   uint RowsCount = InterfacesCount / ColumnsCount;
   if((InterfacesCount % ColumnsCount) > 0) RowsCount++;
   // лайоуты колонок
   QList<QVBoxLayout*> ColumnLayouts;
   for(uint i = 1; i <= ColumnsCount; ++i) {
      QVBoxLayout* Column = new QVBoxLayout;
      Column->setContentsMargins(0, 0, i == ColumnsCount ? 0 : 30, 0);
      ColumnLayouts.push_back(Column);
      InterfacesLayout->addLayout(Column);
      }
   m_MapsVerticalLayout->addLayout(InterfacesLayout);
   // начальные значения переменных и счетчиков
   QList<QVBoxLayout*>::const_iterator it = ColumnLayouts.begin();
   QVBoxLayout* ColumnLayout = *it;
   uint Row = 0;
   // создаем необходимые виджеты для всех карт
   foreach(MapInterface, qobject_cast<TLoadDialogWizard*>(Parent_)->m_MapInterfaces) {
      if(MapInterface->sourceType() == TMapAdapter::Consts::InternetMap &&
         MapInterface->updatability() == TMapAdapter::Consts::QueryUpdatabilityStatic) {
         //
         if(++Row > RowsCount) { Row = 1; ++it; ColumnLayout = *it; }
         //
         QString MapName = MapInterface->pluginName();
         // чекбокс
         QCheckBox* CheckBox = new QCheckBox;
         CheckBox->setText(MapName);
         CheckBox->setFixedWidth(200);
         // от
         QLabel* FromLabel = new QLabel(QNetMapTranslator::tr("from"));
         FromLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
         FromLabel->setDisabled(true);
         QSpinBox* FromZoom = new QSpinBox;
         FromZoom->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
         FromZoom->setMinimum(MapInterface->minZoom());
         FromZoom->setMaximum(MapInterface->maxZoom());
         FromZoom->setValue(MapInterface->minZoom());
         FromZoom->setDisabled(true);
         // до
         QLabel* ToLabel = new QLabel(QNetMapTranslator::tr("to"));
         ToLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
         ToLabel->setDisabled(true);
         QSpinBox* ToZoom = new QSpinBox;
         ToZoom->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
         ToZoom->setMinimum(MapInterface->minZoom());
         ToZoom->setMaximum(MapInterface->maxZoom());
         ToZoom->setValue(MapInterface->maxZoom());
         ToZoom->setDisabled(true);
         //
         QSpacerItem* RightSpacer = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
         // лайоут масштабов одной карты
         QHBoxLayout *ZoomLayout = new QHBoxLayout;
         ZoomLayout->addWidget(FromLabel);
         ZoomLayout->addWidget(FromZoom);
         ZoomLayout->addWidget(ToLabel);
         ZoomLayout->addWidget(ToZoom);
         // добавляем в список
         m_Maps[MapName] = TMapDetails(MapInterface, CheckBox, FromZoom, ToZoom, ZoomLayout);
         // лайоут строки
         QHBoxLayout *Layout = new QHBoxLayout;
         Layout->addWidget(CheckBox);
         Layout->addLayout(ZoomLayout);
         Layout->addItem(RightSpacer);
         ColumnLayout->addLayout(Layout);
         // реакция на сигналы
         QNM_DEBUG_CHECK(connect(CheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(completeChanged())));
         QNM_DEBUG_CHECK(connect(CheckBox, SIGNAL(stateChanged(int)), &(m_Maps[MapName]), SLOT(checkBoxeStateChanged(int))));
         QNM_DEBUG_CHECK(connect(FromZoom, SIGNAL(valueChanged(int)), &(m_Maps[MapName]), SLOT(setValueToZoom(int))));
         QNM_DEBUG_CHECK(connect(ToZoom,   SIGNAL(valueChanged(int)), &(m_Maps[MapName]), SLOT(setValueFromZoom(int))));
         }
      }
   //
   setLayout(m_MapsVerticalLayout);
   }

   //------------------------------------------------------------------
   bool TLoadDialogWizardMapsPage::isComplete() const
   {
   // хотя-бы один чекбокс должен быть отмечен
   for(QMap<QString, TMapDetails>::const_iterator it = m_Maps.begin(); it != m_Maps.end(); ++it) {
      if(it.value().g_CheckBox->isChecked()) return true;
      }
   return false;
   }

   //==============================================================================================
   TLoadDialogWizardParametersPage::TLoadDialogWizardParametersPage(QWidget *Parent_) 
      : QWizardPage(Parent_)
   {
   // Russian: Параметры заданий
   setTitle(QNetMapTranslator::tr("Jobs settings"));
   //
	// Russian: Задайте количество заданий и количество потоков загрузки для каждого задания.
	// При слишком большом числе потоков загрузки есть риск блокировки со стороны поставщика карт.

   m_Label = new QLabel(QNetMapTranslator::tr(
		"Specify the number of the jobs and the number of the downloading threads per job. "
		"If the number of the threads will be too big you can be blocked by the map's service "
		"provider."));

   m_Label->setWordWrap(true);
   // Count Jobs Layout
   // Russian: Количество заданий
   m_CountJobsLabel = new QLabel(QNetMapTranslator::tr("Number of the jobs"));
   m_CountJobsLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   m_CountJobs = new QSpinBox;
   m_CountJobs->setMinimum(1);
   m_CountJobs->setMaximum(100);
   m_CountJobs->setValue(1);
   m_CountJobs->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   m_JobsRightSpacer = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
   //
   m_CountJobsLayout = new QHBoxLayout;
   m_CountJobsLayout->addWidget(m_CountJobsLabel);
   m_CountJobsLayout->addWidget(m_CountJobs);
   m_CountJobsLayout->addItem(m_JobsRightSpacer);
   // Simultaneous Requests Count Layout
   m_SimultaneousRequestsCountLabel = new QLabel(
		// Russian: Количество потоков загрузки для каждого задания
		QNetMapTranslator::tr("Number of downloading threads per the each job"));

   m_SimultaneousRequestsCountLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   m_SimultaneousRequestsCount = new QSpinBox;
   m_SimultaneousRequestsCount->setMinimum(10);
   m_SimultaneousRequestsCount->setMaximum(1000);
   m_SimultaneousRequestsCount->setValue(20);
   m_SimultaneousRequestsCount->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
   m_SimultaneousRequestsCountRightSpacer = 
      new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
   //
   m_SimultaneousRequestsCountLayout = new QHBoxLayout;
   m_SimultaneousRequestsCountLayout->addWidget(m_SimultaneousRequestsCountLabel);
   m_SimultaneousRequestsCountLayout->addWidget(m_SimultaneousRequestsCount);
   m_SimultaneousRequestsCountLayout->addItem(m_SimultaneousRequestsCountRightSpacer);
   //
   m_ParametersBottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
   // Parameters Layout
   m_ParametersLayout = new QVBoxLayout;
   m_ParametersLayout->setSpacing(20);
   m_ParametersLayout->addWidget(m_Label);
   m_ParametersLayout->addLayout(m_CountJobsLayout);
   m_ParametersLayout->addLayout(m_SimultaneousRequestsCountLayout);
   m_ParametersLayout->addItem(m_ParametersBottomSpacer);
   //
   setLayout(m_ParametersLayout);
   }

   //==============================================================================================
   TLoadDialogWizardPathPage::TLoadDialogWizardPathPage(QWidget *Parent_)
      : QWizardPage(Parent_), m_Parent(Parent_)
   {
   TLoadDialogWizard* Main = qobject_cast<TLoadDialogWizard*>(m_Parent);
   m_Organization = Main->m_Organization;
   m_Application  = Main->m_Application;
   //
   // Russian: Файлы заданий
   setTitle(QNetMapTranslator::tr("Job files"));
   //
   m_Label = new QLabel(
		// Russian: Задайте префикс имени файла задания и папку, в которой будут сохранены 
		// задания. Имя файла задания будет сформировано из префикса и порядкового номера задания.
		// Кнопка 'Завершить' будет доступна, если заданная папка существует и в ней нет ни одного
		// файла с заданным префиксом.
		QNetMapTranslator::tr("Specify the file name prefix and the folder for saving the jobs. "
			"The job filename is formed of a prefix and the job's serial number. "
			"The 'Finish' button will become available if the specified folder exists and "
			"contains no file with the prefix specified."));
   m_Label->setWordWrap(true);
   // Jobs Prefix Layout
   // Russian: Префикс имени файла задания
   m_JobsPrefix = new TTextField(QNetMapTranslator::tr("Job filename prefix"),
                                 Main->m_Organization, Main->m_Application, "JobXMLFilePrefix");
   m_JobsPrefix->setText(
      QString("MapsLoadJob_%1_").arg(QDate::currentDate().toString(Qt::ISODate)));
   // путь сохранения
   // Russian: Каталог для файлов заданий
   m_SavePath = new TPathField(QNetMapTranslator::tr("Folder for the job files"),
                               Main->m_Organization, Main->m_Application, "JobXMLSavePath");
   //
   // m_Spacer = new QSpacerItem(0, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
   // Path Layout
   m_PathLayout = new QVBoxLayout(this);
   m_PathLayout->addWidget(m_Label);
   m_PathLayout->addSpacing(m_PathLayout->spacing() * 2);
   m_PathLayout->addWidget(m_JobsPrefix);
   m_PathLayout->addSpacing(m_PathLayout->spacing() * 2);
   m_PathLayout->addWidget(m_SavePath);
   //

   // сигналы
   QNM_DEBUG_CHECK(connect(m_JobsPrefix, SIGNAL(editTextChanged(const QString&)), this, SIGNAL(completeChanged())));
   QNM_DEBUG_CHECK(connect(m_SavePath,   SIGNAL(editTextChanged(const QString&)), this, SIGNAL(completeChanged())));
   }

   //------------------------------------------------------------------
   TLoadDialogWizardPathPage::~TLoadDialogWizardPathPage()
   {
   }

   //------------------------------------------------------------------
   bool TLoadDialogWizardPathPage::isComplete() const
   {
   if(!m_JobsPrefix->currentText().isEmpty() &&    // префикс введен
      !m_SavePath->currentText().isEmpty()   &&    // путь введен
      QDir(m_SavePath->currentText()).exists()) {  // путь существует
      //
      int CountJobs = 
         qobject_cast<TLoadDialogWizard*>(m_Parent)->m_LoadDialogWizardParametersPage->m_CountJobs->value();
      // проверяем на существование файлов (их не должно быть)
      for(int i = 0; i < CountJobs; ++i) {
         QString FileNameTemplate =
            m_SavePath->currentText() + "/" + m_JobsPrefix->currentText() + "%1.xml";
         if(QFile(FileNameTemplate.arg(i++)).exists()) return false;
         }
      return true;
      }
   else
      return false;
   }
}
