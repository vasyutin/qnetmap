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

#ifndef QNETMAP_RASTERMAPSDIALOG_H
#define QNETMAP_RASTERMAPSDIALOG_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QDialog>
   #include <QVBoxLayout>
   #include <QPushButton>
   #include <QLabel>
   #include <QDialogButtonBox>
   #include <QListView>
#endif
#include <QModelIndex>
#include <QStringListModel>
#include <QMutex>
#include <cmath>

#include "qnetmap_interfaces.h"

namespace qnetmap
{
   class TMapAdapter;
   class TMapWidget;
   class TPathField;
   class TTextField;
   class TRasterMapFields;
   class TRasterMapsDialog;
   //
   namespace TRasterMapsMembers
   {
      //! \class Параметры привязки точки 
      class TMapAnchor 
      {
      public:
         TMapAnchor(QPoint DisplayPoint_, QPointF GeographicPoint_, QPointF NewGeographicPoint_)
            : m_BaseDisplayPoint(QPoint(0, 0)),
              m_DisplayPoint(DisplayPoint_),
              m_GeographicPoint(GeographicPoint_),
              m_NewGeographicPoint(NewGeographicPoint_) {}
         virtual ~TMapAnchor() {}

         inline bool operator==(const TMapAnchor &Anchor_) const
         {
         return m_DisplayPoint == Anchor_.m_DisplayPoint;
         }
         inline bool operator<(const TMapAnchor &Anchor_) const
         {
         return distance() < Anchor_.distance();
         }
         void setBasePoint(const QPoint Point_) { m_BaseDisplayPoint = Point_; }
         QPoint baseDisplayPoint(void) const { return m_BaseDisplayPoint; }
         void setDisplayPoint(const QPoint Point_, const TMapAdapter* MapAdapter_);
         QPoint displayPoint(void) const { return m_DisplayPoint; }
         QPointF geographicPoint(void) const { return m_GeographicPoint; }
         QPointF newGeographicPoint(void) const { return m_NewGeographicPoint; }
         //! \brief
         void move(const QPoint NewBaseDisplayPoint_, const TMapAdapter* MapAdapter_);
         int localX(void) const { return m_DisplayPoint.x() - m_BaseDisplayPoint.x(); }
         int localY(void) const { return m_DisplayPoint.y() - m_BaseDisplayPoint.y(); }

      private:
         //! \brief Расстояние от базовой точки
         inline qreal distance(void) const
         {
         return sqrt(pow(qreal(m_DisplayPoint.x() - m_BaseDisplayPoint.x()), 2.) +
                     pow(qreal(m_DisplayPoint.y() - m_BaseDisplayPoint.y()), 2.));
         }

         //! \var глобальные пиксельные координаты базовой точки
         QPoint  m_BaseDisplayPoint;    
         //! \var глобальные пиксельные координаты точки
         QPoint  m_DisplayPoint;        
         //! \var географические координаты точки
         QPointF m_GeographicPoint;     
         //! \var новые географические координаты точки
         QPointF m_NewGeographicPoint;  
      };

      //! \struct Параметры растровой карты
      struct TRasterMapFields 
      {
         QString MapPath; // Путь к файлу карты
         QString MapName; // Имя карты
         };

      //! \class Кнопка удаления точки привязки
      class TDeleteCoordinatesButton : public QPushButton
      {
         Q_OBJECT

      public:
         TDeleteCoordinatesButton(
            TRasterMapsDialog* Dialog_, TMapAnchor Anchor_, QHBoxLayout* Layout_, QWidget* Parent_ = NULL);
         virtual ~TDeleteCoordinatesButton() {}

      private slots:
         void removeLayout(void);

      private:
         Q_DISABLE_COPY(TDeleteCoordinatesButton)

         QHBoxLayout* w_Layout;
         TRasterMapsDialog* w_Dialog;
         QPixmap m_Pixmap;
         TMapAnchor m_Anchor;
      };

      //! \class Окно ввода пути и названия растровой карты
      class TRasterMapPathDialog : public QDialog 
      {
         Q_OBJECT

      public:
         //! \brief
         TRasterMapPathDialog(
            QString Organization_, QString Application_, QStringList* ListNames_,
            TRasterMapFields* MapFields_, QWidget* Parent_ = NULL);
         virtual ~TRasterMapPathDialog() {}

      public slots:
         //! \brief Отрабатывает проверки при изменении полей ввода
         void fieldsChanged(void); 

      private:
         Q_DISABLE_COPY(TRasterMapPathDialog)

         //! \var поле ввода пути к файлу карты
         TPathField* w_PathField;
         //! \var Поле ввода названия карты
         TTextField* w_NameField;
         //! \var Параметры карты
         TRasterMapFields* w_MapFields;
         //! \var Список
         QStringList* w_ListNames;
         //! \var Кнопки
         QDialogButtonBox m_ButtonBox;
         //! \var Кнопка ОК
         QAbstractButton* w_ButtonOK;
      };
   }

