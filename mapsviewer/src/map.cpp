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

#include <algorithm>

#include "map.h"
#include "mapsviewer_translator.h"

#include "../images/pointflag.hex"

#include "../images/about16x16.hex"
#include "../images/create_task16x16.hex"
#include "../images/print_map_16x16.hex"
#include "../images/properties16x16.hex"
#include "../images/view_and_print_map16.hex"

#include "qnetmap_pngimage.h"
#include <qnetmap_version.h>

#include "gpx.h"

//------------------------------------------------------------------
TMap::TMap(QWidget*) 
   : m_PluginsAreLoaded(false),
     w_RasterMapMainInterface(NULL)/*,
     m_MyObject(new MyObject)*/
{
/*
// проба QWebKit
m_WebView = new QWebView;
m_WebFrame = m_WebView->page()->currentFrame();
m_MyObject->setWebFrame(m_WebFrame);
QNMASSERT(connect(m_WebFrame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(populate()));
m_WebFrame->setHtml(QString(
   "<head>"
   "<script src=\"http://api-maps.yandex.ru/2.0/?lang=ru-RU&load=package.route\" type=\"text/javascript\">"
   "</script>"
   "<script>"
   "ymaps.ready(init);"
   "function init () {myObject.YMapInitialized();}"
   "</script>"
   "</head>"
   "<body>"
   "</body>"));
*/

// определяем путь к плагинам
QString PluginsPath = QApplication::applicationDirPath();
QStringList Arguments(QCoreApplication::instance()->arguments());
// If the parameters was set right the number of them is greater than 1.
if(Arguments.size() > 1) {
	QStringList::const_iterator PreEnd = Arguments.end() - 1;
	for(QStringList::const_iterator it = Arguments.begin(); it != PreEnd; ++it) {
		if(*it == QLatin1String("-p")) {
			PluginsPath = *(++it);
			break;
			}
		}
	}

m_Flag.loadFromData(pointflag, sizeof(pointflag));

m_Point1 = QPointF();
m_Point2 = QPointF();

// создаем и размещаем виджет
w_MapWidget = new qnetmap::TMapWidget(PluginsPath, "", 13, QPointF(37.61858, 55.7530),
                                      Consts::Organization, Consts::Application);
// получаем список интерфейсов
m_MapInterfaces = w_MapWidget->mapInterfaces();
// открываем первую карту в списке
foreach(IMapAdapterInterface* Interface, m_MapInterfaces) {
   if(Interface->sourceType() == TMapAdapter::Consts::InternetMap) {
      w_MapWidget->showMapID(Interface->UUID());
      break;
      }
   }

// Проверка загрузки плагинов
if(!m_MapInterfaces.empty()) {
   createActions();
   createMenus();

   QVBoxLayout* layout = new QVBoxLayout;
   layout->addWidget(w_MapWidget);
   layout->setMargin(0);

   QWidget* w = new QWidget();
   w->setLayout(layout);
   setCentralWidget(w);
   // Ловим сообщения виджета карты
   QNM_DEBUG_CHECK(connect(w_MapWidget, SIGNAL(rasterMapsListChanged()), 
                  this,          SLOT(rasterMapsListChanged())));
   /*QNMASSERT(connect(m_MapWidget, SIGNAL(viewChanged(QString, int, QPointF)), 
           this,          SLOT(viewChanged(QString, int, QPointF)));
   QNMASSERT(connect(m_MapWidget, SIGNAL(showsMapID(QString)), 
           this,          SLOT(showsMapID(QString)));
   QNMASSERT(connect(m_MapWidget, SIGNAL(changedVisibility(QString, bool)), 
           this,          SLOT(changedVisibility(QString, bool)));*/
   //
   m_PluginsAreLoaded = true;
   m_IgnoreClicks = false;
   }
else {
	QString ProgramFileName(QFileInfo(QCoreApplication::applicationFilePath()).baseName());
   QMessageBox::critical(NULL, MapsViewerTranslator::tr("Error"), 
		// Russian: Подключаемые модули картографических сервисов не найдены в каталоге '%1'. 
		// Задайте путь к каталогу подключаемых модулей в командной строке с помощью ключа '-p'. 
		// Например, %2 -p PLUGINS_DIRECTORY.
		// См. справку выводимую командой '%2 -h'.
		MapsViewerTranslator::tr("The plugins for the map providers were not found "
			"in the folder '%1'.Specify the path to the plugins folder in the program's "
			"command line with the '-p' option.\n\n"
			"Ex. %2 -p PLUGINS_FOLDER\n\n"
			"See the help for the details (%2 -h).")
			.arg(QDir::toNativeSeparators(PluginsPath)).arg(ProgramFileName),
      MapsViewerTranslator::tr("Finish"));
   }
   
connect(w_MapWidget, SIGNAL(rubberRectSelected(const QRect, int)), this,
	SLOT(rubberBandHasFinishedSelection(const QRect, int)));   
   
// In order to create the status bar.
statusBar()->showMessage(QString());

}
//$(BIN_DIR)\qnetmap\plugins\$(PlatformName)\$(ConfigurationName)

