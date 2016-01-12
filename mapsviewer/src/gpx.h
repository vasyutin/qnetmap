#ifndef __qnetmap_gpx_h
#define __qnetmap_gpx_h

#include "qnetmap_geometry.h"

#include <QPen>
#include <QDateTime>

// -----------------------------------------------------------------------
// The object to show way points
// -----------------------------------------------------------------------
class TWaypointsGeometry: public qnetmap::TGeometry {
public:
	static const int WAYPOINT_PEN_WIDTH = 2;
	
	struct TWaypoint {
		QPointF Coordinates;
		QString Name;
		};
	typedef QList<TWaypoint> TWaypoints;
	
	TWaypointsGeometry(void);
   static const char *geometryName(void);
   QString description(const QPointF Point_) const;
   //
	const TWaypoints &waypoints(void) const {return m_Waypoints;}
	void setWaypoints(const TWaypoints &Waypoints_);
	//
	QColor color(void) const {return m_Pen.color();}
	void setColor(const QColor &Color_) {m_Pen.setColor(Color_);}
	//
	QRectF boundingBoxF(const int Zoom_ = -1);
	const QRectF& gpsBoundingBox(void) const {return m_GPSBoundingBox;}
   
protected:
	void draw(QPainter *Painter_, const QRect &Viewport_, const QPoint Offset_);

private:
	Q_DISABLE_COPY(TWaypointsGeometry)

	// Original waypoints
	TWaypoints m_Waypoints;
	// Waypoints in local coordinates.
	typedef QVector<QPoint> TLocalWaypoints;
	TLocalWaypoints m_LocalWaypoints;
	//	
	QRectF m_GPSBoundingBox;
	QPen m_Pen;
	};

// -----------------------------------------------------------------------
// The object to show routes
// -----------------------------------------------------------------------
class TRouteGeometry: public TWaypointsGeometry {
public:
	static const int POINT_PEN_WIDTH = 4;
	static const int LINE_PEN_WIDTH = 2;

	struct TRoutePoint {
		QDateTime Time;
		QPointF Coordinates;
		QPoint ScreenCoordinates;
		};
	typedef QList<TRoutePoint> TRouteSegment;
	typedef QList<TRouteSegment> TRoute;

	TRouteGeometry(void);
   static const char *geometryName(void);
   QString description(const QPointF Point_) const;
   //
	const TRoute &route(void) const {return m_Route;}
	void setRoute(const TRoute &Waypoints_);
	//
	QColor color(void) const {return m_Pen.color();}
	void setColor(const QColor &Color_) {
		m_Pen.setColor(Color_);
		m_LinePen.setColor(Color_);
		}
	//
	QRectF boundingBoxF(const int Zoom_ = -1);
	const QRectF& gpsBoundingBox(void) const {return m_GPSBoundingBox;}

protected:
	void draw(QPainter *Painter_, const QRect &Viewport_, const QPoint Offset_);

private:
	Q_DISABLE_COPY(TRouteGeometry)
	//
	TRoute m_Route;
	QRectF m_GPSBoundingBox;
	QPen m_Pen;
	QPen m_LinePen;
	};

// -----------------------------------------------------------------------
bool AddGPX(qnetmap::TLayer *GeometryLayer_, const QString &FileName_, QRectF &BoundingBox_,
	QString &ErrorMessage_);

#endif // #ifndef __qnetmap_gpx_h