   //! \class Окно работы с растровыми картами
   class QNETMAP_EXPORT TRasterMapsDialog : public QDialog 
   {
      Q_OBJECT

   public:
      friend class TRasterMapsMembers::TDeleteCoordinatesButton;
      //! \struct Consts
      struct Consts {
         static const int ListMode    = __LINE__; // режим работы со списком карт
         static const int LinkingMode = __LINE__; // режим привязки карты
         };

      TRasterMapsDialog(IMapAdapterInterfaces MapInterfaces_,
                        QString Organization_, QString Application_, QWidget *Parent_ = NULL); 
      virtual ~TRasterMapsDialog();

      //! \brief Возвращает указатель на виджет карты
      TMapWidget* mapWidget(void) const { return w_MapWidget; }

   private slots:
      //! \brief Удаление привязки
      void deleteLinking(void);
      //! \brief Формирование world-файла 
      void acceptLinking(void);
      //! \brief Отмена диалога привязки
      void rejectLinking(void);
      //! \brief Добавляет карту в список
      void addMap(void);
      //! \brief Удаляет карту из списка
      void deleteMap(void);
      //! \brief Отображает текущую карту
      void viewMap(void);
      //! \brief Отрабатывает выбор карты в списке
      void mapNameClicked(const QModelIndex&);
      //! \brief Управлет отображением и реакцией виджетов
      void widgetsEnabling(void);
      //! \brief Переход в центр карты
      void viewCenterMap(void);
      //! \brief Обрабатывает выбор точки привязки с помощью мыши, показывает дталог ввода координат
      void mouseSelectPointEvent(const QMouseEvent*, const QPoint, const QPointF);

   private:
      Q_DISABLE_COPY(TRasterMapsDialog)

      //! \brief Переход в режим списка карт
      void viewList(void);
      //! \brief Устанавливает сообщение вверху окна
      void hint(QString Hint_);
      //! \brief Сохраняет предыдущее сообщение в стек
      void pushHint(void) { m_Hints.push_front(m_TextHint.text()); }
      //! \brief Восстанавливает сообщение из стека
      bool popHint(void) 
      {
      if(m_Hints.size()) { 
         m_TextHint.setText(*(m_Hints.begin())); 
         m_Hints.pop_front();
         return true; 
         }
      return false;
      }
      //! \brief Отображает список точек привязки
      void viewLinkingPoints(void);
      //! \brief Очищает список точек привязки
      void clearLinkingPointsLayout(void);
      //! \brief Устанавливает подсказку
      void hintCtrl(void);
      //! \brief Удаляет лайоут со всем содержимым
      void removeLayout(QLayout* Layout_);
      //! \brief Проверка существования world-файла
      bool worldFileExists(void);

      //! \var Число ПИ
      qreal PI;
      //! \var Подсказка
      QLabel m_TextHint;
      //! \var Путь к карте
      QLabel m_MapPath;
      //! \var Кнопки
      QDialogButtonBox m_ButtonBox;
      //! \var Основной лайоут
      QVBoxLayout m_MainVerticalLayout;
      //! \var Указатель на виджет с картой
      TMapWidget* w_MapWidget;
      //! \var Указатель на кнопку "ОК"
      QAbstractButton* w_ButtonOK;
      //! \var Список карт
      QListView m_MapsListView;
      //! \var Модель списка
      QStringListModel m_Model;
      //! \var Кнопка добавления
      QPushButton m_AddButton;
      //! \var Кнопка удаления
      QPushButton m_DeleteButton;
      //! \var Кнопка привязки
      QPushButton m_LinkingButton;
      //! \var Кнопка удаления привязки
      QPushButton m_DeleteLinkingButton;
      //! \var Кнопка перехода в центр карты
      QPushButton m_CenterMapButton;
      //! \var Кнопка завершения работы
      QPushButton m_ExitDialogButton;
      //! \var Организация (для поиска в реестре)
      QString m_Organization;
      //! \var Приложение (для поиска в реестре)
      QString m_Application;
      //! \var Режим отображения
      int m_Mode;
      //! \var Стек подсказок
      QList<QString> m_Hints;
      //! \var Список точек привязки
      QList<TRasterMapsMembers::TMapAnchor> m_MapAnchors;
      //! \var Лайоут, отображающий список точек привязки
      QVBoxLayout* w_MapAnchors;
      //! \var Список лайоутов, содержащих по одной точке привязки
      QList<QLayout*> m_LinkingPointsLayouts;
      //! \var Признак нажатия на клавишу Ctrl
      bool m_KeyControlPressed;
      //! \var Мьютех признака нажатия на клавишу Ctrl
      QMutex m_KeyControlMutex;

   protected:
      void keyPressEvent(QKeyEvent* Event_);
      void keyReleaseEvent(QKeyEvent *Event_);
   };
}
#endif // #ifndef QNETMAP_RASTERMAPSDIALOG_H
