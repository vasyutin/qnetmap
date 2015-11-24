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

#ifndef MAPSSPIDER_PARAMETERS_H
#define MAPSSPIDER_PARAMETERS_H

#include <QPointF>
#include <QString>
#include <QFileInfo>
#include <QUrl>

#include <vector>

#include <qnetmap_interfaces.h>
#include <qnetmap_mapadapter.h>

///////////////////////////////////////////////////////////////
//! \class TAreaParameters
//! \brief Хранит текущие параметры загружаемой области
class TAreaParameters
{
public:
   TAreaParameters() 
      : m_ZoomInitialized(false), m_Point1Initialized(false), m_Point2Initialized(false) {}
   virtual ~TAreaParameters() {}

   //! \brief Сохраняет в параметрах масштаб
   void setZoom(const int Zoom_) { m_Zoom = Zoom_; m_ZoomInitialized = true; }
   //! \brief Сохраняет в параметрах первую точку загружаемой области
   //! \param qreal X_ - координата X
   //! \param qreal Y_ - координата Y
   void setPoint1(const qreal X_, const qreal Y_) 
   {
   m_Point1.setX(X_);
   m_Point1.setY(Y_);
   m_Point1Initialized = true; 
   }
   //! \brief Сохраняет в параметрах вторую точку загружаемой области
   //! \param qreal X_ - координата X
   //! \param qreal Y_ - координата Y
   void setPoint2(const qreal X_, const qreal Y_)
   {
   m_Point2.setX(X_);
   m_Point2.setY(Y_);
   m_Point2Initialized = true; 
   }
   //! \brief Возвращает признак инициализации первой точки загружаемой области
   //! \return bool - Если true, то точка инициализирована
   bool point1Initialized(void) const { return m_Point1Initialized; }
   //! \brief Возвращает признак инициализации второй точки загружаемой области
   //! \return bool - Если true, то точка инициализирована
   bool point2Initialized(void) const { return m_Point2Initialized; }
   //! \brief Возвращает масштаб
   //! \return int - масштаб
   int zoom(void) const { return m_Zoom; }
   //! \brief Возвращает координаты первой точки
   //! \return QPointF - координаты первой точки
   QPointF point1(void) const { return m_Point1; }
   //! \brief Возвращает координаты второй точки
   //! \return QPointF - координаты второй точки
   QPointF point2(void) const { return m_Point2; }
   //! \brief Проверка инициализации всех параметоров загружаемой области
   //! \return bool - true - все параметры инициализированы
   bool isInitialized(void) const
   {
   return m_ZoomInitialized && m_Point1Initialized && m_Point2Initialized;
   }
   //! \brief Устанавливает начальные значения счетчиков на первый тайл
   //! \param qnetmap::IMapAdapterInterface* - указатель на интерфейс карты
   //! \return unsigned long long - общее количество тайлов области
   unsigned long long setStartValues(qnetmap::IMapAdapterInterface* MapInterface_) 
   {
   MapInterface_->setZoom(m_Zoom);
   QPoint P1 = MapInterface_->coordinateToTile(m_Point1);
   QPoint P2 = MapInterface_->coordinateToTile(m_Point2);
   m_MinTilesX = (P1.x() < P2.x() ? P1.x() : P2.x());
   m_MinTilesY = (P1.y() < P2.y() ? P1.y() : P2.y());
   m_MaxTilesX = (P1.x() > P2.x() ? P1.x() : P2.x());
   m_MaxTilesY = (P1.y() > P2.y() ? P1.y() : P2.y());
   m_CurrentTileX = m_MinTilesX;
   m_CurrentTileY = m_MinTilesY;
   return (unsigned long long)(m_MaxTilesX - m_MinTilesX + 1) * (m_MaxTilesY - m_MinTilesY + 1);
   }
   //! \brief Устанавливает счетчики на следующий тайл
   //! \return bool - если true, достигнут конец загружаемой области
   bool incCurrentTile(void)
   {
   m_CurrentTileX++;
   if(m_CurrentTileX > m_MaxTilesX) {
      m_CurrentTileX = m_MinTilesX;
      m_CurrentTileY++;
      if(m_CurrentTileY > m_MaxTilesY) return false;
      }
   return true;
   }
   //! \brief Возвращает номер текущего тайла по координате X
   //! \return int - Номер текущего тайла по координате X
   int currentTileX(void) const { return m_CurrentTileX; }
   //! \brief Возвращает номер текущего тайла по координате Y
   //! \return int - Номер текущего тайла по координате Y
   int currentTileY(void) const { return m_CurrentTileY; }
   //! \brief Возвращает признак достигнут-ли конец загружаемой области
   //! \return bool - если true, достигнут конец загружаемой области
   bool endMade(void) const 
   { 
   return m_CurrentTileY > m_MaxTilesY; 
   }
                             
private:
   //! \var Масштаб
   int m_Zoom;
   //! \var Признак инициализации масштаба
   bool m_ZoomInitialized;
   //! \var Первая точка прямоугольной области
   QPointF m_Point1;
   //! \var Признак инициализации первой точки
   bool m_Point1Initialized;
   //! \var Вторая точка прямоугольной области
   QPointF m_Point2;
   //! \var Признак инициализации второй точки
   bool m_Point2Initialized;
   //! \var Минимальное значение номера тайла по X
   int m_MinTilesX;
   //! \var Максимальное значение номера тайла по X
   int m_MaxTilesX;
   //! \var Минимальное значение номера тайла по Y
   int m_MinTilesY;
   //! \var Максимальное значение номера тайла по Y
   int m_MaxTilesY;
   //! \var Текущее значение номера тайла по X
   int m_CurrentTileX;
   //! \var Текущее значение номера тайла по Y
   int m_CurrentTileY;
};

