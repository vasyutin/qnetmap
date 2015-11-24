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

#ifndef MAPS_H
#define MAPS_H

#include <qnetmap_global.h>

#ifdef QNETMAP_QT_4
   #include <QPrinter>
   #include <QPrintDialog>
#else
   #include <QtPrintSupport/QPrinter>
   #include <QtPrintSupport/QPrintDialog>
#endif

#include <QtGui>
#include <QPluginLoader>
#include <QDir>
#include <QStringList>
#include <QSettings>

#include "qnetmap_global.h"
#include "qnetmap_interfaces.h"
#include "qnetmap_layer.h"
#include "qnetmap_maplayer.h"
#include "qnetmap_point.h"
#include "qnetmap_imagepoint.h"
#include "qnetmap_widget.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_linestring.h"
#include "qnetmap_loadprogressbar.h"
#include "qnetmap_loaddialogwizard.h"

#include "configuration.h"

namespace Consts {
   static const char *Organization GCC_ATTRIBUTE_UNUSED = "QNetMap";
   static const char *Application  GCC_ATTRIBUTE_UNUSED = "MapsViewer";
} // namespace Consts

/*
class MyObject : public QObject
{
Q_OBJECT

public:
   MyObject():QObject() {}

public slots:
   void print(const QString &s) const { 
      qDebug()<<s; 
      }
   void YMapInitialized(void) const;
   void setWebFrame(QWebFrame* WebFrame_) { m_WebFrame = WebFrame_; }

private:
   QWebFrame* m_WebFrame;
};
*/

class TMap: public QMainWindow
{
   Q_OBJECT

public:
   TMap(QWidget* parent = 0);
   virtual ~TMap();

   //! \brief Возвращает признак загруженности плагинов
   //! \return bool - true - плагины загружены 
   bool pluginsAreLoaded(void) const { return m_PluginsAreLoaded; }
   //! \brief Запоминает папку с кешем
   //! \param QString Directory_ - папка с кешем
   //void setCacheDirectory(const QString Directory_) { m_CacheDirectory = Directory_; }
   //! \brief Запоминает папку хранилища карт
   //! \param QString Directory_ - папка хранилища
   //void setMapsStoreDirectory(const QString Directory_) { m_MapsStoreDirectory = Directory_; }
   //! \brief Запоминает тип хранилища
   //! \param int Type_ - тип хранилища
   //void setMapsStoreType(const int Type_) { m_MapsStoreType = Type_; }
   //! \brief Устанавливает параметры для всех карт
   void setMapsConfiguration(QString MapsStoreDirectory_, int MapsStoreType_);
   //! \brief 
   qnetmap::TMapWidget* mapWidget(void) const { return w_MapWidget; }

   //! \var Список интерфейсов карт
   QList<qnetmap::IMapAdapterInterface*> m_MapInterfaces;

   static QString programTitle(void);
   static QString programVersion(void);
   static QString areaSelectionHelpMessage(void);

protected:
   void resizeEvent(QResizeEvent *event);

   //! \class TMapPrinter
   //! \brief Вспомогательный класс для печати
   class TMapPrinter : public QPrinter
   {
   public:
      //! \brief 
      //! \param PrinterMode Mode_
      //! \param QRect Rect_
      TMapPrinter(PrinterMode Mode_, QRect Rect_ = QRect()) : QPrinter(Mode_), m_Rect(Rect_) {}
      virtual ~TMapPrinter() {}

      //! \brief Возвращает координаты печатаемой области
      //! \return QRect - Координаты печатаемой области
      QRect rect(void) const { return m_Rect; }

   private:
      //! \var координаты печатаемой области
      QRect m_Rect;
   };

private:
   Q_DISABLE_COPY(TMap)

   //
/*
   QWebFrame* m_WebFrame;
   QWebView*  m_WebView;
   MyObject*  m_MyObject;
*/

