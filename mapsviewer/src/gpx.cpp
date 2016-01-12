#include "gpx.h"
#include "mapsviewer_translator.h"

#include "qnetmap_layer.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>

static const char *g_TagGpx  = "gpx";
static const char *g_TagWpt  = "wpt";
static const char *g_TagLat  = "lat";
static const char *g_TagLon  = "lon";
static const char *g_TagName = "name";
static const char *g_TagTrk  = "trk";
static const char *g_TagTrkseg  = "trkseg";
static const char *g_TagTrkpt  = "trkpt";
static const char *g_TagTime  = "time";

// -----------------------------------------------------------------------
bool LatitudeIsValid(double Value_)
{
return Value_ > -90.0 && Value_ < 90.0;
}

// -----------------------------------------------------------------------
bool LongitudeIsValid(double Value_)
{
return Value_ > -180.0 && Value_ < 180.0;
}

// -----------------------------------------------------------------------
bool LoadWaypoints(const QDomDocument &GPXDocument_, TWaypointsGeometry::TWaypoints &Waypoints_,
	QString &ErrorMessage_) 
{
QDomElement RootElement = GPXDocument_.documentElement();
if(RootElement.tagName() != g_TagGpx) {
	ErrorMessage_ = MapsViewerTranslator::tr("Invalid XML root element.");
	return false;
	}
	
// Initializing tags variables
QString TagLat(g_TagLat), TagLon(g_TagLon), TagName(g_TagName);
//
Waypoints_.clear();
for(QDomNode WayPointNode = RootElement.firstChild(); !WayPointNode.isNull(); 
	WayPointNode = WayPointNode.nextSibling()) {
	QDomElement WayPointElement = WayPointNode.toElement();
	if(WayPointElement.isNull() || WayPointElement.tagName() != g_TagWpt) continue;
	//
	TWaypointsGeometry::TWaypoint Waypoint;
	bool LatitudeOk, LongitudeOk;
	Waypoint.Coordinates.setY(WayPointElement.attribute(TagLat).toDouble(&LatitudeOk));
	Waypoint.Coordinates.setX(WayPointElement.attribute(TagLon).toDouble(&LongitudeOk));
	if(!LatitudeOk || !LongitudeOk || !LatitudeIsValid(Waypoint.Coordinates.y()) || 
		!LongitudeIsValid(Waypoint.Coordinates.x())) {
		ErrorMessage_ = MapsViewerTranslator::tr("Invalid GPS coordinate.");
		return false;
		}
	//
	QDomElement	NameElement = WayPointNode.firstChildElement(TagName);
	if(!NameElement.isNull()) 
		Waypoint.Name = NameElement.text();
	//
	Waypoints_.push_back(Waypoint);
	}
if(Waypoints_.empty()) {
	ErrorMessage_ = MapsViewerTranslator::tr("No waypoints data.");
	return false;
	}
return true;
}

// -----------------------------------------------------------------------
bool LoadRoute(const QDomDocument &GPXDocument_, TRouteGeometry::TRoute &Route_,
	QString &ErrorMessage_) 
{
QDomElement RootElement = GPXDocument_.documentElement();
if(RootElement.tagName() != g_TagGpx) {
	ErrorMessage_ = MapsViewerTranslator::tr("Invalid XML root element.");
	return false;
	}
	
// Initializing tags variables
QString TagLat(g_TagLat), TagLon(g_TagLon), TagTime(g_TagTime);
TRouteGeometry::TRoute Route;
//
for(QDomNode TrackNode = RootElement.firstChild(); !TrackNode.isNull(); 
	TrackNode = TrackNode.nextSibling()) {
	if(TrackNode.nodeName() != g_TagTrk) continue;
	//
	TRouteGeometry::TRouteSegment RouteSegment;
	for(QDomNode TrackSegNode = TrackNode.firstChild(); !TrackSegNode.isNull(); 
		TrackSegNode = TrackSegNode.nextSibling()) {
		if(TrackSegNode.nodeName() != g_TagTrkseg) continue;
		//
		for(QDomNode TrackPtNode = TrackSegNode.firstChild(); !TrackPtNode.isNull(); 
			TrackPtNode = TrackPtNode.nextSibling()) {
			QDomElement TrackPtElement = TrackPtNode.toElement();
			if(TrackPtElement.isNull() || TrackPtElement.tagName() != g_TagTrkpt) continue;
			//
			TRouteGeometry::TRoutePoint Point;
			bool LatitudeOk, LongitudeOk;
			Point.Coordinates.setY(TrackPtElement.attribute(TagLat).toDouble(&LatitudeOk));
			Point.Coordinates.setX(TrackPtElement.attribute(TagLon).toDouble(&LongitudeOk));
			if(!LatitudeOk || !LongitudeOk || !LatitudeIsValid(Point.Coordinates.y()) || 
				!LongitudeIsValid(Point.Coordinates.x())) {
				ErrorMessage_ = MapsViewerTranslator::tr("Invalid GPS coordinate.");
				return false;
				}
			//
			QDomElement	TimeElement = TrackPtNode.firstChildElement(TagTime);
			if(!TimeElement.isNull()) {
				QString Value = TimeElement.text().trimmed();
				Point.Time = QDateTime::fromString(Value, Qt::ISODate);
				if(Value.endsWith('Z'))
					Point.Time = Point.Time.toLocalTime();
				}
			//
			RouteSegment.push_back(Point);
			}
		}
	if(!RouteSegment.empty())
		Route.push_back(RouteSegment);
	}
if(Route.empty()) {
	ErrorMessage_ = MapsViewerTranslator::tr("No route data.");
	return false;
	}
Route_ = Route;
return true;
}

