/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl MapControl code by Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNETMAP_MAPCONTROL_H
#define QNETMAP_MAPCONTROL_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QPrinter>
#else
   #include <QtPrintSupport/QPrinter>
#endif
#include <QMutex>
#include <QtGui>

#include "qnetmap_interfaces.h"
#include "qnetmap_polyline.h"

namespace qnetmap
{
   class TLayerManager;
   class TMapAdapter;
   class TLayer;
   class TImagePoint;
   class TPoint;
   class TGeometry;

   //! \class TMapControl
   //! \brief The control element of the widget and also the widget itself
   class QNETMAP_EXPORT TMapControl : public QWidget
   {
      Q_OBJECT

   public:
      typedef QWidget TParent;
      //! Declares what actions mouse movements have on the map
      enum MouseMode {
         Dragging,    //!< The map is moved
         Panning,     //!< A rectangular can be drawn
         SelectPoint, //!< указание точки
         SelectArea,  //!< указание прямоугольной области
         RubberBand,  //!< выбор прямоугольной области
         None         //!< Mouse move events have no efect to the map
         };

      //! Определяет тип выбора
      enum TSelectedAreaType {
         PointAreaType,       //!< Выбрана точка
         RubberBandAreaType   //!< Выбрана прямоугольная область
         };

      //! \class TSelectedArea
      //! \brief Класс с параметрами выбора
      class TSelectedArea
      {
      public:
         TSelectedArea() : m_IsValid(false) {}
         explicit TSelectedArea(TSelectedAreaType Type_,
                       int SelectionOverviewImageWidth_ = 0,
	                    int SelectionOverviewImageHeight_ = 0)
            : m_Type(Type_), m_Show(true), m_ReadOnly(false), m_IsValid(false),
              m_SelectionOverviewImageWidth(SelectionOverviewImageWidth_),
              m_SelectionOverviewImageHeight(SelectionOverviewImageHeight_),
              m_ReturnImage(SelectionOverviewImageWidth_ > 0 && SelectionOverviewImageHeight_ > 0) {}
         explicit TSelectedArea(QPointF &Point_,
                       bool ReadOnly_ = false,
                       bool Show_ = true,
                       int SelectionOverviewImageWidth_ = 0,
	                    int SelectionOverviewImageHeight_ = 0)
            : m_Type(PointAreaType), m_Show(Show_), m_ReadOnly(ReadOnly_),
              m_SelectionOverviewImageWidth(SelectionOverviewImageWidth_),
              m_SelectionOverviewImageHeight(SelectionOverviewImageHeight_),
              m_ReturnImage(SelectionOverviewImageWidth_ > 0 && SelectionOverviewImageHeight_ > 0)
         {
         addPoint(Point_);
         }
         TSelectedArea(QRectF &Rect_,
                       bool ReadOnly_ = false,
                       bool Show_ = true,
                       int SelectionOverviewImageWidth_ = 0,
	                    int SelectionOverviewImageHeight_ = 0)
            : m_Type(RubberBandAreaType), m_Show(Show_), m_ReadOnly(ReadOnly_),
              m_SelectionOverviewImageWidth(SelectionOverviewImageWidth_),
              m_SelectionOverviewImageHeight(SelectionOverviewImageHeight_),
              m_ReturnImage(SelectionOverviewImageWidth_ > 0 && SelectionOverviewImageHeight_ > 0)
         {
         addPoint(Rect_.topLeft());
         addPoint(Rect_.bottomRight());
         }