   //! \var изображение маркера точки
   QPixmap m_Flag;
   //! \var указатель на виджет карт
   qnetmap::TMapWidget* w_MapWidget;
   //! \var указатель на меню инструментов
   QMenu* w_ToolsMenu;
   //! \var указатель на меню карт
   QMenu* w_MapPluginsMenu;
   //! \var указатель на действие - переход в центр карты
   QAction* w_ToolsCenterMap;
   //! \var указатель на действие - вычислить расстояние
   QAction* w_ToolsDistance;
   //! \var указатель на действие - печать
   QAction* w_ToolsPrint;
   //! \var указатель на действие - печать с просмотром
   QAction* w_ToolsPrintPreview;
   //! \var указатель на действие - печать области
   QAction* w_ToolsPrintArea;
   //! \var указатель на действие - печать области с просмотром
   QAction* w_ToolsPrintAreaPreview;
   //! \var указатель на действие - формирование задания на загрузку области
   QAction* w_ToolsLoadArea;
   //! \var указатель на действие - настройка программы
   QAction* w_ToolsConfiguration;
   //! \var указатель на действие - список растровых карт
   QAction* w_ToolsRasterMaps;
   /// Action for closing the application.
   QAction *m_ExitAction;
   /// Action for showing the about dialog.
   QAction *m_AboutAction;
   //! \var 
   QMap<QAction*, QString> m_MapActions;
   //! \var признак игнорирования нажатий на карту
   bool m_IgnoreClicks;
   //! \var признак загруженности плагинов
   bool m_PluginsAreLoaded;
   //! \var координата первого угла выделенной области
   QPointF m_Point1;
   //! \var координата второго угла выделенной области
   QPointF m_Point2;
   //! \var указатель на группу действий - провайдеры карт
   QActionGroup* w_MapProviderGroup;
   //! \var список слоев с прозрачными картами
   QList<qnetmap::TLayer*>  m_OverlayLayers;
   //! \var список действий с прозрачными картами
   QList<QAction*> m_OverlayMapActions;
   //! \var список действий с основными непрозрачными картами
   QList<QAction*> m_BaseMapActions;
   //! \var список действий с растровыми картами
   QList<QAction*> m_RasterMapActions;
   //! \var указатель на основной интерфейс растровой карты
   qnetmap::IMapAdapterInterface* w_RasterMapMainInterface;
   //! \var список интерфейсов растровых карт
   QList<qnetmap::IMapAdapterInterface*> m_RasterMapInterfaces;

   //! \brief Создает действия
   void createActions(void);
   //! \brief Создает меню
   void createMenus(void);
   //! \brief Добавление одного плагина в меню
   //! \param QObject* - указатель на плагин
   //! \param bool - признак отметки как выбранного
   void populateMenus(qnetmap::IMapAdapterInterface* MapInterface_, bool Checked_ = false);

public slots:
   //! \brief Подготовка к вычислению расстояния
   void calcDistance(void);
   //! \brief Событие выбора точки для вычисления расстояния
   //! \param QMouseEvent* - событие мыши
   //! \param QPointF - координаты выбранной точки
   void calcDistanceSelected(const QMouseEvent*, const QPointF);
   //! \brief Печать
   void print(void);
   //! \brief Печать с просмотром
   void printPreview(void);
   //! \brief Печать области
   void printArea(void);
   //! \brief Печать области с просмотром
   void printAreaPreview(void);
   //! \brief Выбор области для мастера формирования заданий на загрузку карт
   void loadArea(void);
   //! \brief настройка программы 
   void configuration(void);
   //! \brief настройка растровых карт 
   void rasterMapsDialog(void);
   //! \brief Событие выбора провайдера карт
   //! \param QAction* - указатель на действие, вызвавшее событие
   void mapProviderSelected(QAction*);
   //! \brief Печать выбранной области карты
   //! \param QPrinter* - контекст принтера (должен указывать на TMapPrinter)
   void printMap(QPrinter*);
   //! \brief Установить вид на центр карты
   void centerMap(void);
   //! \brief Ловит переключение видимости слоя карты в меню 
   void changeVisibility(bool Visibility_);
   //! \brief Ловит изменения списка растровых карт
   void rasterMapsListChanged(void);
   //! \brief Ловит последние изменения параметров карт
   //void viewChanged(QString MapType_, int Zoom_, QPointF Coordinate_);
   //! \brief Ловит изменение типа карты
   //void showsMapType(QString MapType_);
   //! \brief Ловит переключение видимости слоя карты 
   //void changedVisibility(QString MapType_, bool Visibility_);

	void aboutDialog(void);
	void rubberBandHasFinishedSelection(const QRect, int);

   //void populate(void);
};

#endif