//------------------------------------------------------------------
TMap::~TMap()
{
   foreach(QAction* Action, m_OverlayMapActions) delete Action;
   foreach(QAction* Action, m_BaseMapActions)    delete Action;
   foreach(QAction* Action, m_RasterMapActions)  delete Action;
   delete w_MapWidget;
   delete w_MapProviderGroup;
   foreach(TLayer*  Layer,  m_OverlayLayers)     delete Layer;

   /*
   delete m_WebView;
   delete m_MyObject;
   */
}

//------------------------------------------------------------------
QPixmap LoadPixmapFromData(const uchar *Data_, uint Len_)
{
QPixmap Icon;
Icon.loadFromData(Data_, Len_);
return Icon;
}

//------------------------------------------------------------------
void TMap::createActions(void)
{
w_ToolsCenterMap = new QAction(MapsViewerTranslator::tr("Go to the center of the map"
	/* Russian: Перейти в центр карты */), this);
QNM_DEBUG_CHECK(connect(w_ToolsCenterMap, SIGNAL(triggered(bool)), this, SLOT(centerMap())));

w_ToolsDistance = new QAction(MapsViewerTranslator::tr("Calculate distance" 
	/* Russian: Определить расстояние */), this);
QNM_DEBUG_CHECK(connect(w_ToolsDistance, SIGNAL(triggered(bool)), this, SLOT(calcDistance())));

w_ToolsPrint = new QAction(MapsViewerTranslator::tr("Print map..."
	/* Russian: Печать карты... */), this);
QNM_DEBUG_CHECK(connect(w_ToolsPrint, SIGNAL(triggered(bool)), this, SLOT(print())));

w_ToolsPrintPreview = new QAction(MapsViewerTranslator::tr("Print preview..."
	/* Russian: Печать с просмотром */), this);
QNM_DEBUG_CHECK(connect(w_ToolsPrintPreview, SIGNAL(triggered(bool)), this, SLOT(printPreview())));

w_ToolsPrintArea = new QAction(MapsViewerTranslator::tr("Print selected area..."
	/* Russian: Печать выбранной области */), this);
w_ToolsPrintArea->setIcon(LoadPixmapFromData(print_map_16x16, sizeof(print_map_16x16)));
connect(w_ToolsPrintArea, SIGNAL(triggered(bool)), this, SLOT(printArea()));

w_ToolsPrintAreaPreview = new QAction(
	MapsViewerTranslator::tr("Print selected area with preview..."), this);
w_ToolsPrintAreaPreview->setIcon(LoadPixmapFromData(view_and_print_map16, 
	sizeof(view_and_print_map16)));
connect(w_ToolsPrintAreaPreview, SIGNAL(triggered(bool)), this, SLOT(printAreaPreview()));

w_ToolsLoadArea = new QAction(MapsViewerTranslator::tr(
	"Create jobs to download the selected area..."
	/* Russian: Создать задания для загрузки выбранной области карты... */), this);
w_ToolsLoadArea->setIcon(LoadPixmapFromData(create_task16x16, sizeof(create_task16x16)));
connect(w_ToolsLoadArea, SIGNAL(triggered(bool)), this, SLOT(loadArea()));

w_ToolsRasterMaps = new QAction(MapsViewerTranslator::tr("Raster maps..."
	/* Russian: Растровые карты... */), this);
connect(w_ToolsRasterMaps, SIGNAL(triggered(bool)), this, SLOT(rasterMapsDialog()));

w_ToolsConfiguration = new QAction(MapsViewerTranslator::tr("Settings"), this);
w_ToolsConfiguration->setIcon(LoadPixmapFromData(properties16x16, sizeof(properties16x16)));
connect(w_ToolsConfiguration, SIGNAL(triggered(bool)), this, SLOT(configuration()));

m_ExitAction = new QAction(MapsViewerTranslator::tr("Exit"), this);
connect(m_ExitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

m_AboutAction = new QAction(MapsViewerTranslator::tr("About") + "...", this);
m_AboutAction->setIcon(LoadPixmapFromData(about16x16, sizeof(about16x16)));
connect(m_AboutAction, SIGNAL(triggered(bool)), this, SLOT(aboutDialog()));

w_MapProviderGroup = new QActionGroup(this);
connect(w_MapProviderGroup, SIGNAL(triggered(QAction*)), this, 
	SLOT(mapProviderSelected(QAction*)));

m_ShowGPXAction = new QAction(MapsViewerTranslator::tr("Load GPX..."
	/* Russian: Загрузить GPX... */), this);
QNM_DEBUG_CHECK(connect(m_ShowGPXAction, SIGNAL(triggered(bool)), this, SLOT(loadGPX())));
}

//------------------------------------------------------------------
void TMap::createMenus(void)
{
w_ToolsMenu = menuBar()->addMenu(MapsViewerTranslator::tr("&Tools"));

w_ToolsMenu->addAction(w_ToolsLoadArea);

#ifndef _BUILD_SPIDER_TASK_CREATOR
	w_ToolsMenu->addAction(w_ToolsCenterMap);
	w_ToolsMenu->addAction(w_ToolsDistance);
	w_ToolsMenu->addAction(m_ShowGPXAction);
#endif

w_ToolsMenu->addSeparator();

#ifndef _BUILD_SPIDER_TASK_CREATOR
	w_ToolsMenu->addAction(w_ToolsPrint);
	w_ToolsMenu->addAction(w_ToolsPrintPreview);
#endif

w_ToolsMenu->addAction(w_ToolsPrintArea);
w_ToolsMenu->addAction(w_ToolsPrintAreaPreview);

#ifndef _BUILD_SPIDER_TASK_CREATOR
	w_ToolsMenu->addSeparator();
	w_ToolsMenu->addAction(w_ToolsRasterMaps);
#endif

w_ToolsMenu->addSeparator();
w_ToolsMenu->addAction(w_ToolsConfiguration);

w_ToolsMenu->addSeparator();
w_ToolsMenu->addAction(m_ExitAction);

w_MapPluginsMenu = menuBar()->addMenu(MapsViewerTranslator::tr("&Maps"));

//
if(!m_MapInterfaces.empty()) {
   // формируем меню карт
   bool Checked = true;
   foreach(qnetmap::IMapAdapterInterface* MapInterface, m_MapInterfaces) {
      switch(MapInterface->sourceType()) {
         case qnetmap::TMapAdapter::Consts::InternetMap:
            if(MapInterface->isInitialized()) populateMenus(MapInterface, Checked);
            if(Checked) Checked = false;
            break;
         }
      }
   // добавляем в меню растровые карты
   if(w_MapWidget->rasterMapInterface()) {
      qnetmap::IMapAdapterInterfaces RasterMapInterfaces = w_MapWidget->rasterMapInterfaces();
      if(RasterMapInterfaces.size()) {
         w_MapPluginsMenu->addSeparator();
         foreach(qnetmap::IMapAdapterInterface* Interface, RasterMapInterfaces) {
            populateMenus(Interface, false);
            }
         }
      }
   // добавляем в меню прозрачные карты
   if(m_OverlayMapActions.size()) {
      w_MapPluginsMenu->addSeparator();
      foreach(QAction* PluginAction, m_OverlayMapActions) w_MapPluginsMenu->addAction(PluginAction);
      }
   }
QMenu *HelpMenu = menuBar()->addMenu(MapsViewerTranslator::tr("?"));
HelpMenu->addAction(m_AboutAction);
}

//------------------------------------------------------------------
void TMap::centerMap()
{
w_MapWidget->setView(w_MapWidget->baseMapAdapter()->centerMap());
}

//------------------------------------------------------------------
void TMap::print(void)
{
w_MapWidget->print();
}

//------------------------------------------------------------------
void TMap::printPreview(void)
{
w_MapWidget->printPreview();
}

//------------------------------------------------------------------
void TMap::printArea(void)
{
statusBar()->showMessage(areaSelectionHelpMessage());
w_MapWidget->printArea();
}

//------------------------------------------------------------------
void TMap::printAreaPreview()
{
statusBar()->showMessage(areaSelectionHelpMessage());
w_MapWidget->printAreaPreview();
}

//------------------------------------------------------------------
void TMap::printMap(QPrinter *Printer_)
{
w_MapWidget->printMap(Printer_);
}

//------------------------------------------------------------------
void TMap::rasterMapsDialog(void)
{
w_MapWidget->rasterMapsDialog();
}
void TMap::rubberBandHasFinishedSelection(const QRect, int)
{
statusBar()->showMessage(QString());
}

//------------------------------------------------------------------
void TMap::populateMenus(qnetmap::IMapAdapterInterface* MapInterface_, bool Checked_)
   {
   if(MapInterface_) {
      QAction* PluginAction;
      switch(MapInterface_->sourceType()) {
   case qnetmap::TMapAdapter::Consts::EmptyMap:
      break;
   case qnetmap::TMapAdapter::Consts::InternetMap: 
      if(MapInterface_->isTransparent()) {
         // прозрачные карты, накладываемые на основные, непрозрачные
         PluginAction = new QAction(MapInterface_->pluginName(), MapInterface_->plugin());
         m_OverlayMapActions.push_back(PluginAction);
         PluginAction->setCheckable(true);
         PluginAction->setEnabled(true);
         QNM_DEBUG_CHECK(connect(PluginAction, SIGNAL(toggled(bool)), this, SLOT(changeVisibility(bool))));
         }
      else {
         // непрозрачные карты
         // включаем в меню в группу выбора карты
         PluginAction = new QAction(MapInterface_->pluginName(), MapInterface_->plugin());
         m_BaseMapActions.push_back(PluginAction);
         PluginAction->setCheckable(true);
         PluginAction->setChecked(Checked_);
         w_MapProviderGroup->addAction(PluginAction);
         w_MapPluginsMenu->addAction(PluginAction);
         }
      break;
   case qnetmap::TMapAdapter::Consts::RasterMap:
      // растровые карты
      PluginAction = new QAction(MapInterface_->pluginName(), MapInterface_->plugin()); 
      m_RasterMapActions.push_back(PluginAction);
      PluginAction->setCheckable(MapInterface_->isGeocoded());
      PluginAction->setChecked(Checked_);
      PluginAction->setEnabled(MapInterface_->isGeocoded());
      w_MapProviderGroup->addAction(PluginAction);
      w_MapPluginsMenu->addAction(PluginAction);
      break;
   default:
      assert(!"Такие карты неизвестны");
      break;
         }
      }
   }

//------------------------------------------------------------------
void TMap::loadArea(void)
{
statusBar()->showMessage(areaSelectionHelpMessage());
w_MapWidget->chooseLoadingArea();
}

//------------------------------------------------------------------
void TMap::calcDistance()
{
m_IgnoreClicks = true;
w_MapWidget->saveMouseMode();
w_MapWidget->setCursor(QCursor(Qt::CrossCursor));
QNM_DEBUG_CHECK(connect(w_MapWidget, SIGNAL(mouseEventCoordinate(const QMouseEvent*, const QPointF)),
		         this,          SLOT(calcDistanceSelected(const QMouseEvent*, const QPointF))));
}

//------------------------------------------------------------------
void TMap::calcDistanceSelected(const QMouseEvent* Event_, const QPointF Point_)
{
if(m_Point1 == QPointF() && Event_->type() == QEvent::MouseButtonPress) {
   // первый клик
	m_Point1 = Point_;
	w_MapWidget->geometryLayer()->
      addGeometry(new TImagePoint(m_Point1, &m_Flag, "", qnetmap::BottomRight));
	w_MapWidget->forceUpdate();
   }
else if(m_Point2 == QPointF() && Event_->type() == QEvent::MouseButtonPress) {
   // второй клик
	disconnect(w_MapWidget, SIGNAL(mouseEventCoordinate(const QMouseEvent*, const QPointF)),
				  this,          SLOT(calcDistanceSelected(const QMouseEvent*, const QPointF)));

   m_Point2 = Point_;
	w_MapWidget->geometryLayer()->
      addGeometry(new TImagePoint(m_Point2, &m_Flag, "", qnetmap::BottomRight));
   // вычисляем расстояние
   double PI = acos(-1.0);
	double a1 = m_Point1.x() * (PI / 180.0);
	double b1 = m_Point1.y() * (PI / 180.0);
	double a2 = m_Point2.x() * (PI / 180.0);
	double b2 = m_Point2.y() * (PI / 180.0);
   double r = QNetMapConsts::GoogleMapsSphereRadius / 1000.; //(в километрах)

	double Distance = acos(cos(a1) * cos(b1) * cos(a2) * cos(b2) + 
                          cos(a1) * sin(b1) * cos(a2) * sin(b2) + 
                          sin(a1) * sin(a2)) * r;

   // отрисовываем
	TPointList Points;
	Points.append(new TPoint(m_Point1));
	QPixmap* PixMap = new QPixmap(100, 20);
	PixMap->fill(Qt::transparent);
	QPainter Painter(PixMap);
	Painter.setFont(QFont("Helvetica", 7));
	Painter.drawText(PixMap->rect(), QString().setNum(Distance, 'f', 3) + " km");
	Painter.end();
	Points.append(new TPoint(m_Point2, PixMap, "", qnetmap::BottomLeft));
	w_MapWidget->geometryLayer()->addGeometry(new TLineString(Points));
	w_MapWidget->forceUpdate();
	m_Point1 = QPointF();
	m_Point2 = QPointF();
	m_IgnoreClicks = false;
   //
   w_MapWidget->restoreMouseMode();
   }
}

//------------------------------------------------------------------
void TMap::mapProviderSelected(QAction* Action_)
{
QList<QAction*>::const_iterator it =
   std::find(m_BaseMapActions.begin(), m_BaseMapActions.end(), Action_);
if(it != m_BaseMapActions.end()) {
   // основная непрозрачная карта
   qnetmap::IMapAdapterInterface *MapInterface = 
      qobject_cast<qnetmap::IMapAdapterInterface *>(Action_->parent());
   w_MapWidget->showMapID(MapInterface->UUID());
   }
else {
   QList<QAction*>::const_iterator it =
      std::find(m_RasterMapActions.begin(), m_RasterMapActions.end(), Action_);
   if(it != m_RasterMapActions.end()) {
      // растровая карта
      qnetmap::IMapAdapterInterface *MapInterface = 
         qobject_cast<qnetmap::IMapAdapterInterface *>(Action_->parent());
      w_MapWidget->showMapID(MapInterface->UUID());
      }
   }
}

//------------------------------------------------------------------
void TMap::resizeEvent(QResizeEvent *event)
{
QWidget::resizeEvent(event);
}

//------------------------------------------------------------------
void TMap::configuration(void)
{
TMapConfiguration ConfigurationDialog(this);
ConfigurationDialog.exec();
}

//------------------------------------------------------------------
void TMap::setMapsConfiguration(QString MapsStorageDirectory_, int MapsStorageType_)
{
qnetmap::IMapAdapterInterface* MapInterface;
foreach(MapInterface, m_MapInterfaces) {
   MapInterface->setMappingStoragePath(MapsStorageDirectory_, true);
   MapInterface->setStorageType(MapsStorageType_);
   }
}

// -----------------------------------------------------------------------
void TMap::changeVisibility(bool Visibility_)
{
QAction* Action = qobject_cast<QAction*>(QObject::sender());
qnetmap::IMapAdapterInterface *MapInterface =
   qobject_cast<qnetmap::IMapAdapterInterface *>(Action->parent());
w_MapWidget->setVisibleMapID(MapInterface->UUID(), Visibility_);
}

// -----------------------------------------------------------------------
void TMap::rasterMapsListChanged(void)
{
}

/*
// -----------------------------------------------------------------------
void TMap::viewChanged(QString MapID_, int Zoom_, QPointF Coordinate_) 
{ 
Q_UNUSED(MapID_);
Q_UNUSED(Zoom_);
Q_UNUSED(Coordinate_);
}

// -----------------------------------------------------------------------
void TMap::showsMapID(QString MapID_) 
{ 
Q_UNUSED(MapID_);
}

// -----------------------------------------------------------------------
void TMap::changedVisibility(QString MapID_, bool Visibility_)
{ 
Q_UNUSED(MapID_);
Q_UNUSED(Visibility_);
}
*/

/*
// -----------------------------------------------------------------------
void TMap::populate(void)
{
m_WebFrame->addToJavaScriptWindowObject("myObject", m_MyObject);
}

// -----------------------------------------------------------------------
void MyObject::YMapInitialized(void) const
{
m_WebFrame->evaluateJavaScript(
      "ymaps.route([{ type: 'wayPoint', point: [55.751378, 37.611845] },"
                   "{ type: 'wayPoint', point: [55.811511, 37.312518] }"
      "]).then(function (route) {"
         "myObject.print(route.getHumanLength());"
         "}, function (error) { myObject.print(\"Возникла ошибка: \" + error.message);});");
}
*/

// -----------------------------------------------------------------------
QString TMap::programTitle(void)
{
#ifndef _BUILD_SPIDER_TASK_CREATOR
	return MapsViewerTranslator::tr("Maps viewer");
#else
	// Russian: Просмотр карт и выбор области для загрузки
	return MapsViewerTranslator::tr("Maps viewer and download job creator");
#endif
}

// -----------------------------------------------------------------------
QString TMap::programVersion(void)
{
return MapsViewerTranslator::tr("version") + ' ' + qnetmap::VersionString() + " (" +
	qnetmap::VersionHash() + ')';
}

// -----------------------------------------------------------------------
QString TMap::areaSelectionHelpMessage(void)
{
// Russian: Удерживайте нажатой левую кнопку мыши, чтобы выбрать область карты.
return MapsViewerTranslator::tr("Hold down the left mouse button to select area pf the map.");
}

// -----------------------------------------------------------------------
const char *ThirdPartyCopyrights = 
"\n"
"- QMapControl library (http://qmapcontrol.sourceforge.net/)\n"
"  by Kai Winter. All rights reserved.\n"
"- FatCow-Farm Fresh Icons (http://www.fatcow.com/free-icons)\n"
"  Copyright 2009-2013 FatCow Web Hosting.\n"
"  All rights reserved.\n"
;

// -----------------------------------------------------------------------
void TMap::aboutDialog(void)
{
class TAboutDialog: public QDialog {
public:
	TAboutDialog(void): QDialog(NULL, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint) {
		setWindowTitle(MapsViewerTranslator::tr("About"));
		//
        QPalette Palette(palette());
        Palette.setColor(backgroundRole(), Qt::white);
        setPalette(Palette);
		setAutoFillBackground(true);
		//
		QVBoxLayout *Layout = new QVBoxLayout(this);
		//
		QLabel *Label = new QLabel(programTitle());
		QFont Font(Label->font());
		Font.setPointSize(11);
		Font.setBold(true);
		Label->setFont(Font);
		//
		QFontMetrics Metrics(Font);
		int Width = Metrics.width(Label->text());
		if(Width < 360 - 20) setFixedWidth(360);
		
		//
		Layout->addWidget(Label, 0, Qt::AlignCenter);
		Layout->addWidget(new QLabel(programVersion()), 0, Qt::AlignCenter);
		//
		Layout->addSpacing(8);
		
		// Russian: Это программное обеспечение является частью  
		// <a href="http://sourceforge.net/projects/qnetmap">
		// библиотеки QNetMap</a>. 
		// Copyright (c) 2011-2014 JSC Sintels 
		// (<a href="http://sintels.ru">http://sintels.ru</a>) and/or its affiliates.
		// Copyright (c) 2012-2014 OJSC T8 
		// (<a href="http://t8.ru">http://t8.ru</a>) and/or its affiliates.");
		QString Caption = MapsViewerTranslator::tr(
			"This software is a part of the  "
			"<a href=\"http://sourceforge.net/projects/qnetmap\">"
			"QNetMap library</a>. "
			"Copyright (c) 2011-2015 JSC Sintels "
			"(<a href=\"http://sintels.ru\">http://sintels.ru</a>) and/or its affiliates."
			"Copyright (c) 2012-2014 OJSC T8 "
			"(<a href=\"http://t8.ru\">http://t8.ru</a>) and/or its affiliates.");
		Label = new QLabel(Caption);
		Label->setOpenExternalLinks(true);
		Label->setWordWrap(true);
		Label->setTextInteractionFlags(Label->textInteractionFlags() | Qt::LinksAccessibleByMouse | 
			Qt::LinksAccessibleByKeyboard);
		Layout->addWidget(Label);
		
		Layout->addSpacing(8);
		// Russian: Уведомления и условия третьих сторон, связанные со сторонним программным 
		// обеспечением:
		Caption = MapsViewerTranslator::tr(
			"Third party software notices and/or additional terms and conditions:") + 
			ThirdPartyCopyrights;
		Label = new QLabel(Caption);
		Label->setWordWrap(true);
		Layout->addWidget(Label);
		
		Layout->addSpacing(8);
		//
		QPushButton *Button = new QPushButton(MapsViewerTranslator::tr("OK"));
		Layout->addWidget(Button, 0, Qt::AlignCenter);
		connect(Button, SIGNAL(clicked()), this, SLOT(accept()));
		}
	};

// ----
TAboutDialog().exec();
}

// -----------------------------------------------------------------------
void TMap::loadGPX(void)
{
qnetmap::TLayer *GeometryLayer = w_MapWidget->geometryLayer();
if(!GeometryLayer) {
	assert(!"loadGPX:: Layer 'Geometry Layer' was not defined.");
	return;
   }
   
QSettings Settings(Consts::Organization, Consts::Application);   
QString Folder = Settings.value(Consts::LastGPXFileFolder).toString();   
//   
QString GPXFileName = QFileDialog::getOpenFileName(NULL, 
	MapsViewerTranslator::tr("GPX file to load"), Folder, 
	MapsViewerTranslator::tr("GPX file (*.gpx)"));
if(GPXFileName.isEmpty()) return;
//
QString ErrorMessage;
QRectF BoundingBox;
if(!AddGPX(GeometryLayer, GPXFileName, BoundingBox, ErrorMessage)) {
	QMessageBox::critical(NULL, programTitle(), ErrorMessage);
	return;
	}
Settings.setValue(Consts::LastGPXFileFolder, QFileInfo(GPXFileName).absolutePath());
setView(BoundingBox);
}

// -----------------------------------------------------------------------
void TMap::setView(const QRectF &Rect_)
{
// Get the scale so the Rect_ fits the window.
QSize Size = w_MapWidget->size();
qreal dx = Rect_.width() / 20;
qreal dy = Rect_.height() / 20;
QRectF AdjustedRect(Rect_);
AdjustedRect.adjust(-dx, -dy, dx, dy);
//		
int Zoom = w_MapWidget->calculateZoom(AdjustedRect, Size.width(), Size.height());
w_MapWidget->setZoom(Zoom);
w_MapWidget->setView(AdjustedRect.center());
}
