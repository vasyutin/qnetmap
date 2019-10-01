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

#ifndef MAPSSPIDER_TMapsSpiderDialog_H
#define MAPSSPIDER_TMapsSpiderDialog_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QSystemTrayIcon>
   #include <QDialog>
   #include <QProgressBar>
   #include <QPlastiqueStyle>
#endif
#include <QDir>
#include <QPluginLoader>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QTime>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QPointF;
QT_END_NAMESPACE

#include "parameters.h"
#include "jobsxmlparser.h"
#include "qnetmap_global.h"
#include "qnetmap_interfaces.h"
#include "qnetmap_mapnetwork.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_pngimage.h"
#include "qnetmap_mapcontrol.h"

#include <vector>
#include <map>

namespace Consts {
   static const char *Application  GCC_ATTRIBUTE_UNUSED = "MapsSpider";

} // namespace Consts

///////////////////////////////////////////////////////////////
//! \class TMapsSpiderDialog
//! \brief Основное окно приложения
class TMapsSpiderDialog : public QDialog
{
   Q_OBJECT

public:
   TMapsSpiderDialog(bool ShowTrayIcon_);
   virtual ~TMapsSpiderDialog();

   //! \brief Установка видимости окна
   void setVisible(bool visible);

   static QString programTitle(void);
   static QString programVersion(void);

protected:
   //! \brief Обработка события закрытия окна
   void closeEvent(QCloseEvent *event);

private slots:
   //! \brief Установка нужной иконки окна и в трее
   void setIcon(void);
   //! \brief Обработка событий из трея
   void trayIconActivated(QSystemTrayIcon::ActivationReason Reason_);
   //! \brief Просмотр и выбор папки
   void browseDirectory(void);
   //! \brief Просмотр и выбор файла
   void browseFile(void);
   //! \brief Старт загрузки тайлов
   void start(void);
   //! \brief Остановка загрузки тайлов
   void stop(void);
   //! \brief Обработка события щелчка по сообщению
   void messageClicked(void);
   //! \brief Установка кнопки "старт" включена/выключена в зависимости от
   //!        заполнености необходимых полей.
   //! \param 
   void setStartButtonEnabled(QString);
   //! \brief Загружает необходимые плагины
   bool loadPlugins(void);
   //! \brief Сохраняет указатель на интерфейс карты
   //! \param qnetmap::IMapAdapterInterface* MapInterface_ - Указатель на интерфейс карты
   bool saveMapInterface(qnetmap::IMapAdapterInterface* MapInterface_);
   //! \brief Выдача первых g_SimultaneousRequestsCount запросов
   void startLoadTiles();
   //! \brief Обработка сигнала завершения загрузки тайла
   //! \param QNetworkReply* - указатель на запрос
   void requestFinished(QNetworkReply*);

private:
   Q_DISABLE_COPY(TMapsSpiderDialog)

   //! \brief Инициализирует необходимые действия
   void createActions(void);
   //! \brief Создает иконку в трее
   void createTrayIcon(void);
   //! \brief Создает кнопку
   //! \param QString &Text_ - текст на кнопке
   //! \param QObject *Receiver_ класс-получатель сообщения о нажатии на кнопку
   //! \param char *Member_ - слот-получатель сообщения о нажатии на кнопку
   //! \return QPushButton* - указатель на созданную кнопку
   QPushButton *createButton(const QString &Text_, const QObject *Receiver_, const char *Member_);
   //! \brief Показывает сообщение в всплывающем туултипе трея
   //! \param QString &Title_ - заголовок
   //! \param QString &Message_ - сообщение
   void showMessage(QString &Title_, QString &Message_);
   //! \brief Показывает сообщение об ошибке в всплывающем туултипе трея
   //! \param QString &Message_ - сообщение
   void showErrorMessage(QString &Message_);
   //! \brief Создает и посылает новый запрос
   //! \return bool - если true, то запрос послан
   bool newRequest(void);
   //! \brief Проверка все-ли загружено
   //! \return bool - если true, все тайлы загружены
   bool allLoaded(void) const;
   //! \brief Обработка событий таймера
   //! \param QTimerEvent *Event_ - событие
   void timerEvent(QTimerEvent *Event_);
   //! \brief Вычисляет оставшееся до окончания загрузки время и превращает в строку
   //! \return QString - Оставшееся до окончания загрузки время
   QString timeLeft(void);
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
   //! \brief Устанавливает видимость и доступность элементов страницы в соответствии с режимом работы
   //! \param int CurrentMode_ - режим работы (может быть 1 или 2)
   void setWidgetsVisibility(int CurrentMode_);
   