// -----------------------------------------------------------------------
bool AddGPX(qnetmap::TLayer *GeometryLayer_, const QString &FileName_, QRectF &BoundingBox_, 
	QString &ErrorMessage_)
{
assert(GeometryLayer_);

QFile GPXFile(FileName_);
if(!GPXFile.open(QIODevice::ReadOnly)) {
	ErrorMessage_ = MapsViewerTranslator::tr("Can't open file '%1'.")
		.arg(QDir::toNativeSeparators(FileName_));
	return false;
	}
QDomDocument GPXDocument;	
int ErrorLine, ErrorColumn;
QString ErrorMessage;
if(!GPXDocument.setContent(&GPXFile, &ErrorMessage, &ErrorLine, &ErrorColumn)) {
	ErrorMessage_ = MapsViewerTranslator::tr("File '%1' has error in line %2, column %3: %4.")
		.arg(QDir::toNativeSeparators(FileName_)).arg(ErrorLine).arg(ErrorColumn).arg(ErrorMessage);
	return false;
	}
	
BoundingBox_ = QRectF();
//
TRouteGeometry *RouteGeometry = NULL;
TWaypointsGeometry *WaypointsGeometry = NULL;
//
TRouteGeometry::TRoute Route;
if(LoadRoute(GPXDocument, Route, ErrorMessage_)) {
	RouteGeometry = new TRouteGeometry;
	RouteGeometry->setRoute(Route);
	RouteGeometry->setColor(Qt::green);
	GeometryLayer_->addGeometry(RouteGeometry);
	BoundingBox_ = RouteGeometry->gpsBoundingBox();
	assert(!BoundingBox_.isValid());
	}
TWaypointsGeometry::TWaypoints Waypoints;
if(LoadWaypoints(GPXDocument, Waypoints, ErrorMessage_)) {
	WaypointsGeometry = new TWaypointsGeometry;
	WaypointsGeometry->setWaypoints(Waypoints);
	WaypointsGeometry->setColor(Qt::blue);
	GeometryLayer_->addGeometry(WaypointsGeometry);
	if(BoundingBox_.isNull()) {
		BoundingBox_ = WaypointsGeometry->gpsBoundingBox();
		}
	else {	
		const QRectF &Other = WaypointsGeometry->gpsBoundingBox();
		QPointF TopLeft(qMin(BoundingBox_.topLeft().x(), Other.topLeft().x()),
			qMax(BoundingBox_.topLeft().y(), Other.topLeft().y()));
		QPointF BottomRight(qMax(BoundingBox_.bottomRight().x(), Other.bottomRight().x()),
			qMin(BoundingBox_.bottomRight().y(), Other.bottomRight().y()));
		BoundingBox_ = QRectF(TopLeft, BottomRight);
		}
	}
//
if(!RouteGeometry && !WaypointsGeometry) {
	ErrorMessage_ = MapsViewerTranslator::tr("File '%1' contains no waypoint or route data.")
		.arg(QDir::toNativeSeparators(FileName_));
	return false;
	}
return true;
}

//////////////////////////////////////////////////////////////////////////
TWaypointsGeometry::TWaypointsGeometry(void)
{
m_Pen.setWidth(WAYPOINT_PEN_WIDTH);
}

// -----------------------------------------------------------------------
const char *TWaypointsGeometry::geometryName(void)
{
return "Waypoints";
}

