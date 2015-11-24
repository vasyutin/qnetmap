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

#ifndef QNETMAP_LOADDIALOG_WIZARD_H
#define QNETMAP_LOADDIALOG_WIZARD_H


#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QWizard>
   #include <QWizardPage>
   #include <QSpinBox>
#endif

#include "qnetmap_xmltagnames.h"
#include "qnetmap_interfaces.h"
#include "qnetmap_visual_elments.h"

class QCheckBox;
class QHBoxLayout;
class QLabel;

namespace qnetmap
{
   class TLoadDialogWizardMapsPage;
   class TLoadDialogWizardParametersPage;
   class TLoadDialogWizardPathPage;

   //==============================================================================================
   //! \class TMapDetails
   //! \brief Вспомогательный класс, содержащий необходимые данные о картах
   class TMapDetails : public TBase 
   {
      Q_OBJECT

   public:
      TMapDetails() {};
      TMapDetails(IMapAdapterInterface* MapInterface_, QCheckBox* CheckBox_,
                  QSpinBox*  FromZoom_, QSpinBox*  ToZoom_, QHBoxLayout* ZoomLayout_) :
         g_MapInterface(MapInterface_),
         g_CheckBox(CheckBox_),
         g_FromZoom(FromZoom_),
         g_ToZoom(ToZoom_),
         g_ZoomLayout(ZoomLayout_) {}
      TMapDetails& operator=(const TMapDetails& Other_)
      {
      if(this != &Other_) { // защита от неправильного самоприсваивания
         g_MapInterface = Other_.g_MapInterface;
         g_CheckBox     = Other_.g_CheckBox;
         g_FromZoom     = Other_.g_FromZoom;
         g_ToZoom       = Other_.g_ToZoom;
         g_ZoomLayout   = Other_.g_ZoomLayout;
         }
      return *this;
      }
      explicit TMapDetails(const TMapDetails& Copy_)
         : TBase(),
           g_MapInterface(Copy_.g_MapInterface),
           g_CheckBox(Copy_.g_CheckBox),
           g_FromZoom(Copy_.g_FromZoom),
           g_ToZoom(Copy_.g_ToZoom),
           g_ZoomLayout(Copy_.g_ZoomLayout) {}

      //! \var Указатель на интерфейс карты
      IMapAdapterInterface* g_MapInterface;
      //! \var Указатель на чекбокс, соответствующий карте
      QCheckBox* g_CheckBox;
      //! \var Указатель на спинбокс со значением начального масштаба
      QSpinBox*  g_FromZoom;
      //! \var Указатель на спинбокс со значением конечного масштаба
      QSpinBox*  g_ToZoom;
      //! \var Указатель на лайоут, объединяющий виджеты, с изменяемой доступностью
      QHBoxLayout* g_ZoomLayout;

   public slots:
      //--------------------------------------------------
      //! \brief Переключает доступность виджетов в зависимости от состояния чекбокса
      void checkBoxeStateChanged(int State_)
      {
      // определяем доступность виджетов в соответствии с состоянием чек-бокса
      bool Disabled = State_ == Qt::Checked ? false : true;
      // идем по всем виджетам лайоута и изменяем их доступность
      for(int i=0; i < g_ZoomLayout->count(); ++i) {
         QWidget* Widget = g_ZoomLayout->itemAt(i)->widget();
         if(Widget) Widget->setDisabled(Disabled);
         }
      }
      //--------------------------------------------------
      //! \brief Устанавливает значение начального масштаба не более указанного
      void setValueFromZoom(int Value_)
      {
      if(g_FromZoom->value() > Value_) g_FromZoom->setValue(Value_);
      }
      //--------------------------------------------------
      //! \brief Устанавливает значение конечного масштаба не менее указанного
      void setValueToZoom(int Value_)
      {
      if(g_ToZoom->value() < Value_) g_ToZoom->setValue(Value_);
      }
   };

   //==============================================================================================
   //! \class TLoadDialogWizard
   //! \brief Реализует мастер формирования заданий на загрузку карт
   class QNETMAP_EXPORT TLoadDialogWizard : public QWizard
   {
      Q_OBJECT

   public:
      friend class TLoadDialogWizardPathPage;
      friend class TLoadDialogWizardMapsPage;

      //! \brief Конструктор класса
      //! \param QPointF PointF1_ - первая угловая точка загружаемой области
      //! \param QPointF PointF2_ - вторая угловая точка загружаемой области
      //! \param IMapAdapterInterfaces MapInterfaces_ - список указателей на интерфейсы карт
      //! \param QWidget *Parent_ - указатель на класс-родитель
      TLoadDialogWizard(QPointF PointF1_, QPointF PointF2_,
                        IMapAdapterInterfaces MapInterfaces_,
                        QString Organization_, QString Application_,
                        QWidget *Parent_ = 0);

      //! \brief Формирует файлы заданий по результатам работы мастера
      void accept(void);