///////////////////////////////////////////////////////////////
//! \class TMapParameters
//! \brief Хранит текущие параметры карты
class TMapParameters
{
public:
   TMapParameters() : m_PluginName(""), m_MapInterface(NULL) {}
   virtual ~TMapParameters()
   {
   saveValues();
   }

   //! \brief Сохраняет в параметрах наименование плагина
   //! \param QString - Наименование плагина
   void setPluginName(const QString &Name_) { m_PluginName = Name_; }
   //! \brief Возвращает наименование плагина
   //! \return QString - Наименование плагина
   QString pluginName(void) const { return m_PluginName; }
   //! \brief Возвращает признак инициализации необходимых параметров класса
   //! \return bool - true - Все инициализировано
   bool isInitialized(void) const { return m_PluginName != "" && m_Areas.size() > 0; }
   //! \brief Сохраняет в параметрах указатель на интерфейс карты
   //! \param qnetmap::IMapAdapterInterface* - Указатель на интерфейс карты
   void setMapInterface(qnetmap::IMapAdapterInterface* MapInterface_)
   {
   m_MapInterface = MapInterface_;
   }
   //! \brief Возвращает указатель на интерфейс карты
   //! \return qnetmap::IMapAdapterInterface* - Указатель на интерфейс карты
   qnetmap::IMapAdapterInterface* mapInterface(void) const { return m_MapInterface; }
   //! \brief Устанавливает в параметрах класса и в параметрах загружаемых областей
   //!        начальные значения счетчиков
   //! \return unsigned long long - Общее количество загружаемых тайлов
   unsigned long long setStartValues(void) 
   {
   unsigned long long TilesCount = 0;
   restoreValues();
   m_itCurrentArea = m_Areas.begin();
   for(std::vector<TAreaParameters>::iterator it = m_Areas.begin(); it != m_Areas.end(); ++it) {
      TilesCount += it->setStartValues(m_MapInterface);
      }
   return TilesCount;
   }
   //! \brief Сохраняет значения параметров
   void saveValues(void) 
   {
   }
   //! \brief Восстанавливает ранее сохраненные значения параметров
   void restoreValues(void) 
   {
   }
   //! \brief Увеличивает счетчики класса
   void incItCurrentArea(void) {
      m_itCurrentArea++;
      if(m_itCurrentArea == m_Areas.end()) m_itCurrentArea = m_Areas.begin();
   }

   //! \var Список загружаемых областей карты класса
   std::vector<TAreaParameters> m_Areas;
   //! \var Указатель на текущую область
   std::vector<TAreaParameters>::iterator m_itCurrentArea;

private:
   //! \var Имя плагина карты
   QString m_PluginName;
   //! \var Указатель на интерфейс карты
   qnetmap::IMapAdapterInterface* m_MapInterface;
};

///////////////////////////////////////////////////////////////
//! \class TReplyParameters
//! \brief Класс для хранения параметров http-запроса
class TReplyParameters
{
public:
   TReplyParameters() 
      : m_MapInterface(NULL), m_Url(QUrl()), m_TileFileInfo(QFileInfo()),
        m_X(0), m_Y(0), m_Zoom(0), m_Requery(false) {}
   TReplyParameters(qnetmap::IMapAdapterInterface* MapInterface_, QUrl Url_, QFileInfo TileFileInfo_,
                    int X_, int Y_, int Zoom_) 
      : m_MapInterface(MapInterface_), m_Url(Url_), m_TileFileInfo(TileFileInfo_),
        m_X(X_), m_Y(Y_), m_Zoom(Zoom_), m_Requery(false) {}
   virtual ~TReplyParameters() {}

   //! \brief Получить URL http-запроса
   //! \return URL http-запроса
   QUrl url(void) const { return m_Url; }
   //! \brief Получить новый URL http-запроса, соответственно изменив m_Url
   //!        (один и тот-же тайл можно получить из нескольких источников)
   //! \return новый URL http-запроса
   QUrl newUrl(void) 
   {
   m_Url = QUrl("http://" + m_MapInterface->host() + m_MapInterface->query(m_X, m_Y, m_Zoom));
   return m_Url; 
   }
   //! \brief Получить путь сохранения тайла
   //! \return Путь сохранения тайла
   QFileInfo tileFileInfo(void) const { return m_TileFileInfo; }
   //! \brief Получить указатель на интерфайс карты
   //! \return Указатель на интерфайс карты
   qnetmap::IMapAdapterInterface* mapInterface(void) const { return m_MapInterface; }
   //! \brief Получить номер тайла по координате X
   //! \return int - номер тайла по координате X
   int x(void) const { return m_X; }
   //! \brief Получить номер тайла по координате Y
   //! \return int - номер тайла по координате Y
   int y(void) const { return m_Y; }
   //! \brief Получить масштаб
   //! \return int - масштаб
   int zoom(void) const { return m_Zoom; }
   //! \brief Получить признак повторности запроса
   //! \return bool - true - запрос повторный
   int isRequery(void) const { return m_Requery; }
   //! \brief Установить признак повторности запроса
   //! \param bool Requery_ - признак повторности запроса 
   void setRequery(const bool Requery_ = true) { m_Requery = Requery_; }

private:
   //! \var Указатель на интерфайс карты
   qnetmap::IMapAdapterInterface* m_MapInterface;
   //! \var URL запроса
   QUrl m_Url;
   //! \var Куда необходимо сохранить тайл
   QFileInfo m_TileFileInfo;
   //! \var номер тайла по координате X
   int m_X;
   //! \var номер тайла по координате Y
   int m_Y;
   //! \var масштаб
   int m_Zoom;
   //! \var признак повторности запроса
   bool m_Requery;
};

#endif

