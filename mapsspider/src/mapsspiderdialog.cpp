/*
* This file is part of MapsSpider,
* an open-source cross-platform software for downloading maps, 
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
* along with MapsSpider. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>

#include "mapsspiderdialog.h"
#include "qnetmap_httpparameters.h"
#include "qnetmap_mapnetwork.h"
#include "mapsspider_translator.h"

#include <qnetmap_version.h>

//! \var список загружаемых карт
std::vector<TMapParameters> g_Maps;
//! \var количество одновременных запросов
int g_SimultaneousRequestsCount;

//------------------------------------------------------------------
TMapsSpiderDialog::TMapsSpiderDialog(bool ShowTrayIcon_):
	#ifdef NOTRAYICON
		QDialog(NULL, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
	#endif
   #ifdef QNETMAP_QT_4
      w_ProgressBarStyle(new QPlastiqueStyle()),
   #else
      w_ProgressBarStyle(QStyleFactory::create("plastique")),
   #endif
      m_SystemTrayAvailable(QSystemTrayIcon::isSystemTrayAvailable()),
      m_ShowTrayIcon(ShowTrayIcon_),
      m_ErrorMessage(QString()),
      m_TimerID(0),
      m_IsLoadingMaps(false),
      m_IsErrorStartLoadingMap(false),
      m_SpeedTilesPerHour(0),
      m_TilesCount(0),
      m_LoadedTilesCount(0),
      m_AtomicLoadedTilesCount(0),
      m_NotLoadedTilesCount(0),
      m_OldLoadedTilesCount(0),
      m_OldSecondsLeft(0)
{
g_SimultaneousRequestsCount = 50;
QSettings Settings(qnetmap::QNetMapConsts::Organization, Consts::Application);

setWindowTitle(programTitle() + " (" + programVersion() + ")");
resize(600, 175);

// создаем основную страницу окна приложения
w_FileBrowseButton = createButton(MapsSpiderTranslator::tr("Select..." 
	/* Russian: Выбрать... */), this, SLOT(browseFile()));
w_DirectoryBrowseButton = createButton(w_FileBrowseButton->text(), this, 
	SLOT(browseDirectory()));
	
// Russian: Загрузить
w_StartButton = createButton(MapsSpiderTranslator::tr("Load"), this, SLOT(start()));

w_StartButton->setDefault(true);
w_StartButton->setEnabled(false);
// Russian: Выйти
w_ExitButton = createButton(MapsSpiderTranslator::tr("Exit"), qApp, SLOT(quit()));
// Russian: Остановить
w_StopButton = createButton(MapsSpiderTranslator::tr("Stop"), this, SLOT(stop()));


w_FileComboBox      = createComboBox("XMLFileName", 10);
w_DirectoryComboBox = createComboBox("MapsSavePath", 10);
// Russian: Файл задания
w_FileLabel      = new QLabel(MapsSpiderTranslator::tr("Job file"));
// Russian: Каталог для карт
w_DirectoryLabel = new QLabel(MapsSpiderTranslator::tr("Maps folder"));


QHBoxLayout *buttonsLayout = new QHBoxLayout;
buttonsLayout->addStretch();
buttonsLayout->addWidget(w_StopButton);
buttonsLayout->addWidget(w_StartButton);
buttonsLayout->addWidget(w_ExitButton);

//
QGridLayout *MainLayout = new QGridLayout;
MainLayout->setColumnStretch(1, 1);
MainLayout->setRowMinimumHeight(2, 10);
MainLayout->setRowMinimumHeight(5, 10);
/// строка выбора файла задания
MainLayout->addWidget(w_FileLabel, 0, 0);
MainLayout->addWidget(w_FileComboBox, 0, 1, 1, 2);
MainLayout->addWidget(w_FileBrowseButton, 0, 3);
/// строка выбора папки для сохранения тайлов
MainLayout->addWidget(w_DirectoryLabel, 1, 0);
MainLayout->addWidget(w_DirectoryComboBox, 1, 1, 1, 2);
MainLayout->addWidget(w_DirectoryBrowseButton, 1, 3);
/// строка с сообщением о количестве загруженных тайлов и оставшемся времени
w_ProgressTiles = new QLabel();
MainLayout->addWidget(w_ProgressTiles, 3, 0, 1, 2);
w_TimeLeft = new QLabel();
MainLayout->addWidget(w_TimeLeft, 3, 2, 1, 2);
/// прогресс-бар
w_ProgressBar = new QProgressBar();
w_ProgressBar->setStyle(w_ProgressBarStyle);
MainLayout->addWidget(w_ProgressBar, 4, 0, 1, 4);
/// линия
QFrame* HLine = new QFrame(this);
HLine->setFrameShape(QFrame::HLine);
HLine->setFrameShadow(QFrame::Sunken);
QVBoxLayout *HLineLayout = new QVBoxLayout;
HLineLayout->addStretch();
HLineLayout->addWidget(HLine);
MainLayout->addLayout(HLineLayout, 5, 0, 1, 4);
/// строка с кнопками подтверждения и выхода
MainLayout->addLayout(buttonsLayout, 6, 0, 1, 4);
///
setLayout(MainLayout);