         //! \brief Возвращает признак возможности редактирования
         bool isReadOnly(void) const { return m_ReadOnly; }
         //! \brief Возвращает признак необходимости показа выбора
         bool toShow(void) const { return m_Show; }
         //! \brief Возвращает признак необходимости возврата рисунка
         bool toReturnImage(void) const { return m_ReturnImage; }
         //! \brief Возвращает тип выбора
         TSelectedAreaType type(void) const { return m_Type; }
         //! \brief Возвращает список точек
         QList<QPointF> points(void)  const { return m_Points; }
         //! \brief Ширина возвращаемого рисунка
         int imageWidth(void)  const { return m_SelectionOverviewImageWidth; }
         //! \brief Высота возвращаемого рисунка
         int imageHeight(void) const { return m_SelectionOverviewImageHeight; }
         //! \brief Возврат признака правильности задания области
         bool isValid(void) const { return m_IsValid; }
         //! \brief добавление точки в список
         void addPoint(const QPointF PointF_)
         {
         if(isValidPoint(PointF_)) {
            m_Points.push_back(PointF_);
            setIsValid();
            }
         else assert(!"Invalid point");
         }

      private:
         //! \brief Установка признака правильности задания области
         void setIsValid(void)
         {
         bool IsValid = false;
         //
         switch(m_Type) {
         case PointAreaType:
            if(m_Points.count() == 1 && isValidPoint(*(m_Points.begin()))) IsValid = true;
            break;
         case RubberBandAreaType:
            if(m_Points.count() == 2) {
               QPointF Point1 = *(m_Points.begin());
               QPointF Point2 = *(m_Points.begin() + 1);
               IsValid = isValidPoint(Point1) && isValidPoint(Point2);
               }
            break;
            }
         //
         m_IsValid = IsValid;
         }
         //! \brief Проверка вхождения координат точки в нужный диапазон
         bool isValidPoint(const QPointF PointF_) const
         {
         qreal X = PointF_.x(), Y = PointF_.y();
         return X >= -180. && X <= 180. && Y >= -90. && Y <= 90.;
         }

         //! \var Тип выбора
         TSelectedAreaType m_Type;
         //! \var Признак необходимости показа выбора
         bool m_Show;
         //! \var Признак возможности редактирования
         bool m_ReadOnly;
         //! \var признак правильности задания координат области
         bool m_IsValid;
         //! \var Список точек, соответствующий выбранному типу
         QList<QPointF> m_Points;
         //! \var Ширина возвращаемого рисунка
         int m_SelectionOverviewImageWidth;
         //! \var Высота возвращаемого рисунка
         int m_SelectionOverviewImageHeight;
         //! \var Признак необходимости возврата рисунка
         bool m_ReturnImage;
         //! \var Папка, содержащая плагины
         QString m_PluginsDir;
         };

      //
      TMapControl(QSize size, MouseMode mousemode = Dragging,
                  bool showScale = false, bool showCrosshairs = false,
                  TSelectedArea* Area_ = NULL,
                  QWidget *Parent_ = NULL);
      virtual ~TMapControl();

      //! adds a layer
      /*!
      * If multiple layers are added, they are painted in the added order.
      * @param layer the layer which should be added
      */
      bool addLayer(TLayer* layer);

      //! returns the layer with the given name
      /*!
      * @param  layername name of the wanted layer
      * @return the layer with the given name
      */
      TLayer* layer(const QString& layername) const;

      //! returns the names of all layers
      /*!
      * @return returns a QList with the names of all layers
      */
      QList<QString> layerNames() const;

      //! returns the number of existing layers
      /*!
      * @return returns the number of existing layers
      */
      int numberOfLayers() const;

      //! returns the coordinate of the center of the map
      /*!
      * @return returns the coordinate of the middle of the screen
      */
      QPointF currentCoordinate() const;

      //! returns the current zoom level
      /*!
      * @return returns the current zoom level
      */
      int currentZoom() const;

      //! sets the middle of the map to the given coordinate
      /*!
      * @param  coordinate the coordinate which the view?s middle should be set to
      */
      void setView(const QPointF& coordinate) const;

      //! sets the view, so all coordinates are visible
      /*!
      * @param  coordinates the Coorinates which should be visible
      */
      void setView(const QList<QPointF> coordinates) const;

