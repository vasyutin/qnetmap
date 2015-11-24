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
#include <QTextCodec>
#include <QSettings>
#include <QDir>


#include "main.h"
#include "map.h"
#include "mapsviewer_translator.h"

#if defined(_MSC_VER) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#include <windows.h>
#endif

void SetDafaultPathes(void);
void InstallTranslator(const QString &Name_);

//*************************************************************************
// Путь к плагинам может быть передан в единственном параметре 
// при запуске программы. 
// Иначе плагины ищутся в папке программы.
//*************************************************************************
int main(int argc, char *argv[])
{
QApplication app(argc, argv);


#ifdef QNETMAP_QT_4
   QTextCodec *Codec = QTextCodec::codecForName("UTF-8");
   QTextCodec::setCodecForTr(Codec);
   QTextCodec::setCodecForCStrings(Codec);
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
{
   // отлов утечек памяти в debug-сборке
   QFileInfo FileInfo(qApp->applicationFilePath());
   QString AppName = qApp->applicationDirPath() + "/" + FileInfo.baseName() + "_memoryLeaksDump.rtf";
   HANDLE hLogFile = CreateFileA(
#ifdef QNETMAP_QT_4
   AppName.toAscii().constData(), 
#else
   AppName.toLatin1().constData(),
#endif
   GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); _CrtSetReportFile(_CRT_WARN,   hLogFile);
   _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); _CrtSetReportFile(_CRT_ERROR,  hLogFile);
   _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); _CrtSetReportFile(_CRT_ASSERT, hLogFile);
   _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}
#endif

SetDafaultPathes();

// Sometimes we are running the program on the English version of OS but we need
// all messages to be in other language (ex. Russian). In that case we have to use
// -l switch.
QString LanguageName = QLocale::system().name().toLower();
QStringList Arguments(app.arguments());
for(QStringList::const_iterator it = Arguments.begin(); it != Arguments.end(); ++it) {
	if(*it == QLatin1String("-l") && (++it) != Arguments.end()) {
		LanguageName = it->toLower();
		break;
		}
	if(*it == QLatin1String("/?") || *it == QLatin1String("-h")) {
		QString Message = TMap::programTitle() + " (" + TMap::programVersion() + ")\n\n" +
			MapsViewerTranslator::tr(
			"Syntax:\n"
			"   %1 [options]\n"
			"Options:\n"
			"   -p <Folder> - The folder with the plugins for the maps providers.\n"
			"   -l <Lang> - Two letter code of the language. Force to use translation\n"
			"      files for the language specified (if they are exist).")
			.arg(QFileInfo(QCoreApplication::applicationFilePath()).baseName());
		QMessageBox::information(NULL, TMap::programTitle(), Message);
		return 2;
		}
	}

// Installing translators for the current language.
if(LanguageName != QLatin1String("en")) {
	InstallTranslator(QString("qt_") + LanguageName);
	InstallTranslator(QString("qnetmap_") + LanguageName);
	}

TMap Map;
OnQuitObject OQO;
int RetValue = 1;
if(Map.pluginsAreLoaded()) {
   // картографические плагины загружены, можно работать
   Map.resize(800,600);
   Map.setWindowTitle(TMap::programTitle());
   Map.show();
   RetValue = app.exec();
   Map.mapWidget()->abortLoading();
	}

return RetValue;
}

// -----------------------------------------------------------------------
// Костыль для присвоения начальных значений путей для кэша и хранилища карт.
// -----------------------------------------------------------------------
void SetDafaultPathes(void)
{
#ifdef Q_OS_WIN
QSettings Settings("HKEY_CURRENT_USER\\Software\\QNetMap\\MapsViewer",
	QSettings::NativeFormat);
//
if(!Settings.contains("CachePath_0")) {
	QString Dir = QDir::home().absoluteFilePath("MapsCache");
	QDir::home().mkpath(Dir);
	Settings.setValue("CachePath_0", QDir::toNativeSeparators(Dir));
	}
if(!Settings.contains("StoragePath_0")) {
	QString Dir = QDir::home().absoluteFilePath("MapsStore");
	QDir::home().mkpath(Dir);
	Settings.setValue("StoragePath_0", QDir::toNativeSeparators(Dir));
	}
if(!Settings.contains("StorageType")) {
	Settings.setValue("StorageType", unsigned(3));
	}
#endif
}

// -----------------------------------------------------------------------
void InstallTranslator(const QString &Name_)
{
QTranslator *Translator = new QTranslator(QCoreApplication::instance());
if(Translator->load(Name_, QCoreApplication::applicationDirPath())) {
	QCoreApplication::instance()->installTranslator(Translator);
	}
else
	delete Translator;
}