// -----------------------------------------------------------------------
void TWaypointsGeometry::setWaypoints(const TWaypointsGeometry::TWaypoints &Waypoints_)
{
assert(!Waypoints_.empty());
//
double MaxLatitude, MinLatitude, MaxLongitude, MinLongitude;
MaxLatitude = MinLatitude = Waypoints_.front().Coordinates.y();
MaxLongitude = MinLongitude = Waypoints_.front().Coordinates.x();
//
for(TWaypointsGeometry::TWaypoints::const_iterator it = Waypoints_.begin() + 1; 
	it != Waypoints_.end(); ++it) {
	if(MaxLatitude < it->Coordinates.y()) MaxLatitude = it->Coordinates.y();
	if(MinLatitude > it->Coordinates.y()) MinLatitude = it->Coordinates.y();
	//
	if(MaxLongitude < it->Coordinates.x()) MaxLongitude = it->Coordinates.x();
	if(MinLongitude > it->Coordinates.x()) MinLongitude = it->Coordinates.x();
	}
m_Waypoints = Waypoints_;
m_GPSBoundingBox = QRectF(QPointF(MinLongitude, MaxLatitude), QPointF(MaxLongitude, MinLatitude));
//
m_LocalWaypoints.resize(m_Waypoints.size());
// For repaint.
m_Zoom = -1;
}

// -----------------------------------------------------------------------
QRectF TWaypointsGeometry::boundingBoxF(const int Zoom_)
{
if(!w_MapAdapter) return QRectF();
//
int CurrentZoom = (Zoom_ >= 0)? Zoom_: w_MapAdapter->currentZoom();
// m_LocalWaypoints.empty() means no conversion to local coordinates is made.
if(CurrentZoom != m_Zoom) {
	m_LocalWaypoints.resize(m_Waypoints.size());
	TLocalWaypoints::iterator l_it = m_LocalWaypoints.begin();
	for(TWaypoints::const_iterator it = m_Waypoints.begin(); it != m_Waypoints.end(); 
		++it, ++l_it)
		*l_it = w_MapAdapter->coordinateToDisplay(it->Coordinates);
	// Корректируем вычисленный ранее ограничительный прямоугольник на половину ширины точек.
	QPoint TopLeft = w_MapAdapter->coordinateToDisplay(m_GPSBoundingBox.topLeft());
	TopLeft.rx() -= WAYPOINT_PEN_WIDTH / 2;
	TopLeft.ry() -= WAYPOINT_PEN_WIDTH / 2;
	QPoint BottomRight = w_MapAdapter->coordinateToDisplay(m_GPSBoundingBox.bottomRight());
	BottomRight.rx() += WAYPOINT_PEN_WIDTH / 2;
	BottomRight.ry() += WAYPOINT_PEN_WIDTH / 2;
	//
   m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(TopLeft),
		w_MapAdapter->displayToCoordinate(BottomRight)).normalized();
	boundingBox(CurrentZoom);
   m_Zoom = CurrentZoom;
   }
return m_BoundingBoxF;
}

// -----------------------------------------------------------------------
void TWaypointsGeometry::draw(QPainter *Painter_, const QRect &Viewport_, const QPoint Offset_)
{
Q_UNUSED(Offset_)

if(!isVisible() || !w_MapAdapter) return;

// Recalculating only if zoom changes
int CurrentZoom = w_MapAdapter->currentZoom();
if(CurrentZoom != m_Zoom) {
	boundingBoxF(CurrentZoom);
   m_Zoom = CurrentZoom;
   }
Painter_->setRenderHint(QPainter::Antialiasing, true);
if(m_LocalWaypoints.empty()) return;
//
for(TLocalWaypoints::const_iterator it = m_LocalWaypoints.begin(); 
	it != m_LocalWaypoints.end(); ++it) {
	if(!Viewport_.contains(*it)) continue;
	//
	Painter_->setPen(m_Pen);
	Painter_->drawPoint(*it);
	}
}

// -----------------------------------------------------------------------
QString TWaypointsGeometry::description(const QPointF Point_) const
{
const qreal AREA_SIZE = 6;
//
assert(m_LocalWaypoints.size() == m_Waypoints.size());
QPoint ScreenPoint = w_MapAdapter->coordinateToDisplay(Point_);

for(TLocalWaypoints::const_iterator it = m_LocalWaypoints.begin(); it != m_LocalWaypoints.end(); 
	++it) {
	if(qAbs(it->x() - ScreenPoint.x()) > AREA_SIZE / 2 ||
		qAbs(it->y() - ScreenPoint.y()) > AREA_SIZE / 2)
		continue;
	//
	int Offset = it - m_LocalWaypoints.begin();
	return m_Waypoints[Offset].Name;
	}
return QString();
}

//////////////////////////////////////////////////////////////////////////
TRouteGeometry::TRouteGeometry(void)
{
m_Pen.setWidth(POINT_PEN_WIDTH);
m_LinePen.setWidth(LINE_PEN_WIDTH);
}

// -----------------------------------------------------------------------
const char* TRouteGeometry::geometryName(void)
{
return "Route";
}