      //! sets the view and zooms in, so all coordinates are visible
      /*!
      * The code of setting the view to multiple coordinates is "brute force" and pretty slow.
      * Have to be reworked.
      * @param  coordinates the Coorinates which should be visible
      */
      //void setViewAndZoomIn(const QList<QPointF> coordinates) const;

      //! sets the view to the given TPoint
      /*!
      * @param point the geometric point the view should be set to
      */
      void setView(const TPoint* point) const;

      //! Keeps the center of the map on the TGeometry, even when it moves
      /*!
      * To stop the following the method stopFollowing() have to be called
      * @param  geometry the TGeometry which should stay centered.
      */
      void followGeometry(const TGeometry* geometry) const;

      //TODO:
      // void followGeometry(const QList<TGeometry*>) const;

      //! Stops the following of a TGeometry
      /*!
      * if the view is set to follow a TGeometry this method stops the trace.
      * See followGeometry().
      * @param geometry the TGeometry which should not followed anymore
      */
      void stopFollowing(TGeometry* geometry);

      //! Smoothly moves the center of the view to the given Coordinate
      /*!
      * @param  coordinate the Coordinate which the center of the view should moved to
      */
      void moveTo(QPointF coordinate);

      //! sets the Mouse Mode of the MapControl
      /*!
      * There are three MouseModes declard by an enum.
      * The MouesMode Panning draws an rectangular in the map while the MouseButton is pressed.
      * When the Button is released a boxDragged() signal is emitted.
      *
      * The second MouseMode (the default) is Dragging, which allows to drag the map around.
      * @param mousemode the MouseMode
      */
      void setMouseMode(const MouseMode mousemode);

      //! returns the current MouseMode
      /*!
      * For a explanation for the MouseModes see setMouseMode()
      * @return the current MouseMode
      */
      TMapControl::MouseMode mouseMode();
      //! \brief сохраняет текущий режим мыши в свойстве m_OldMouseMode
      void saveMouseMode(void);
      //! \brief восстанавливает режим мыши из свойства m_OldMouseMode
      void restoreMouseMode(void);

      //int rotation;

      //! Enable persistent caching of map tiles
      /*!
      * Call this method to allow the QMapControl widget to save map tiles
      * persistent (also over application restarts).
      * Tiles are stored in the subdirectory "QMapControl.cache" within the
      * user's home directory. This can be changed by giving a path.
      * @param path the path to the cache directory
      */
      void enablePersistentCache(const QDir& path=QDir::homePath() + "/QNetMap.cache");

      //! Sets the proxy for HTTP connections
      /*!
      * This method sets the proxy for HTTP connections.
      * This is not provided by the current Qtopia version!
      * @param host the proxy?s hostname or ip
      * @param port the proxy?s port
      */
      void setProxy(const QString& Host_, const int Port_);
      void unSetProxy(void);

      //! \brief Загрузка из файла списка прокси-серверов с проверкой их работоспособности
      //! \param QString FilePath_ - текстовый файл со списком серверов
      //!                            в формате сервер:порт построчно
      //!                            сервер может быть как IP-адресом, так и доменным именем
      //! \param unsigned int MSecs_ - ожидание в миллисекундах, ответ в течении которых
      //!                              включает сервер в список
      //! \return int - количество сохраненных работающих прокси
      //int setProxyList(const QString& FilePath_, const unsigned int MSecs_ = 1000);