// прочие установки
setWidgetsVisibility(1);
setStartButtonEnabled("");
createActions();

// сигналы
QNM_DEBUG_CHECK(connect(w_FileComboBox,      SIGNAL(editTextChanged(QString)), this, SLOT(setStartButtonEnabled(QString))));
QNM_DEBUG_CHECK(connect(w_DirectoryComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(setStartButtonEnabled(QString))));

if(m_SystemTrayAvailable && m_ShowTrayIcon) {
   createTrayIcon();
   setIcon();

   // сигналы
   QNM_DEBUG_CHECK(connect(w_TrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked())));
   QNM_DEBUG_CHECK(connect(w_TrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                  this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason))));

   // показываем иконку в трее
   w_TrayIcon->show();
   }
}

//------------------------------------------------------------------
TMapsSpiderDialog::~TMapsSpiderDialog() 
{ 
// сохраняем необходимые значения в реестре при выходе из программы
saveComboBox(w_FileComboBox,      "XMLFileName", 10);
saveComboBox(w_DirectoryComboBox, "MapsSavePath", 10);

delete w_ProgressBarStyle;
}

//------------------------------------------------------------------
void TMapsSpiderDialog::setVisible(bool visible)
{
w_RestoreAction->setEnabled(isMaximized() || !visible);
QDialog::setVisible(visible);
}

//------------------------------------------------------------------
void TMapsSpiderDialog::closeEvent(QCloseEvent *Event_)
{
if(m_SystemTrayAvailable && m_ShowTrayIcon) hide();
Event_->ignore();
}