   private:
      Q_DISABLE_COPY(TLoadDialogWizard)

      // координаты точек загружаемой области
      //! \var Координаты первой угловой точки загружаемой области
      QPointF m_PointF1;
      //! \var Координаты второй угловой точки загружаемой области
      QPointF m_PointF2;
      //
      //! \var Список указателей на интерфейсы карт
      IMapAdapterInterfaces     m_MapInterfaces;
      //! \var ключ организации для реестра
      QString m_Organization;
      //! \var ключ приложения для реестра
      QString m_Application;
      //! \var Указатель на страницу карт
      TLoadDialogWizardMapsPage*       m_LoadDialogWizardMapsPage;
      //! \var Указатель на страницу параметров
      TLoadDialogWizardParametersPage* m_LoadDialogWizardParametersPage;
      //! \var Указатель на страницу путей
      TLoadDialogWizardPathPage*       m_LoadDialogWizardPathPage;
   };

   //==============================================================================================
   //! \class TLoadDialogWizardMapsPage
   //! \brief Реализует страницу карт мастера формирования заданий
   class TLoadDialogWizardMapsPage : public QWizardPage
   {
      Q_OBJECT

   public:
      friend class TLoadDialogWizard;

      explicit TLoadDialogWizardMapsPage(QWidget *Parent_ = 0);

      //! \brief Проверка правильности ввода на странице
      //! \return bool - true, если страница заполнена правильно
      bool isComplete() const;

   private:
      Q_DISABLE_COPY(TLoadDialogWizardMapsPage)

      //! \var Описание содержания страницы и действий на ней
      QLabel* m_Label;
      //! \var Указатель на объединяющий лайоут
      QVBoxLayout* m_MapsVerticalLayout;
      //! \var список интерфейсов плагинов карт
      QMap<QString, TMapDetails> m_Maps;
   };

   //==============================================================================================
   //! \class TLoadDialogWizardParametersPage
   //! \brief Реализует страницу параметров мастера формирования заданий
   class TLoadDialogWizardParametersPage : public QWizardPage
   {
      Q_OBJECT

   public:
      friend class TLoadDialogWizard;
      friend class TLoadDialogWizardPathPage;

      explicit TLoadDialogWizardParametersPage(QWidget *Parent_ = 0);

   private:
      Q_DISABLE_COPY(TLoadDialogWizardParametersPage)

      //! \var Описание содержания страницы и действий на ней
      QLabel* m_Label;
      //! \var Указатель на объединяющий лайоут
      QVBoxLayout *m_ParametersLayout;
      //! \var Указатель на лайоут заданий
      QHBoxLayout *m_CountJobsLayout;
      //! \var Указатель на лайоут запросов
      QHBoxLayout *m_SimultaneousRequestsCountLayout;
      //! \var Указатель на метку заданий
      QLabel *m_CountJobsLabel;
      //! \var Указатель на метку запросов
      QLabel *m_SimultaneousRequestsCountLabel;
      //! \var Указатель на спинбокс заданий
      QSpinBox *m_CountJobs;
      //! \var Указатель на спинбокс запросов
      QSpinBox *m_SimultaneousRequestsCount;
      //! \var Указатель на спайсер общий
      QSpacerItem *m_ParametersBottomSpacer;
      //! \var Указатель на спайсер заданий
      QSpacerItem *m_JobsRightSpacer;
      //! \var Указатель на спайсер запросов
      QSpacerItem *m_SimultaneousRequestsCountRightSpacer;
   };

   //==============================================================================================
   //! \class TLoadDialogWizardPathPage
   //! \brief Реализует страницу путей мастера формирования заданий
   class TLoadDialogWizardPathPage : public QWizardPage
   {
      Q_OBJECT

   public:
      friend class TLoadDialogWizard;

      explicit TLoadDialogWizardPathPage(QWidget *Parent_ = 0);
      virtual ~TLoadDialogWizardPathPage();

      //! \brief Проверка правильности ввода на странице
      //! \return bool - true, если страница заполнена правильно
      bool isComplete() const;

   private:
      Q_DISABLE_COPY(TLoadDialogWizardPathPage)

      //! \var Описание содержания страницы и действий на ней
      QLabel* m_Label;
      //! \var Указатель на общий лайоут
      QVBoxLayout *m_PathLayout;
      //! \var Указатель класс ввода пути
      TPathField* m_SavePath;
      //! \var Указатель на класс ввода префикса
      TTextField* m_JobsPrefix;
      //! \var Указатель на спайсер
      QSpacerItem *m_Spacer;
      //! \var Указатель на родительский виджет
      QWidget *m_Parent;
      //! \var ключ организации для реестра
      QString m_Organization;
      //! \var ключ приложения для реестра
      QString m_Application;

   public slots:
   };
}
#endif // QNETMAP_LOADDIALOG_WIZARD_H