      //! Displays the scale within the widget
      /*!
      * @param show true if the scale should be displayed
      */
      void showScale(const bool visible);
      //! \brief Устанавливает признак отображения перекрестия в центре карты
      //! \param bool visible - true - перекрестие в центре карты отображается
      void showCrosshairs(const bool visible);
      //! \brief Вывод отображенной на экране карты на указанное устройство
      //! \param QPrinter *Printer_ - Указатель на устройство
      //! \param QRect Rect_ - Отображаемый прямоугольник карты (если пустой, отбражается все окно)
      //! \param QPoint PointUpLeft_ - Координаты верхнего левого угла на устройстве печати
      //! \param bool DrawEdging_ - Признак отрисовки рамки вокруг карты (по умолчанию отсутствует)
      //! \return QRect - Координаты прямоугольника, отображенного на печатающем устройстве
      QRect print(QPrinter *Printer_, QRect Rect_ = QRect(),
                  QPoint PointUpLeft_ = QPoint(0,0), bool DrawEdging_ = false);
      //! \brief Отрисовывает карту на указаном устройстве
      void drawScreen(QPainter* Painter_) const;
      //! \brief Получить указатель на менеджер слоев
      //! \return TLayerManager* - Указатель на менеджер слоев
      inline TLayerManager* layerManager(void) const { return w_LayerManager; }
      //! \brief Получить признак состояния прерывания загрузки
      //! \return bool - Признак состояния прерывания загрузки
      inline bool abortLoading(void) const { return m_AbortLoading; }
      //! \brief Установить признак состояния прерывания загрузки
      //! \param bool AbortLoading_ - Признак состояния прерывания загрузки
      inline void setAbortLoading(const bool AbortLoading_) { m_AbortLoading = AbortLoading_; }
      //! \brief
      bool setPointMarker(const QPointF Point_, QPixmap* m_PointMarker, bool NoUpdate_ = false);
      //! \brief Загрузка плагинов
      //! \param QString PluginsPath_ - путь для поиска плагинов
      //!                               если путь не задан, производится поиск сначала
      //!                               в папке программы, затем в текущей папке
      //!                               (имена плагинов должны соответствовать шаблону qnetmap_*.dll)
      //! \param bool MessagePluginsNotFound_ - признак выдачи сообщения, если плагины не найдены
      //! \return IMapAdapterInterfaces - список интерфейсов карт загруженных плагинов
      static IMapAdapterInterfaces loadPlugins(const QString PluginsPath_ = "", const bool MessagePluginsNotFound_ = true);
      //! \brief Возвращает размер видимой области в пикселах
      inline QSize size(void) const { return m_Size; }
      //! \brief Преобразует экранные координаты в географические
      //! \param QPoint Point_ - Точка, содержащая экранные координаты
      //! \return QPointF - географические координаты точки
      QPointF displayToWorldCoordinate(const QPoint Point_);
      //! \brief Преобразует экранные координаты прямоугольника в географические
      //! \param QRect Rect_ - Прямоугольник, содержащий экранные координаты
      //! \return QRectF - географические координаты прямоугольника
      QRectF displayToWorldCoordinate(const QRect Rect_);
      //! \brief Преобразует географические координаты прямоугольника в экранные
      //! \param QRectF RectF_ - Прямоугольник, содержащий географические координаты
      //! \return QRect - экранные координаты прямоугольника
      QRect worldToDisplayCoordinate(const QRectF RectF_);
      //! \brief Преобразует географические координаты точки в экранные
      //! \param QPointF PointF_ - Точка, содержащая географические координаты
      //! \return QPoint - экранные координаты точки
      QPoint worldToDisplayCoordinate(const QPointF PointF_);
      //! \brief
      inline bool modified(void) const { return m_Modified; }
      //! \brief
      inline void setModified(bool Modified_ = true);
      //! \brief 
      inline bool editMode(void) const { return m_EditMode; }
      //! \brief 
      inline void setEditMode(const bool Mode_ = true) { m_EditMode = Mode_; }
      //! \brief Преобразует экранные координаты в глобальные пиксельные
      //! \param QPoint Point_ - экранные координаты
      //! \return QPoint - глобальные пиксельные координаты
      QPoint displayToImage(const QPoint Point_);
      //! \brief Преобразует глобальные пиксельные координаты в экранные
      //! \param QPoint Point_ - глобальные пиксельные координаты
      //! \return QPoint - экранные координаты
      QPoint imageToDisplay(const QPoint Point_);
      //! \brief selection of objects in view mode
      inline bool selectionInViewMode(void) const { return m_SelectionInViewMode; }
      //! \brief set selection of objects in view mode
      inline void setSelectionInViewMode(const bool Selection_ = true) { m_SelectionInViewMode = Selection_; }
      //! \brief 
      void deleteLayerManager(void);