   // указатели на элементы окна
   QComboBox    *w_FileComboBox;
   QComboBox    *w_DirectoryComboBox;
   QLabel       *w_FileLabel;
   QLabel       *w_DirectoryLabel;
   QPushButton  *w_FileBrowseButton;
   QPushButton  *w_DirectoryBrowseButton;
   QPushButton  *w_StartButton;
   QPushButton  *w_ExitButton;
   QPushButton  *w_StopButton;
   QProgressBar *w_ProgressBar;
   QLabel       *w_Picture;
   QLabel       *w_ProgressTiles;
   QLabel       *w_TimeLeft;
   QStyle       *w_ProgressBarStyle;

   // действия
   QAction *w_RestoreAction;
   QAction *w_QuitAction;
   QAction *w_StartAction;
   QAction *w_StopAction;

   // трей
   // \var признак присутствия в системе трея
   bool m_SystemTrayAvailable;
   // \var указатель на иконку трея
   QSystemTrayIcon *w_TrayIcon;
   // \var указатель на меню трея
   QMenu *w_TrayIconMenu;
   // \var признак показа иконки в трее
   bool m_ShowTrayIcon;

   // \var последнее сообщение об ошибке
   QString m_ErrorMessage;
   // \var время старта загрузки
   QTime   m_TimeStart;
   // \var ID таймера
   int     m_TimerID;
   // \var папка с плагинами
   QDir    m_PluginsDir;
   // \var указатель на текущую карту
   std::vector<TMapParameters>::iterator m_itCurrentMap;
   //! \var Список параметров запросов, отсортированный по действующим запросам
   QMap<QNetworkReply*, TReplyParameters> m_NetworkReplies;
   //! \var Мьютекс работы со списком запросов
   QMutex m_NetworkRepliesMutex;
   //! \var Список параметров ошибочных запросов
   QList<TReplyParameters> m_ErrorReplyParameters;
   //! \var Мьютекс работы со списком ошибочных запросов
   QMutex m_ErrorReplyParametersMutex;

   // \var процесс загрузки тайлов активен
   bool m_IsLoadingMaps;
   // \var при подготовке к загрузке тайлов получена ошибка
   bool m_IsErrorStartLoadingMap;
   // \var скорось загрузки тайлов (тайлов в час)
   int  m_SpeedTilesPerHour;
   // \var общее количество загружаемых тайлов
   unsigned long long m_TilesCount;
   // \var общее количество загруженных тайлов
   unsigned long long m_LoadedTilesCount;
   // \var количество загруженных тайлов между срабатываниями таймера
   QAtomicInt m_AtomicLoadedTilesCount;
   // \var количество тайлов, при загрузке которых протокол http вернул ошибку
   QAtomicInt m_NotLoadedTilesCount;
   // \var предыдущее значение количества загруженных тайлов
   unsigned long long m_OldLoadedTilesCount;
   // \var предыдущее значение оставшихся секунд
   unsigned long long m_OldSecondsLeft;

   // \var указатели на классы - загрузчики
   QMap<int, QNetworkAccessManager*> m_NetworkManagers;
   // \var количество классов - загрузчиков
   unsigned int m_CountNetworkManagers;

	#ifdef Q_OS_WIN
		// The mutex preventing installer to work when the app is started
		HANDLE m_InstallerMutex;
	#endif
	};

#endif
