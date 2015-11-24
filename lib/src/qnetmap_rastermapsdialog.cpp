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

#include "qnetmap_rastermapsdialog.h"

#include "../images/qnetmap_deleteicon.hex"
#include "qnetmap_currentcoordinate.h"
#include "qnetmap_layer.h"
#include "qnetmap_visual_elments.h"
#include "qnetmap_coordinatesdialog.h"
#include "qnetmap_rastermapadapter.h"
#include "qnetmap_widget.h"

#include <set>

//using namespace qnetmap::TRasterMapsMembers;

namespace qnetmap
{
   // -------------------------------------------------------------
   TRasterMapsMembers::TRasterMapPathDialog::TRasterMapPathDialog(
      QString Organization_, QString Application_, QStringList* ListNames_,
      TRasterMapFields* MapFields_, QWidget* Parent_) 
      : QDialog(Parent_), w_MapFields(MapFields_), w_ListNames(ListNames_)
   {
   setWindowTitle(QNetMapTranslator::tr("Add raster map" /* Russian: Добавление растровой карты */));
   setMinimumWidth(500);
   QVBoxLayout* MainLayout = new QVBoxLayout;
   // поля ввода
   w_PathField = new TPathField(
      QNetMapTranslator::tr("Raster map file" /* Russian: Файл с растровой картой */),
      Organization_, Application_, QNetMapConsts::FilePathKey, 10, 
      TPathField::Consts::BrowseFile,
      QNetMapTranslator::tr("Images" /* Russian: Изображения */) + 
		QString(" (*.png *.tif *.jpg *.bmp *.gif)"));
   w_NameField = new TTextField(
      QNetMapTranslator::tr("Display name" /* Russian: Отображаемое имя */),
      Organization_, Application_, QNetMapConsts::NameKey);
   // Кнопки подтверждения / отмены
   m_ButtonBox.setMinimumSize(QSize(0, 30));
   m_ButtonBox.setOrientation(Qt::Horizontal);
   m_ButtonBox.setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
   /// находим кнопку ОК и делаем ее недоступной
   QList<QAbstractButton*> ListButtons = m_ButtonBox.buttons();
   foreach(QAbstractButton* Button, ListButtons) {
      if(m_ButtonBox.buttonRole(Button) == QDialogButtonBox::AcceptRole) {
         Button->setEnabled(false);
         w_ButtonOK = Button;
         break;
         }
      }
   // все помещаем на основной лайоут
   MainLayout->addWidget(w_PathField);
   MainLayout->addWidget(w_NameField);
   MainLayout->addWidget(&m_ButtonBox);
   setLayout(MainLayout);
   // сигналы
   QNM_DEBUG_CHECK(connect(w_PathField, SIGNAL(editTextChanged(const QString&)), this, SLOT(fieldsChanged())));
   QNM_DEBUG_CHECK(connect(w_NameField, SIGNAL(editTextChanged(const QString&)), this, SLOT(fieldsChanged())));
   QNM_DEBUG_CHECK(connect(&m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept())));
   QNM_DEBUG_CHECK(connect(&m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject())));
   //
   fieldsChanged();
   }

   // -------------------------------------------------------------
   void TRasterMapsMembers::TRasterMapPathDialog::fieldsChanged(void) 
   {
   QString Name = w_NameField->currentText().trimmed();
   QString Path = w_PathField->currentText().trimmed();
   w_MapFields->MapName = Name;
   w_MapFields->MapPath = Path;
   // все поля правильно заполнены ?
   if(!Name.isEmpty() && !Path.isEmpty() && QFile(Path).exists()) {
      // а есть-ли такое имя в списке?
      bool Enabled = true;
      for(QStringList::const_iterator it = w_ListNames->begin(); it != w_ListNames->end(); ++it) {
         if(Name == *it) { Enabled = false; break; }
         }
      w_ButtonOK->setEnabled(Enabled);
      }
   else w_ButtonOK->setEnabled(false);
   }

   // -------------------------------------------------------------
   TRasterMapsMembers::TDeleteCoordinatesButton::TDeleteCoordinatesButton(
      TRasterMapsDialog* Dialog_, TMapAnchor Anchor_, QHBoxLayout* Layout_, QWidget* Parent_) 
      : QPushButton(Parent_), w_Layout(Layout_), w_Dialog(Dialog_), m_Anchor(Anchor_)
   {
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   setMaximumSize(QSize(11, 11));
   setFlat(true);
   m_Pixmap.loadFromData(qnetmap_deleteicon, sizeof(qnetmap_deleteicon));
   setIcon(QIcon(m_Pixmap));
   //
   QNM_DEBUG_CHECK(connect(this, SIGNAL(clicked()), SLOT(removeLayout())));
   }

   // -------------------------------------------------------------
   void TRasterMapsMembers::TDeleteCoordinatesButton::removeLayout(void)
   {
   w_Dialog->removeLayout(w_Layout);
   int i = w_Dialog->m_MapAnchors.indexOf(m_Anchor);
   w_Dialog->m_MapAnchors.removeAt(i);
   }

   // -------------------------------------------------------------
   TRasterMapsDialog::TRasterMapsDialog(IMapAdapterInterfaces MapInterfaces_,
                                        QString Organization_, QString Application_, QWidget *Parent_)
      : QDialog(Parent_), PI(acos(-1.0)), m_Organization(Organization_), m_Application(Application_),
        m_Mode(Consts::ListMode), m_KeyControlPressed(false)
   {
   setWindowTitle(QNetMapTranslator::tr("Raster map list" /* Russian: Список растровых карт */));
   QPalette palette/*(palette())*/;
   palette.setColor(backgroundRole(), Qt::white);
   setPalette(palette);
   setAutoFillBackground(true);

   setMinimumSize(800, 600);
   // карта
   w_MapWidget = new TMapWidget(MapInterfaces_, "", 12, QPointF(0,0), Organization_, Application_);
   // Лайаут основной
   setLayout(&m_MainVerticalLayout);
   m_MainVerticalLayout.setSpacing(8);
   m_MainVerticalLayout.setMargin(8);

   // Лайаут для размещения элементов верхней строки с подсказкой
   QHBoxLayout *TopLayout = new QHBoxLayout;
   m_MainVerticalLayout.addLayout(TopLayout);
   TopLayout->addWidget(&m_TextHint);
   hint(QNetMapTranslator::tr("Select the map or action" /* Russian: Выберите карту или действие */));
   TopLayout->addStretch();
   // Лайаут для списка, кнопок и карты
   QHBoxLayout *MiddleLayout = new QHBoxLayout;
   m_MainVerticalLayout.addLayout(MiddleLayout);
   // Лайаут для списка и кнопок
   QVBoxLayout *ListLayout = new QVBoxLayout;
   // Лайаут для карты
   QVBoxLayout *MapLayout = new QVBoxLayout;

   // Путь к карте
   MapLayout->addWidget(&m_MapPath);
   // Рамка для виджета карты
   QFrame *MapWidgetFrame = new QFrame;
   MapWidgetFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
   QHBoxLayout *MapWidgetFrameLayout = new QHBoxLayout(MapWidgetFrame);
   MapWidgetFrameLayout->setMargin(0);
   MapWidgetFrameLayout->addWidget(w_MapWidget);
   MapLayout->addWidget(MapWidgetFrame);
   // лайоут для списка точек привязки
   w_MapAnchors = new QVBoxLayout;
   w_MapAnchors->setMargin(0);
   MapLayout->addLayout(w_MapAnchors);

   // Список карт
   QStringList ListMaps;
   IMapAdapterInterfaces RasterMapInterfaces = w_MapWidget->rasterMapInterfaces();
   foreach(IMapAdapterInterface* Interface, RasterMapInterfaces) {
      ListMaps << Interface->pluginName();
      }
   m_Model.setStringList(ListMaps);
   m_MapsListView.setModel(&m_Model);
   ListLayout->addWidget(&m_MapsListView);
   QGroupBox *ListGroup = new QGroupBox(QNetMapTranslator::tr("Raster maps" /* Russian: Растровые карты */));
   ListGroup->setMaximumWidth(200);
   ListGroup->setLayout(ListLayout);
   
   // кнопки
   m_CenterMapButton.setText(QNetMapTranslator::tr("Map's center" /* Russian: Центр карты */));
   m_LinkingButton.setText(QNetMapTranslator::tr("Add reference points..." /* Russian: Привязать... */));
   m_DeleteLinkingButton.setText(QNetMapTranslator::tr("Delete reference points" /* Russian: Удалить привязку */));
   m_AddButton.setText(QNetMapTranslator::tr("Add to list..." /* Russian: Добавить в список... */));
   m_DeleteButton.setText(QNetMapTranslator::tr("Delete from list" /* Ru: Удалить из списка */));
   m_ExitDialogButton.setText(QNetMapTranslator::tr("OK"));
   ListLayout->addWidget(&m_CenterMapButton);
   ListLayout->addWidget(&m_LinkingButton);
   ListLayout->addWidget(&m_DeleteLinkingButton);
   ListLayout->addWidget(&m_AddButton);
   ListLayout->addWidget(&m_DeleteButton);
   ListLayout->addItem(new QSpacerItem(0, 7));
   ListLayout->addWidget(&m_ExitDialogButton);

   // Кнопки подтверждения / отмены
   m_ButtonBox.setMinimumSize(QSize(0, 30));
   m_ButtonBox.setOrientation(Qt::Horizontal);
   m_ButtonBox.setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
   // находим кнопку ОК и делаем ее недоступной
   QList<QAbstractButton*> ListButtons = m_ButtonBox.buttons();
   foreach(QAbstractButton* Button, ListButtons) {
      if(m_ButtonBox.buttonRole(Button) == QDialogButtonBox::AcceptRole) {
         w_ButtonOK = Button;
         w_ButtonOK->setEnabled(false);
         break;
         }
      }
   MapLayout->addWidget(&m_ButtonBox);
   m_ButtonBox.setVisible(false);   

   MiddleLayout->addWidget(ListGroup);
   MiddleLayout->addLayout(MapLayout);
   // устанавливаем текущей первую карту в списке
   if(m_Model.rowCount()) mapNameClicked(m_Model.index(0, 0));

   // сигналы
   QNM_DEBUG_CHECK(connect(&m_MapsListView,     SIGNAL(clicked(const QModelIndex&)),   
                  this,                SLOT(mapNameClicked(const QModelIndex&))));
   QNM_DEBUG_CHECK(connect(&m_MapsListView,     SIGNAL(activated(const QModelIndex&)), 
                  this,                SLOT(mapNameClicked(const QModelIndex&))));
   QNM_DEBUG_CHECK(connect(&m_ButtonBox,        SIGNAL(accepted()), this, SLOT(acceptLinking())));
   QNM_DEBUG_CHECK(connect(&m_ButtonBox,        SIGNAL(rejected()), this, SLOT(rejectLinking())));
   QNM_DEBUG_CHECK(connect(&m_AddButton,        SIGNAL(clicked()),  this, SLOT(addMap())));
   QNM_DEBUG_CHECK(connect(&m_DeleteButton,     SIGNAL(clicked()),  this, SLOT(deleteMap())));
   QNM_DEBUG_CHECK(connect(&m_LinkingButton,    SIGNAL(clicked()),  this, SLOT(viewMap())));
   QNM_DEBUG_CHECK(connect(&m_DeleteLinkingButton, SIGNAL(clicked()), this, SLOT(deleteLinking())));
   QNM_DEBUG_CHECK(connect(&m_CenterMapButton,  SIGNAL(clicked()),  this, SLOT(viewCenterMap())));
   QNM_DEBUG_CHECK(connect(&m_ExitDialogButton, SIGNAL(clicked()),  this, SLOT(accept())));
   //
   widgetsEnabling();
   }

   // -------------------------------------------------------------
   TRasterMapsDialog::~TRasterMapsDialog()
   {
   delete w_MapWidget;
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::widgetsEnabling(void)
   {
   bool ListIsNotEmpty = m_Model.rowCount() > 0;
   m_LinkingButton.setEnabled(ListIsNotEmpty);
   m_DeleteLinkingButton.setEnabled(worldFileExists());
   m_DeleteButton.setEnabled(ListIsNotEmpty);
   m_CenterMapButton.setEnabled(ListIsNotEmpty);
   w_MapWidget->setVisibleBaseMap(ListIsNotEmpty);
   w_MapWidget->setEnabled(ListIsNotEmpty);
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::viewCenterMap(void)
   {
   w_MapWidget->setView(w_MapWidget->baseMapAdapter()->centerMap());
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::deleteLinking(void)
   {
   TMapAdapter* MapAdapter = w_MapWidget->baseMapAdapter();
   QString RasterFilePath = MapAdapter->rasterFileFullPath().trimmed();
   //
   QString WorldFilePath  = RasterFilePath + "w";
   QFile WorldFile(WorldFilePath);
   if(WorldFile.exists()) { 
      QMessageBox MessageBox(QMessageBox::Question, 
                             QNetMapTranslator::tr("Deleting reference points" /* Ru: Удаление привязки */), 
                             QNetMapTranslator::tr("Do you really want to delete reference points?" /* Ru: Удалить привязку карты? */),
                             QMessageBox::Ok | QMessageBox::Cancel);
      if(MessageBox.exec() == QMessageBox::Ok && WorldFile.remove()) {
         MapAdapter->setRasterFileFullPath(RasterFilePath);
         viewCenterMap();
         mapNameClicked(m_MapsListView.currentIndex());
         w_MapWidget->rasterMapsLinkChanged();
         }
      else {
         QMessageBox::critical(NULL, QNetMapTranslator::tr("Error"),
            QNetMapTranslator::tr("Error deleting reference points file \"%1\""
					/* Ru: Невозможно удалить файл привязки "%1" */).arg(WorldFilePath));
         }
      }
   }

   // -------------------------------------------------------------
   bool TRasterMapsDialog::worldFileExists(void)
   {
   QFile WorldFile(w_MapWidget->baseMapAdapter()->rasterFileFullPath().trimmed() + "w");
   return WorldFile.exists();
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::rejectLinking(void)
   {
   viewList();
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::addMap(void)
   {
   TRasterMapsMembers::TRasterMapFields MapFields;
   QStringList ListNames = m_Model.stringList();
   TRasterMapsMembers::TRasterMapPathDialog Dialog(m_Organization, m_Application, &ListNames, &MapFields);
   //
   if(Dialog.exec() == QDialog::Accepted) {
      // добавляем в список интерфейсов виджета карты
      if(w_MapWidget->addRasterMap(MapFields.MapPath, MapFields.MapName)) {
         // запись в реестр
         QSettings Settings(m_Organization, m_Application);
         QString TemplatePropertyFilePath = QString(QNetMapConsts::RasterMapFilePathKey) + "_%1";
         QString TemplatePropertyMapName  = QString(QNetMapConsts::RasterMapNameKey)  + "_%1";
         int i = 0;
         while(Settings.contains(TemplatePropertyFilePath.arg(i)) &&
               Settings.contains(TemplatePropertyMapName.arg(i))) { i++; }
         Settings.setValue(TemplatePropertyMapName.arg(i),  MapFields.MapName);
         Settings.setValue(TemplatePropertyFilePath.arg(i), MapFields.MapPath);
         // добавляем в ListView
         int Row = m_Model.rowCount();
         m_Model.insertRows(Row, 1);
         m_Model.setData(m_Model.index(Row), MapFields.MapName);
         w_MapWidget->showMapName(MapFields.MapName);
         viewCenterMap();
         mapNameClicked(m_Model.index(Row));
         // видимость кнопок и виджета карты
         widgetsEnabling();
         }
      else assert(!"Растровая карта не добавлена");
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::deleteMap(void)
   {
   QModelIndex Index = m_MapsListView.currentIndex();
   if(Index.row() >= 0) {
      QString MapName = m_Model.data(Index, Qt::DisplayRole).toString();
      QMessageBox messageBox(QMessageBox::Question, 
                             QNetMapTranslator::tr("Deleting map" /* Ru: Удаление карты */), 
                             QNetMapTranslator::tr("Delete map \"%1\" from the list?"
									  /* Ru: Удалить карту "%1" из списка */).arg(MapName),
                             QMessageBox::Ok | QMessageBox::Cancel);
      if(messageBox.exec() == QMessageBox::Ok) {
         if(w_MapWidget->deleteRasterMap(MapName)) {
            // удаляем из реестра
            w_MapWidget->saveRasterMapInterfaces();
            // удаляем из ListView
            m_Model.removeRows(Index.row(), 1);
            // показываем карту из оставшихся
            Index = m_MapsListView.currentIndex();
            if(Index.row() >= 0) mapNameClicked(Index);
            else                 m_MapPath.setText(QString());
            // видимость кнопок и карты
            widgetsEnabling();
            }
         }
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::viewList(void)
   {
   m_Mode = Consts::ListMode;
   popHint();
   disconnect(w_MapWidget, 
              SIGNAL(mouseSelectPointEvent(const QMouseEvent*, const QPoint, const QPointF, const QPixmap)),
              this, SLOT(mouseSelectPointEvent(const QMouseEvent*, const QPoint, const QPointF)));
   //
   m_MapsListView.setEnabled(true);
   m_AddButton.setEnabled(true);
   m_ExitDialogButton.setEnabled(true);
   m_ButtonBox.setVisible(false);
   clearLinkingPointsLayout();
   widgetsEnabling();
   // удаляем объекты с карты
   TLayer* GeometryLayer = w_MapWidget->layer("Geometries Layer");
   if(GeometryLayer) GeometryLayer->clearGeometries();
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::viewMap(void)
   {
   QString FilePath = w_MapWidget->baseMapAdapter()->rasterFileFullPath();
   // выделяем путь к файлу
   QFileInfo fi(FilePath);
   QDir Folder;
   if(fi.isDir()) Folder = QDir(FilePath);
   else           Folder = fi.dir();
   QString Path = Folder.absolutePath();
   // проверяем на возможность записи
   if(TQNetMapHelper::checkDirectoryForWritePermissions(Path)) {
      QMessageBox messageBox(QMessageBox::Question, 
                             QNetMapTranslator::tr("Adding reference points" /* Ru: Привязка карты */), 
                             QNetMapTranslator::tr("The existing reference points will be overwritten. Confirm?"
									  /* Ru: Существующий файл привязки карты будет перезаписан. Разрешить? */),
                             QMessageBox::Ok | QMessageBox::Cancel);
      if(!worldFileExists() || messageBox.exec() == QMessageBox::Ok) {
         // входим в режим изменения привязки карты
         m_Mode = Consts::LinkingMode;
         m_MapAnchors.clear();
         pushHint();
         hintCtrl();
         QNM_DEBUG_CHECK(connect(
            w_MapWidget, 
            SIGNAL(mouseSelectPointEvent(const QMouseEvent*, const QPoint, const QPointF, const QPixmap)),
            SLOT(mouseSelectPointEvent(const QMouseEvent*, const QPoint, const QPointF))));
         //
         m_MapsListView.setEnabled(false);
         m_AddButton.setEnabled(false);
         m_LinkingButton.setEnabled(false);
         m_DeleteLinkingButton.setEnabled(false);
         m_DeleteButton.setEnabled(false);
         m_ExitDialogButton.setEnabled(false);
         m_ButtonBox.setVisible(true);
         }
      }
   else {
      // не можем писать в папку
      QMessageBox::critical(this, 
			QNetMapTranslator::tr("Access rights error" /* Ru: Ошибка прав доступа */),
         QNetMapTranslator::tr("Can't write to folder \"%1\"."
				/* Ru: Папка "%1" недоступна для записи */).arg(Path));
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::mapNameClicked(const QModelIndex& Index)
   {
   if(Index.row() >= 0 && Index.row() < m_Model.rowCount()) {
      QString MapName = m_Model.data(Index, Qt::DisplayRole).toString();
      w_MapWidget->showMapName(MapName);
      viewCenterMap();
      if(worldFileExists()) {
         m_DeleteLinkingButton.setEnabled(true);
         m_MapPath.setText(w_MapWidget->baseMapAdapter()->rasterFileFullPath());
         }
      else {
         m_DeleteLinkingButton.setEnabled(false);
         m_MapPath.setText(
            "<font color=\"red\">(" + 
				QNetMapTranslator::tr("No reference points" /* Ru: Нет привязки */) + QString(") ") + 
            w_MapWidget->baseMapAdapter()->rasterFileFullPath() + "</font>");
         }
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::keyPressEvent(QKeyEvent *Event_)
   {
   switch(Event_->key()) {
   case Qt::Key_Control:
      {
      QMutexLocker Locker(&m_KeyControlMutex);
      if(m_Mode == Consts::LinkingMode && m_MapAnchors.size() < 3) {
         m_KeyControlPressed = true;
         w_MapWidget->saveMouseMode();
         w_MapWidget->setMouseMode(TMapControl::SelectPoint);
         pushHint();
         hint(QNetMapTranslator::tr("Click left mouse button on the reference point" 
				/* Ru: Выберите точку привязки и щелкните мышью */));
         }
      break;
      }
   default:
      break;
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::keyReleaseEvent(QKeyEvent *Event_)
   {
   switch(Event_->key()) {
   case Qt::Key_Control:
      {
      QMutexLocker Locker(&m_KeyControlMutex);
      if(m_Mode == Consts::LinkingMode && m_KeyControlPressed) {
         m_KeyControlPressed = false;
         w_MapWidget->restoreMouseMode();
         popHint();
         }
      break;
      }
   default:
      break;
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::mouseSelectPointEvent(
      const QMouseEvent* Event_, const QPoint Point_, const QPointF PointF_)
   {
   Q_UNUSED(Event_)

   QPointF NewPoint;
   TCoordinatesDialog Dialog(&NewPoint);
   if(Dialog.exec() == QDialog::Accepted) {
      m_MapAnchors.push_back(TRasterMapsMembers::TMapAnchor(Point_, PointF_, NewPoint));
      w_ButtonOK->setEnabled(m_MapAnchors.size() > 1);
      viewLinkingPoints();
      }
   //
   QMutexLocker Locker(&m_KeyControlMutex);
   if(m_Mode == Consts::LinkingMode && m_KeyControlPressed) {
      m_KeyControlPressed = false;
      w_MapWidget->restoreMouseMode();
      popHint();
      }
   //
   if(m_MapAnchors.size() == 3) {
      hint(QNetMapTranslator::tr("Confirm selected coordinates" 
			/* Ru: Подтвердите введенные координаты */));
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::hint(QString Hint_)
   {
   m_TextHint.setText(Hint_);
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::viewLinkingPoints(void)
   {
   if(m_MapAnchors.size()) {
      // удаляем все виджеты из лайоута
      clearLinkingPointsLayout();
      // добавляем виджеты для измененного списка
      foreach(TRasterMapsMembers::TMapAnchor Anchor, m_MapAnchors) {
         QHBoxLayout *Layout = new QHBoxLayout;
         Layout->setMargin(0);
         Layout->setContentsMargins(10, 0, 0, 0);
         QLabel* Label = new QLabel;
         TRasterMapsMembers::TDeleteCoordinatesButton* DeleteButton = 
            new TRasterMapsMembers::TDeleteCoordinatesButton(this, Anchor, Layout);
         TCurrentCoordinate::setView(TCurrentCoordinate::Consts::DegreeCoordinatesLat);
         Label->setText(TCurrentCoordinate::textCoordinates(Anchor.newGeographicPoint(), mapWidget()));
         m_LinkingPointsLayouts.push_front(Layout);
         Layout->addWidget(DeleteButton);
         Layout->addWidget(Label);
         w_MapAnchors->addLayout(Layout);
         }
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::clearLinkingPointsLayout(void)
   {
   while(m_LinkingPointsLayouts.size()) {
      QLayout* Layout = *(m_LinkingPointsLayouts.begin());
      removeLayout(Layout);
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::removeLayout(QLayout* Layout_)
   {
   if(Layout_) {
      while(Layout_->count() > 0) {
         QLayoutItem* Item = Layout_->itemAt(0);
         QWidget* Widget = Item->widget();
         Layout_->removeWidget(Widget);
         Widget->deleteLater();
         }
      w_MapAnchors->removeItem(Layout_);
      Layout_->deleteLater();
      int i = m_LinkingPointsLayouts.indexOf(Layout_);
      m_LinkingPointsLayouts.removeAt(i);
      }
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::hintCtrl(void)
   {
   hint(QNetMapTranslator::tr("To add reference point press Ctrl key and click left mouse button"
		/* Ru: Для выбора точки привязки нажмите Ctrl и щелкните мышью */));
   }

   // -------------------------------------------------------------
   void TRasterMapsDialog::acceptLinking(void)
   {
   TMapAdapter* MapAdapter = w_MapWidget->baseMapAdapter();
   //
   std::multiset<TRasterMapsMembers::TMapAnchor> MapAnchors;
   foreach(TRasterMapsMembers::TMapAnchor Anchor, m_MapAnchors) { MapAnchors.insert(Anchor); }
   //
   std::multiset<TRasterMapsMembers::TMapAnchor> Anchors;
   // определяем самую близкую к верхнему левому углу карты точку (будет первой в set)
   QPointF TopLeftPointF = MapAdapter->topLeftMap();
   QPoint  TopLeftPoint  = MapAdapter->coordinateToDisplay(TopLeftPointF);
   foreach(TRasterMapsMembers::TMapAnchor Anchor, MapAnchors) { 
      Anchor.setBasePoint(TopLeftPoint);  // устанавливаем базовую точку
      Anchors.insert(Anchor); 
      }
   MapAnchors.clear();
   // по первой точке в set смещаем все точки привязки
   TRasterMapsMembers::TMapAnchor FirstAnchor = *(Anchors.begin());
   //QPointF PointF = FirstAnchor.geographicPoint();
   QPointF NewPointF = FirstAnchor.newGeographicPoint();
   QPoint  NewPoint  = MapAdapter->coordinateToDisplay(NewPointF);
   /// координаты смещенной верхней левой точки пиксельные и географические
   TopLeftPoint  = QPoint(NewPoint.x() - FirstAnchor.displayPoint().x() + TopLeftPoint.x(), 
                          NewPoint.y() - FirstAnchor.displayPoint().y() + TopLeftPoint.y());
   TopLeftPointF = MapAdapter->displayToCoordinate(TopLeftPoint);
   /// пересчитываем координаты всех точек, смещая базовую точку
   foreach(TRasterMapsMembers::TMapAnchor Anchor, Anchors) {
      Anchor.move(TopLeftPoint, MapAdapter);
      MapAnchors.insert(Anchor);
      }
   FirstAnchor = *(MapAnchors.begin());
   // вычисляем масштабы пиксела по осям
   qreal PixelXScale = 0, PixelYScale = 0;
   QSizeF MovedMapSize = MapAdapter->mapSize();
   QSizeF BaseMapSize  = MapAdapter->baseMapSize();
   /// инициализируем крайние новые географические координаты
   qreal NewMaxXF = FirstAnchor.newGeographicPoint().x(), NewMinXF = NewMaxXF;
   qreal NewMaxYF = FirstAnchor.newGeographicPoint().y(), NewMinYF = NewMaxYF;
   /// инициализируем старые крайние пиксельные координаты
   int MaxX = FirstAnchor.displayPoint().x(), MinX = MaxX;
   int MaxY = FirstAnchor.displayPoint().y(), MinY = MaxY;
   /// инициализируем новые крайние пиксельные координаты
   QPoint Point = MapAdapter->coordinateToDisplay(FirstAnchor.newGeographicPoint());
   int NewMaxX = Point.x(), NewMinX = NewMaxX;
   int NewMaxY = Point.y(), NewMinY = NewMaxY;
   /// определяем крайние координаты
   foreach(TRasterMapsMembers::TMapAnchor Anchor, MapAnchors) {
      int X = Anchor.displayPoint().x(), Y = Anchor.displayPoint().y();
      Point = MapAdapter->coordinateToDisplay(Anchor.newGeographicPoint());
      int NewX = Point.x(), NewY = Point.y();
      qreal XF = Anchor.newGeographicPoint().x(), YF = Anchor.newGeographicPoint().y();
      if(XF < NewMinXF) { NewMinXF = XF; NewMinX = NewX; MinX = X; }
      if(XF > NewMaxXF) { NewMaxXF = XF; NewMaxX = NewX; MaxX = X; }
      if(YF < NewMinYF) { NewMinYF = YF; NewMinY = NewY; MinY = Y; }
      if(YF > NewMaxYF) { NewMaxYF = YF; NewMaxY = NewY; MaxY = Y; }
      }
   // относительные коэффициенты искажения на уровне
   qreal LevelXScale = (qreal)(NewMaxX - NewMinX) / (MaxX - MinX);
   qreal LevelYScale = (qreal)(NewMaxY - NewMinY) / (MaxY - MinY);
   // масштаб на карте
   PixelXScale = MapAdapter->pixelXScale() * LevelXScale * (MovedMapSize.width()  / BaseMapSize.width());
   PixelYScale = MapAdapter->pixelYScale() * LevelYScale * (MovedMapSize.height() / BaseMapSize.height());
   // пересчитываем положение верхней левой точки
   TRasterMapsMembers::TMapAnchor Anchor = *(MapAnchors.begin());
   TopLeftPoint = QPoint(Anchor.displayPoint().x() - Anchor.localX() * LevelXScale,
                         Anchor.displayPoint().y() - Anchor.localY() * LevelYScale);
   TopLeftPointF = MapAdapter->displayToCoordinate(TopLeftPoint);
   // сохраняем world-файл
   bool WorldFileWrited = false;
   QString RasterFilePath = MapAdapter->rasterFileFullPath().trimmed();
   QString WorldFilePath  = RasterFilePath + "w";
   QFile WorldFile(WorldFilePath);
   if((!WorldFile.exists() || 
       WorldFile.remove()) && 
       WorldFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      // ВНИМАНИЕ - записываем координаты середины пиксела!
      qreal X, Y;
      X = TopLeftPointF.x() * PI / 180. * QNetMapConsts::PopularVisualisationSphereRadius + PixelXScale * 0.5;
      Y = log(tan(0.25 * PI + 0.5 * TopLeftPointF.y() * PI / 180.)) * 
          QNetMapConsts::PopularVisualisationSphereRadius + PixelYScale * 0.5;
      //
      QTextStream out(&WorldFile);
      out.setRealNumberPrecision(12);
      out <<  PixelXScale << "\n" << 0.0 << "\n" << 0.0 << "\n";
      out << -PixelYScale << "\n" << X << "\n" << Y << "\n";
      WorldFile.close();
      WorldFileWrited = true;
      //
      MapAdapter->setRasterFileFullPath(RasterFilePath);
      w_MapWidget->rasterMapsLinkChanged();
      }
   else {
      QMessageBox::critical(NULL, QNetMapTranslator::tr("Error"),
         QNetMapTranslator::tr("Can't write data to file \"%1\"."
			/* Ru: Невозможно записать данные в файл "%1". */).arg(WorldFilePath));
      WorldFileWrited = false;
      }
   // переходим в центр, иначе можем потерять карту - увидеть пустое поле
   if(WorldFileWrited) viewCenterMap();
   // переходим к списку растровых карт
   viewList();
   mapNameClicked(m_MapsListView.currentIndex());
   }

   // -------------------------------------------------------------
   void TRasterMapsMembers::TMapAnchor::setDisplayPoint(
      const QPoint Point_, const TMapAdapter* MapAdapter_)
   {
   m_DisplayPoint = Point_;
   m_GeographicPoint = MapAdapter_->displayToCoordinate(Point_);
   }

   // -------------------------------------------------------------
   void TRasterMapsMembers::TMapAnchor::move(
      const QPoint NewBaseDisplayPoint_, const TMapAdapter* MapAdapter_) 
   {
   m_DisplayPoint = QPoint(NewBaseDisplayPoint_.x() + m_DisplayPoint.x() - m_BaseDisplayPoint.x(), 
                           NewBaseDisplayPoint_.y() + m_DisplayPoint.y() - m_BaseDisplayPoint.y());
   m_BaseDisplayPoint = NewBaseDisplayPoint_;
   m_GeographicPoint = MapAdapter_->displayToCoordinate(m_DisplayPoint);
   }
}