   private:
      Q_DISABLE_COPY(TMapControl)

      //! \brief 
      void showPointCoordinatesToolTip(const QPoint MousePoint_, const QPointF Coordinates_);

      //! \var Список дистанций масштабной линейки
      static QList<double> m_DistanceList;
      //! \var Признак того, что есть неотрисованные выполненные запросы тайлов из интернет
      bool m_RequestFinished;
      //! \var Указатель на менеджер слоев
      TLayerManager* w_LayerManager;
      //! \var Middle of the widget (half size)
      QPoint m_ScreenMiddle;
      //! \var Used for scrolling (MouseMode Dragging)
      QPoint m_MouseClickPoint;
      //! \var Used for scrolling and dragging (MouseMode Dragging/Panning)
      QPoint m_MouseCurrentPoint;
      //! \var Size of the widget
      QSize m_Size;
      //! \var Прямоугольник выделения участка карты в режиме RubberBand
      QRubberBand* w_RubberBand;
      //! \var Прямоугольник выделения участка карты в режиме Dragging
      QRubberBand* w_DraggingRubberBand;
      //! \var 
      QPoint       m_DruggingPoint;
      //! \var Признак нажатия левой кнопки мыши
      bool m_MouseLeftButtonPressed;
      //! \var Текущий режим мыши
      MouseMode m_MouseMode;
      //! \var Предыдущий режим мыши
      MouseMode m_OldMouseMode;
      //! \var Признак видимости линейки масштаба
      bool m_ScaleVisible;
      //! \var Признак видимости перекрестия в центре карты
      bool m_CrosshairsVisible;
      //! \var Used for method moveTo()
      QMutex m_MoveMutex;
      //! \var Used for method moveTo()
      QPointF m_Target;
      //! \var Used for method moveTo()
      int m_Steps;
      //! \var Признак процесса прерывания всех загрузок для данного управляющего класса
      bool m_AbortLoading;
      //! \var хранилище "старого курсора"
      QCursor m_OldCursor;
      //! \var Картинка маркера позиции
      QPixmap m_PointMarker;
      //! \var Указатель на класс маркера позиции
      TImagePoint* w_PointMarkerImage;
      //! \var Указатель на класс с параметрами выбранной области
      TSelectedArea m_SelectedArea;
      //! \var Признак того, что выбранная область показана
      bool m_SelectionIsShown;
      //! \var Прямоугольник с географическими координатами выбранной области
      QRectF m_RubberBandCoordinate;
      //! \var
      bool m_SelectArea;
      //! \var
      QAtomicInt m_MouseMoveAtomicInt;
      //! \var
      TGeometry* w_GeometryClicked;
      //! \var
      TGeometry* w_OldGeometryClicked;
      //! \var
      QPoint m_GeometryClickedCoordinateShift;
      //! \var Признак изменения состояния объектов на карте
      bool m_Modified;
      //! \var 
      bool m_EditMode;
      //! \var 
      TPolyLine::TPointParameters m_PointParameters;
      //! \var 
      bool m_MouseMovedBetweenPressRelease;
      //! \var selection of objects in view mode
      bool m_SelectionInViewMode;
      //! \var 
      static QAtomicInt m_AtomicLoadedMapControlsCount;

//       TMapControl& operator= (const TMapControl& rhs);
//       TMapControl(const TMapControl& old);