//------------------------------------------------------------------
void TMapsSpiderDialog::setIcon(void)
{
static int i = 0;

static QString ToolTip1 = MapsSpiderTranslator::tr(
	"Maps Spider:\nOverall tiles - %1,\n"
	"Tiles loaded - %2 (%3 %),\n"
	"Errors - %4 (%5 %)\n"
	"Time left - %6"
	/* Russian: Maps Spider:\nОбщее количество тайлов - %1,\n
	   Загружено тайлов - %2 (%3 %),\n
		Запросов с ошибками - %4 (%5 %)\n
		Осталось времени - %6
	*/
	);

static QString ToolTip2 = MapsSpiderTranslator::tr(
	"Maps Spider:\nOverall tiles - %1,\n"
	"Tiles loaded - %2 (%3 %),\n"
	"Errors - %4 (%5 %)\n"
	/* Russian: Maps Spider:\nОбщее количество тайлов - %1,\n
   Загружено тайлов - %2 (%3 %),\n
   Запросов с ошибками - %4 (%5 %)\n
	*/
	);

// Russian: Загружено %1 тайлов из %2 (%3 ошибок)
static QString ProgressTiles = MapsSpiderTranslator::tr("Loaded %1 tiles of %2 (%3 errors)");


unsigned long long LoadedTilesCount = m_LoadedTilesCount + m_AtomicLoadedTilesCount;
//
if(m_IsLoadingMaps && m_TilesCount > LoadedTilesCount) {
   // идет процесс загрузки тайлов
   w_ProgressTiles->setText(
      ProgressTiles.arg(LoadedTilesCount).arg(m_TilesCount).arg(m_NotLoadedTilesCount));
   // Russian: Осталось времени
   w_TimeLeft->setText(MapsSpiderTranslator::tr("Time left") + " - " + timeLeft());

   w_ProgressBar->setValue(LoadedTilesCount);
   // трей
   if(m_SystemTrayAvailable && m_ShowTrayIcon) {
      i =  i < 10 ? i + 1 : 1;
      QString IconName = ":/globe32_%1.png";
      QIcon icon(IconName.arg(i));
      w_TrayIcon->setIcon(icon);
      w_TrayIcon->setToolTip(ToolTip1
         .arg(m_TilesCount)
         .arg(LoadedTilesCount)
         .arg((LoadedTilesCount * 100) / m_TilesCount)
         .arg(m_NotLoadedTilesCount)
         .arg((m_NotLoadedTilesCount * 100) / m_TilesCount)
         .arg(timeLeft()));
      }
   }
else if(m_IsErrorStartLoadingMap) {
   // ошибка при подготовке старта загрузки
   if(m_SystemTrayAvailable && m_ShowTrayIcon) {
      QIcon icon(":/globe32_error.png");
      w_TrayIcon->setIcon(icon);
      if(!m_ErrorMessage.isEmpty()) w_TrayIcon->setToolTip(m_ErrorMessage);
      }
   }
else if(i > 0) {
   // процесс загрузки завершен
   w_ProgressBar->setValue(LoadedTilesCount);
   if(m_SystemTrayAvailable && m_ShowTrayIcon) {
      QIcon icon(":/globe32_accept.png");
      w_TrayIcon->setIcon(icon);
      w_TrayIcon->setToolTip(ToolTip2
         .arg(m_TilesCount)
         .arg(LoadedTilesCount)
         .arg((LoadedTilesCount * 100) / m_TilesCount)
         .arg(m_NotLoadedTilesCount)
         .arg((m_NotLoadedTilesCount * 100) / m_TilesCount));
      }
   }
else {
   // Загрузка еще не начиналась
   if(m_SystemTrayAvailable && m_ShowTrayIcon) {
      QIcon icon(":/globe32.png");
      w_TrayIcon->setIcon(icon);
      setWindowIcon(icon);
      w_TrayIcon->setToolTip(programTitle());
      }
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::trayIconActivated(QSystemTrayIcon::ActivationReason Reason_)
{
switch (Reason_) {
case QSystemTrayIcon::Trigger:
   // щелкнули левой кнопкой по иконке
case QSystemTrayIcon::DoubleClick:
   // щелкнули дважды левой кнопкой по иконке
   break;
case QSystemTrayIcon::MiddleClick:
   // щелкнули средней кнопкой по иконке
   break;
default:
   break;
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::createActions()
{
w_RestoreAction = new QAction(MapsSpiderTranslator::tr("Show"), this);
QNM_DEBUG_CHECK(connect(w_RestoreAction, SIGNAL(triggered()), this, SLOT(showNormal())));

w_StartAction = new QAction(MapsSpiderTranslator::tr("Start"), this);
QNM_DEBUG_CHECK(connect(w_StartAction, SIGNAL(triggered()), this, SLOT(start())));

w_StopAction = new QAction(MapsSpiderTranslator::tr("Stop"), this);
QNM_DEBUG_CHECK(connect(w_StopAction, SIGNAL(triggered()), this, SLOT(stop())));
w_StopAction->setEnabled(false);

w_QuitAction = new QAction(MapsSpiderTranslator::tr("Exit"), this);
QNM_DEBUG_CHECK(connect(w_QuitAction, SIGNAL(triggered()), qApp, SLOT(quit())));
}

//------------------------------------------------------------------
void TMapsSpiderDialog::createTrayIcon()
{
w_TrayIconMenu = new QMenu(this);
w_TrayIconMenu->addAction(w_RestoreAction);
w_TrayIconMenu->addSeparator();
w_TrayIconMenu->addAction(w_StartAction);
w_TrayIconMenu->addAction(w_StopAction);
w_TrayIconMenu->addSeparator();
w_TrayIconMenu->addAction(w_QuitAction);

w_TrayIcon = new QSystemTrayIcon(this);
w_TrayIcon->setContextMenu(w_TrayIconMenu);
}

//------------------------------------------------------------------
void TMapsSpiderDialog::browseDirectory()
{
QString Directory = QDir::toNativeSeparators(
	// Russian:	Выберите каталог
   QFileDialog::getExistingDirectory(this, MapsSpiderTranslator::tr("Select folder"), 
   QDir::currentPath()));

if (!Directory.isEmpty()) {
   if (w_DirectoryComboBox->findText(Directory) == -1)
      w_DirectoryComboBox->addItem(Directory);
   w_DirectoryComboBox->setCurrentIndex(w_DirectoryComboBox->findText(Directory));
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::browseFile()
{
QString File = QDir::toNativeSeparators(
	// Russian:	Выберите файл с заданием
   QFileDialog::getOpenFileName(this, MapsSpiderTranslator::tr("Select job file"), 
   QDir::currentPath(), "*.xml"));


if(!File.isEmpty()) {
   if(w_FileComboBox->findText(File) == -1) {
      w_FileComboBox->addItem(File);
      }
   w_FileComboBox->setCurrentIndex(w_FileComboBox->findText(File));
   }
}

//------------------------------------------------------------------
QPushButton* TMapsSpiderDialog::
   createButton(const QString &Text_, const QObject *Receiver_, const char *Member_)
{
QPushButton* button = new QPushButton(Text_);
QNM_DEBUG_CHECK(connect(button, SIGNAL(clicked()), Receiver_, Member_));
return button;
}

//------------------------------------------------------------------
void TMapsSpiderDialog::start(void)
{
m_TimeStart = QTime::currentTime();
// Russian:	Осталось времени - --:--:--
w_TimeLeft->setText(MapsSpiderTranslator::tr("Time left - --:--:-- ") + ' ');

w_StartAction->setEnabled(false);
w_StopAction->setEnabled(true);

TJobsXMLParser Handler;
QFile XMLFile(w_FileComboBox->currentText());
QXmlInputSource Source(&XMLFile);
QXmlSimpleReader Reader;
Reader.setContentHandler(&Handler);
Reader.setErrorHandler(&Handler); 
if(!Reader.parse(Source)) {
   // ошибка при подготовке старта загрузки
   QString Error = Handler.errorString();
   showErrorMessage(Error);
   m_IsErrorStartLoadingMap = true;
   setIcon();
   }
else {
   if(!loadPlugins()) {
      // число плагинов меньше необходимых
      // Russian: Необходимые плагины не найдены.
      QString Error = MapsSpiderTranslator::tr("The necessary plugins was not found.");

      showErrorMessage(Error);
      m_IsErrorStartLoadingMap = true;
      setIcon();
      }
   else {
      // изменяем видимость и доступность элементов окна
      setWidgetsVisibility(2);
      // запускаем процесс загрузки тайлов
      m_itCurrentMap = g_Maps.begin();
      startLoadTiles();
      }
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::stop(void)
{
// удаляем менеджеры загрузки
for(unsigned int i=1; i<=m_CountNetworkManagers; i++) {
   disconnect(m_NetworkManagers[i], SIGNAL(finished(QNetworkReply*)),
              this,                 SLOT(requestFinished(QNetworkReply*)));
   delete m_NetworkManagers[i];
   }
//
m_IsLoadingMaps = false;
w_StartAction->setEnabled(true);
w_StopAction->setEnabled(false);
// изменяем видимость и доступность элементов окна
setWidgetsVisibility(1);
//
killTimer(m_TimerID);
g_Maps.clear();
setIcon();
show();
}

//------------------------------------------------------------------
void TMapsSpiderDialog::messageClicked(void)
{
}

//------------------------------------------------------------------
void TMapsSpiderDialog::setStartButtonEnabled(QString)
{
QString File = w_FileComboBox->currentText();
QString Directory = w_DirectoryComboBox->currentText();
w_StartButton->setEnabled(!File.isEmpty() && !Directory.isEmpty() &&
                          QFile(File).exists() && QDir(Directory).exists());
}

//------------------------------------------------------------------
void TMapsSpiderDialog::showErrorMessage(QString &Message_)
{
if(isVisible()) QMessageBox::critical(this, MapsSpiderTranslator::tr("Error"), Message_);
else {
   QSystemTrayIcon::MessageIcon Icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Critical);
   w_TrayIcon->showMessage(MapsSpiderTranslator::tr("Error"), Message_, Icon, 10000);
   }
m_ErrorMessage = Message_;
}

//------------------------------------------------------------------
void TMapsSpiderDialog::showMessage(QString &Title_, QString &Message_)
{
QSystemTrayIcon::MessageIcon Icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
w_TrayIcon->showMessage(Title_, Message_, Icon, 20000);
}

//------------------------------------------------------------------
bool TMapsSpiderDialog::loadPlugins()
{
// загружаем плагины
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

if(PluginsPath.isEmpty()) {
   // ищем в папке программы
   m_PluginsDir = QDir(QApplication::applicationDirPath(), "qnetmap_*.dll");
   if(m_PluginsDir.entryList(QDir::Files).size() == 0) {
      // ищем в текущей папке
      m_PluginsDir = QDir(".", "qnetmap_*.dll");
      }
   }
else {
   // ищем в указанной папке
   m_PluginsDir = QDir(PluginsPath, "qnetmap_*.dll");
   }

// получаем список интерфейсов
qnetmap::IMapAdapterInterfaces MapInterfaces = qnetmap::TMapControl::loadPlugins(PluginsPath);
// выбираем нужные
foreach(qnetmap::IMapAdapterInterface* MapInterface, MapInterfaces) {
   if(saveMapInterface(MapInterface)) {
      MapInterface->setMappingStoragePath(w_DirectoryComboBox->currentText(), true); 
      }
   else delete MapInterface;
   }

// проверяем, все-ли необходимые плагины загружены и возвращаем результат
for(std::vector<TMapParameters>::const_iterator it = g_Maps.begin(); it != g_Maps.end(); ++it) {
   if(it->mapInterface() == NULL) return false;
   }

return true;
}

//------------------------------------------------------------------
bool TMapsSpiderDialog::saveMapInterface(qnetmap::IMapAdapterInterface* MapInterface_)
{
if(MapInterface_->sourceType() == qnetmap::TMapAdapter::Consts::InternetMap) {
   // ищем интерфейс плагина в списке
   QString PluginName = MapInterface_->pluginName();
   for(std::vector<TMapParameters>::iterator it = g_Maps.begin(); it != g_Maps.end(); ++it) {
      if(it->pluginName() == PluginName) {
         it->setMapInterface(MapInterface_);
         return true;
         }
      }
   }

return false;
}

//------------------------------------------------------------------
void TMapsSpiderDialog::startLoadTiles()
{
// создаем классы - менеджеры загрузки
m_CountNetworkManagers = (g_SimultaneousRequestsCount / 80) + 1;
for(unsigned int i=1; i<=m_CountNetworkManagers; ++i) {
   m_NetworkManagers[i] = new QNetworkAccessManager();
   QNM_DEBUG_CHECK(connect(m_NetworkManagers[i], SIGNAL(finished(QNetworkReply*)),
                  this,                 SLOT(requestFinished(QNetworkReply*))));
   }
//
// очередь запросов
m_NetworkRepliesMutex.lock();
m_NetworkReplies.clear();
m_NetworkRepliesMutex.unlock();
// обнуляем счетчики
m_TilesCount = 0;
m_LoadedTilesCount = 0;
m_AtomicLoadedTilesCount.fetchAndStoreOrdered(0);
m_NotLoadedTilesCount.fetchAndStoreOrdered(0);
m_OldSecondsLeft = 0;
m_OldLoadedTilesCount = 0;
m_SpeedTilesPerHour = 0;
// очищаем очередь ошибочных запросов
m_ErrorReplyParametersMutex.lock();
m_ErrorReplyParameters.clear();
m_ErrorReplyParametersMutex.unlock();
// начальные значения по всем загружаемым областям
for(std::vector<TMapParameters>::iterator it = g_Maps.begin(); it != g_Maps.end(); ++it) {
   m_TilesCount += it->setStartValues();
   }
// начинаем запрашивать, следующии запросы будут добавляться
// по мере завершения в обработчике requestFinished
m_IsLoadingMaps = true;
bool SendRequest = true;
for(int i = 1; i < g_SimultaneousRequestsCount && SendRequest; i++) SendRequest = newRequest();
//
w_TimeLeft->setText("");
// инициализируем прогресс-бар
w_ProgressTiles->setText("");
w_ProgressBar->setMinimum(0);
w_ProgressBar->setMaximum(m_TilesCount);
// запускаем обновление иконки в трее по таймеру
m_TimerID = startTimer(2000);
}

//------------------------------------------------------------------
void TMapsSpiderDialog::timerEvent(QTimerEvent *Event_)
{
static QTime StartTime = QTime::currentTime();
static int i = 0;

Q_UNUSED(Event_);

// общее количество загруженных тайлов
m_LoadedTilesCount += m_AtomicLoadedTilesCount.fetchAndStoreOrdered(0);
// вычисляем скорость загрузки по нескольким срабатываниям таймера
if(i++ == 5) {
   if(m_LoadedTilesCount > m_OldLoadedTilesCount) {
      m_SpeedTilesPerHour =
         (m_LoadedTilesCount - m_OldLoadedTilesCount) * 3600 / StartTime.secsTo(QTime::currentTime());
      m_OldLoadedTilesCount = m_LoadedTilesCount;
      StartTime = QTime::currentTime();
      }
   i = 0;
   }

setIcon();

if(m_NetworkReplies.isEmpty()) {
   killTimer(m_TimerID);
   m_IsLoadingMaps = false;

   // а все-ли тайлы загружены?
   if(m_TilesCount > m_LoadedTilesCount) {
      // запускаем процесс снова
      startLoadTiles();
      }
   else {
      // изменяем видимость и доступность элементов окна
      setWidgetsVisibility(1);
      w_StartAction->setEnabled(true);
      w_StopAction->setEnabled(false);
      }
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::requestFinished(QNetworkReply* NetworkReply_)
{
m_NetworkRepliesMutex.lock();
if(m_NetworkReplies.size() > 0) {
   // параметры запроса
   TReplyParameters ReplyParameters = m_NetworkReplies[NetworkReply_];
   m_NetworkReplies.remove(NetworkReply_);
   m_NetworkRepliesMutex.unlock();
   // обработка результата
   switch(NetworkReply_->error()) {
   case QNetworkReply::NoError: {
      // данные по запросу получены
      QPixmap Tile;
      Tile.loadFromData(NetworkReply_->readAll());
      // сохраняем тайл по указанному пути
      QFileInfo TileFileInfo = ReplyParameters.tileFileInfo();
      QDir Path(TileFileInfo.absolutePath());
      if(!Path.exists()) Path.mkpath(Path.absolutePath());
      QString FileFullName = TileFileInfo.absoluteFilePath();
      QString MapVersion = ReplyParameters.mapInterface()->mapVersion();
      if(MapVersion.isEmpty())   Tile.save(FileFullName);
      else {
         // Дописываем в тайл информацию о версии карт
         QByteArray PNGImage;
         QBuffer Buffer(&PNGImage);
         Buffer.open(QIODevice::WriteOnly);
         Tile.save(&Buffer, "PNG");
         qnetmap::TPNGImage Image(PNGImage);
         Image.addChunk("qNMp", QByteArray("Map version").append('\0').append(MapVersion));
         Image.save(FileFullName);
         }
      // увеличиваем счетчик загруженных тайлов
      m_AtomicLoadedTilesCount.fetchAndAddOrdered(1);
      break;
      }
   case QNetworkReply::ContentNotFoundError: {
      // запрашиваемый тайл отсутствует (HTTP 404)
      // например, не все области доступны с высоким разрешением
      // увеличиваем счетчик загруженных тайлов
      m_AtomicLoadedTilesCount.fetchAndAddOrdered(1);
      break;
      }
   default: {
      // прочие ошибки
#ifdef _DEBUG
      // обработка ошибки отсутствия запрашиваемого тайла
      QString ErrorString = NetworkReply_->errorString();
      QNetworkReply::NetworkError Error = NetworkReply_->error();
      // пишем в лог
      if(Error != QNetworkReply::OperationCanceledError) {
         qnetmap::TBase::qnetmaplog(qnetmap::QNetMapTranslator::tr("NetworkError: %1 %2").arg(NetworkReply_->request().url().toString()).arg(ErrorString), this);
      }
#endif
      if(ReplyParameters.mapInterface() != NULL) {
         // заносим запрос в список ошибочных
         m_ErrorReplyParametersMutex.lock();
         m_ErrorReplyParameters.push_back(ReplyParameters);
         m_ErrorReplyParametersMutex.unlock();
         // увеличиваем счетчик ошибок загрузки тайлов
         m_NotLoadedTilesCount.fetchAndAddOrdered(1);
         }
      }
   }
   // следующий запрос
   if(w_StopAction->isEnabled()) newRequest();

   // We receive ownership of the reply object and therefore need to handle deletion.
   NetworkReply_->deleteLater();
   }
else m_NetworkRepliesMutex.unlock();
}

//------------------------------------------------------------------
bool TMapsSpiderDialog::newRequest(void)
{
bool RequestSent = false;
bool AllLoaded = allLoaded();
//
QMutexLocker ErrorReplyLocker(&m_ErrorReplyParametersMutex);
int ErrorListSize = m_ErrorReplyParameters.size();
qnetmap::THttpParametersList& HttpParametersList = qnetmap::TMapNetwork::httpParametersList();
unsigned HttpParametersListSize = HttpParametersList.size();
//
while(!RequestSent && (!AllLoaded || ErrorListSize > 0)) {
   if(AllLoaded || ErrorListSize > 500 || (ErrorListSize > 0 && qrand() % 6 == 0)) {
      // параметры запроса
      TReplyParameters ReplyParameters = m_ErrorReplyParameters.first();
      m_ErrorReplyParameters.removeFirst();
      //
      ReplyParameters.setRequery();
      // создаем запрос
      QUrl Url = ReplyParameters.newUrl();
      QNetworkRequest Request = QNetworkRequest(Url);
      // устанавливаем параметры http-запроса
      if(HttpParametersListSize) {
         qnetmap::THttpParametersList::const_iterator it = HttpParametersList.begin() + rand() % HttpParametersList.size();
#ifdef QNETMAP_QT_4
      Request.setRawHeader("User-Agent", it->userAgent().toAscii());
#else
      Request.setRawHeader("User-Agent", it->userAgent().toLatin1());
#endif
      }
      // устанавливаем приоритет сети
      Request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork);
      // запрашиваем
      QList<QNetworkProxy> ProxyList = QNetworkProxyFactory::proxyForQuery(QNetworkProxyQuery(Url));
      unsigned ManagerNumber = qrand() % m_CountNetworkManagers + 1;
      m_NetworkManagers[ManagerNumber]->setProxy(ProxyList[0]);
      QNetworkReply* Reply = m_NetworkManagers[ManagerNumber]->get(Request);
      // запоминаем параметры запроса
      QMutexLocker NRLocker(&m_NetworkRepliesMutex);
      m_NetworkReplies[Reply] = ReplyParameters;
      // уменьшаем счетчик ошибок загрузки тайлов
      m_NotLoadedTilesCount.fetchAndAddOrdered(-1);
      //
      RequestSent = true;
      }
   else {
      // проверка, достигнут-ли конец в текущей области
      if(!m_itCurrentMap->m_itCurrentArea->endMade()) {
         qnetmap::IMapAdapterInterface* MapInterface = m_itCurrentMap->mapInterface();
         int Zoom = m_itCurrentMap->m_itCurrentArea->zoom();
         int X = m_itCurrentMap->m_itCurrentArea->currentTileX();
         int Y = m_itCurrentMap->m_itCurrentArea->currentTileY();
         MapInterface->setZoom(Zoom);
         if(MapInterface->isValid(X, Y, Zoom)) {
            QFileInfo TileFileInfo(MapInterface->tileStorageFullPath(X, Y, Zoom));
            if(!TileFileInfo.exists()) {
               QUrl Url("http://" + MapInterface->host() + MapInterface->query(X, Y, Zoom));
               // создаем запрос
               QNetworkRequest Request = QNetworkRequest(Url);
               // устанавливаем параметры http-запроса
               if(HttpParametersListSize) {
                  qnetmap::THttpParametersList::const_iterator it = HttpParametersList.begin() + rand() % HttpParametersList.size();
#ifdef QNETMAP_QT_4
                  Request.setRawHeader("User-Agent", it->userAgent().toAscii());
#else
                  Request.setRawHeader("User-Agent", it->userAgent().toLatin1());
#endif
               }
               // устанавливаем приоритет сети
               Request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferNetwork); 
               // запрашиваем
               QNetworkReply* Reply = m_NetworkManagers[qrand() % m_CountNetworkManagers + 1]->get(Request);
               // запоминаем параметры запроса
               QMutexLocker Locker(&m_NetworkRepliesMutex);
               m_NetworkReplies[Reply] = TReplyParameters(MapInterface, Url, TileFileInfo, X, Y, Zoom);
               //
               RequestSent = true;
               }
            else {
               // тайл загружен ранее
               // увеличиваем счетчик загруженных тайлов
               m_AtomicLoadedTilesCount.fetchAndAddOrdered(1);
               }
            }
         else {
            // у тайла некорректный номер
            // увеличиваем счетчик загруженных тайлов
            m_AtomicLoadedTilesCount.fetchAndAddOrdered(1);
            }
         }
      // увеличиваем все счетчики
      m_itCurrentMap->m_itCurrentArea->incCurrentTile();
      m_itCurrentMap->incItCurrentArea();
      m_itCurrentMap++;
      if(m_itCurrentMap == g_Maps.end()) m_itCurrentMap = g_Maps.begin();
      // все тайлы в обработке?
      if(!RequestSent) AllLoaded = allLoaded();
      }
   }

return RequestSent;
}

//------------------------------------------------------------------
bool TMapsSpiderDialog::allLoaded(void) const
{
static QMutex Mutex;
//
QMutexLocker Locker(&Mutex);
//
std::vector<TMapParameters>::const_iterator itMaps;
for(itMaps = g_Maps.begin(); itMaps != g_Maps.end(); ++itMaps) {
   std::vector<TAreaParameters>::const_iterator itAreas;
   for(itAreas = itMaps->m_Areas.begin(); itAreas != itMaps->m_Areas.end(); ++itAreas) {
      if(!itAreas->endMade()) return false;
      }
   }

return true;
}

//------------------------------------------------------------------
QString TMapsSpiderDialog::timeLeft(void)
{
// Russian: %1д.
QString TemplateDays = MapsSpiderTranslator::tr("%1d");
if(m_SpeedTilesPerHour <= 0) return QString("--:--:--");


unsigned long long SecondsLeft = 
   (m_TilesCount - m_LoadedTilesCount - m_AtomicLoadedTilesCount) * 3600 / m_SpeedTilesPerHour;
if((m_OldSecondsLeft > 0.7 * SecondsLeft && m_OldSecondsLeft < 1.3 * SecondsLeft))
   SecondsLeft = (SecondsLeft +  5 * m_OldSecondsLeft) / 6;
unsigned int Days = SecondsLeft / (3600 * 24);
unsigned int DayTime = SecondsLeft % (3600 * 24);
QTime Time = QTime(DayTime / 3600, (DayTime % 3600) / 60, (DayTime % 3600) % 60);
m_OldSecondsLeft = SecondsLeft;

if(Days == 0) return Time.toString();
else          return TemplateDays.arg(Days) + Time.toString();
}

//------------------------------------------------------------------
QComboBox* TMapsSpiderDialog::createComboBox(const QString &Name_, const int Count_)
{
QSettings Settings(qnetmap::QNetMapConsts::Organization, Consts::Application);
// создаем
QComboBox* comboBox = new QComboBox;
comboBox->setEditable(true);
comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
// заполняем из реестра
QString TemplatePropertyName = Name_ + "_%1";
for(int i=0; i<Count_; ++i) {
   QString Key = TemplatePropertyName.arg(i);
   if(Settings.contains(Key)) {
      QString Value = Settings.value(Key).toString();
      if(Value.isEmpty()) break;
      comboBox->addItem(Value);
      }
   }
//
return comboBox;
}

//------------------------------------------------------------------
void TMapsSpiderDialog::saveComboBox(
   QComboBox* ComboBox_, const QString &Name_, const int Count_)
{
QSettings Settings(qnetmap::QNetMapConsts::Organization, Consts::Application);
// запоминаем не более Count_ строк в реестре
int CurrentIndex = ComboBox_->currentIndex();
int Count = Count_ > ComboBox_->count() ? ComboBox_->count() : Count_;
QString TemplatePropertyName = Name_ + "_%1";
for(int i=0, delta=1; i<Count_; ++i) {
   QString Key;
   if(i == CurrentIndex) {
      delta = 0;
      Key = TemplatePropertyName.arg(0);
      }
   else
      Key = TemplatePropertyName.arg(i + delta);
   //
   if(i < Count) Settings.setValue(Key, ComboBox_->itemText(i));
   else if(Settings.contains(Key)) Settings.remove(Key);
   }
}

//------------------------------------------------------------------
void TMapsSpiderDialog::setWidgetsVisibility(int CurrentMode_)
{
bool Mode1 = CurrentMode_ == 1;
bool Mode2 = CurrentMode_ == 2;

w_FileBrowseButton->setEnabled(Mode1);
w_DirectoryBrowseButton->setEnabled(Mode1);
w_FileComboBox->setEnabled(Mode1);
w_DirectoryComboBox->setEnabled(Mode1);
w_StartButton->setVisible(Mode1);
w_ExitButton->setVisible(Mode1);

w_ProgressTiles->setVisible(Mode2);
w_TimeLeft->setVisible(Mode2);
w_ProgressBar->setVisible(Mode2);
w_StopButton->setVisible(Mode2);
}

//------------------------------------------------------------------
QString TMapsSpiderDialog::programTitle(void)
{
// Russian: Загрузка карт
return MapsSpiderTranslator::tr("Maps loader");
}

//------------------------------------------------------------------
QString TMapsSpiderDialog::programVersion(void)
{
return qnetmap::VersionString() + "-" + qnetmap::VersionHash();
}