// -----------------------------------------------------------------------
void TRouteGeometry::setRoute(const TRouteGeometry::TRoute &Route_)
{
assert(!Route_.empty());
#ifdef _DEBUG
	for(TRoute::const_iterator r_it = Route_.begin(); r_it != Route_.end(); ++r_it) {
		assert(r_it->size() > 1);
		}
#endif

m_Route = Route_;
//
double MaxLatitude = -100, MinLatitude = 100, MaxLongitude = -200, MinLongitude = 200;
//
for(TRoute::const_iterator r_it = m_Route.begin(); r_it != m_Route.end(); ++r_it) {
	for(TRouteSegment::const_iterator it = r_it->begin(); it != r_it->end(); ++it) {
		if(MaxLatitude < it->Coordinates.y()) MaxLatitude = it->Coordinates.y();
		if(MinLatitude > it->Coordinates.y()) MinLatitude = it->Coordinates.y();
		//
		if(MaxLongitude < it->Coordinates.x()) MaxLongitude = it->Coordinates.x();
		if(MinLongitude > it->Coordinates.x()) MinLongitude = it->Coordinates.x();
		}
	}
m_GPSBoundingBox = QRectF(QPointF(MinLongitude, MaxLatitude), QPointF(MaxLongitude, MinLatitude));
// For repaint.
m_Zoom = -1;
}

// -----------------------------------------------------------------------
QRectF TRouteGeometry::boundingBoxF(const int Zoom_)
{
if(!w_MapAdapter) return QRectF();
//
int CurrentZoom = (Zoom_ >= 0)? Zoom_: w_MapAdapter->currentZoom();
if(CurrentZoom != m_Zoom) {
	for(TRoute::iterator r_it = m_Route.begin(); r_it != m_Route.end(); ++r_it) {
		for(TRouteSegment::iterator it = r_it->begin(); it != r_it->end(); ++it) {
			it->ScreenCoordinates = w_MapAdapter->coordinateToDisplay(it->Coordinates);
			}
		}
	// Корректируем вычисленный ранее ограничительный прямоугольник на половину ширины точек.
	QPoint TopLeft = w_MapAdapter->coordinateToDisplay(m_GPSBoundingBox.topLeft());
	TopLeft.rx() -= WAYPOINT_PEN_WIDTH / 2;
	TopLeft.ry() -= WAYPOINT_PEN_WIDTH / 2;
	QPoint BottomRight = w_MapAdapter->coordinateToDisplay(m_GPSBoundingBox.bottomRight());
	BottomRight.rx() += WAYPOINT_PEN_WIDTH / 2;
	BottomRight.ry() += WAYPOINT_PEN_WIDTH / 2;
	//
   m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(TopLeft),
		w_MapAdapter->displayToCoordinate(BottomRight)).normalized();
	boundingBox(CurrentZoom);
   m_Zoom = CurrentZoom;
   }
return m_BoundingBoxF;
}

// -----------------------------------------------------------------------
void TRouteGeometry::draw(QPainter *Painter_, const QRect &Viewport_, const QPoint Offset_)
{
Q_UNUSED(Offset_)

if(!isVisible() || !w_MapAdapter) return;

// Recalculating only if zoom changes
int CurrentZoom = w_MapAdapter->currentZoom();
if(CurrentZoom != m_Zoom) {
	boundingBoxF(CurrentZoom);
   m_Zoom = CurrentZoom;
   }
Painter_->setRenderHint(QPainter::Antialiasing, true);
if(m_Route.empty()) return;
//
for(TRoute::const_iterator r_it = m_Route.begin(); r_it != m_Route.end(); ++r_it) {
	for(TRouteSegment::const_iterator it = r_it->begin() + 1; it != r_it->end(); ++it) {
		TRouteSegment::const_iterator prev_it = it - 1;
		if(Viewport_.contains(it->ScreenCoordinates) || 
			Viewport_.contains(prev_it->ScreenCoordinates)) {
			//
			Painter_->setPen(m_LinePen);
			Painter_->drawLine(it->ScreenCoordinates, prev_it->ScreenCoordinates);
			//
			Painter_->setPen(m_Pen);
			Painter_->drawPoint(it->ScreenCoordinates);
			Painter_->drawPoint(prev_it->ScreenCoordinates);
			}
		}
	}
}

// -----------------------------------------------------------------------
QString TRouteGeometry::description(const QPointF Point_) const
{
const qreal AREA_SIZE = 6;
//
QPoint ScreenPoint = w_MapAdapter->coordinateToDisplay(Point_);

for(TRoute::const_iterator r_it = m_Route.begin(); r_it != m_Route.end(); ++r_it) {
	for(TRouteSegment::const_iterator it = r_it->begin(); it != r_it->end(); ++it) {
		if(qAbs(it->ScreenCoordinates.x() - ScreenPoint.x()) > AREA_SIZE / 2 ||
			qAbs(it->ScreenCoordinates.y() - ScreenPoint.y()) > AREA_SIZE / 2)
			continue;
		//
		return it->Time.toString();
		}
	}
return QString();
}