   protected:
      void timerEvent(QTimerEvent* evnt);
      void paintEvent(QPaintEvent* evnt);
      void mousePressEvent(QMouseEvent* evnt);
      void mouseReleaseEvent(QMouseEvent* evnt);
      void mouseMoveEvent(QMouseEvent* evnt);
      void wheelEvent(QWheelEvent* evnt);
      void deleteRubberBand(void);
      void resizeRubberBand(void);
      //! \brief Вырезает указанную часть экрана
      QPixmap cutPartOfTheScreen(const QRect Rect_);

   signals:
      //! \brief Signal, Emitted AFTER a MouseEvent occured
      /*!
      * This signals allows to receive click events within the MapWidget together with the world coordinate.
      * It is emitted on MousePressEvents and MouseReleaseEvents.
      * The kind of the event can be obtained by checking the events type.
      * @param  evnt the QMouseEvent that occured
      * @param  coordinate the corresponding world coordinate
      */
      void mouseEventCoordinate(const QMouseEvent* evnt, const QPointF Coordinate_);
      void mouseMoveEventCoordinate(const TMapControl* MapControl_, const QPointF Coordinate_);
      void mouseSelectPointEvent(
         const QMouseEvent* evnt, const QPoint Point_, const QPointF PointF_, const QPixmap Pixmap_);
      //! \brief Signal, Emitted, after a Rectangular is dragged.
      /*!
      * It is possible to select a rectangular area in the map, if the MouseMode is set to Panning.
      * The coordinates are in world coordinates
      * @param  QRectF the dragged Rect
      */
      void boxDragged(const QRectF);
      //! \brief Signal, This signal is emitted, when a TGeometry is clicked
      /*!
      * @param geometry The clicked TGeometry object
      * @param coord_px  The coordinate in pixel coordinates
      */
      //void geometryClicked(TGeometry* geometry, QPoint coord_px);

      //! \brief Signal, This signal is emitted, after the view have changed
      /*!
      * @param coordinate The current coordinate
      * @param zoom The current zoom
      */
      void viewChanged(const QPointF &coordinate, int zoom);
      //! \brief Сигнал о выборе прямоугольной области
      //! \param QRect - пиксельные координаты прямоугольника
      //! \param int - текущий масштаб
      void rubberRectSelected(const QRect, int);
      //! \brief Сигнал об отмене выбора прямоугольной области
      void rubberRectDeselect(void);
      //! \brief Сигнал о выборе прямоугольной области
      //! \param QRectF - географические координаты прямоугольника
      //! \param QPixmap - захваченная с экрана выбранная область
      void rubberRectSelected(const QRectF RectF_, const QPixmap Pixmap_);
      //! \brief
      void modifiedChanged(void);

   public slots:
      //! \brief zooms in one step
      void zoomIn(void);
      //! \brief zooms out one step
      void zoomOut(void);
      //! \brief sets the given zoomlevel
      /*!
      * @param zoomlevel the zoomlevel
      */
      void setZoom(const int zoomlevel, const bool ZoomBackground_ = true,  const bool MapUpdate_ = true);
      //! \brief scrolls the view to the left
      void scrollLeft(const int pixel = 10);
      //! \brief scrolls the view to the right
      void scrollRight(const int pixel = 10);
      //! \brief scrolls the view up
      void scrollUp(const int pixel = 10);
      //! \brief scrolls the view down
      void scrollDown(const int pixel = 10);
      //! \brief scrolls the view by the given point
      void scroll(const QPoint scroll);
      //! \brief updates the map for the given rect
      /*!
      * @param rect the area which should be repainted
      */
      void updateRequest(const QRect rect);
      //! \brief updates the hole map by creating a new offscreen image
      void updateRequestNew(void);
      //! \brief Resizes the map to the given size
      /*!
      * @param newSize The new size
      */
      void resize(const QSize newSize);
      //! \brief обновление
      void update(void);
      //! \brief "Силовое" обновление
      void forceUpdate(void);

   private slots:
      void tick();
      void loadingFinished();
      void positionChanged(TGeometry* Geometry_);
   };
}
#endif
