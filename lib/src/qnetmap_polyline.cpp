#include <QPainter>
#include <iostream>

#include "qnetmap_polyline.h"
#include "qnetmap_mapadapter.h"
#include "qnetmap_widget.h"
#include "qnetmap_layer.h"
#include "qnetmap_layermanager.h"

#define POINTSQUAREHALFHEIGHT 4

namespace qnetmap
{
   TPolyLine::TPolyLine(const TCoordinatesList& Points_, const QString& Name_) 
      : TGeometry(Name_), m_Type(PolyLinePolygonalType), m_Color(0xff000000), m_OutlineColor(0x80ffffff), m_Thickness(1),
        m_LastTouchesSegment(-1), m_LastTouchesPoint(-1), m_Closed(false)
   {
      setGeometryType("PolyLine");
      setPoints(Points_, false);
   }

   TPolyLine::~TPolyLine()
   {
      foreach(TPolyLinePoint* Point, m_Points) delete Point;
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::addPoint( QPointF const& Point_, const bool SendUpdate /*= true*/, const bool SetModified_ /*= true*/ )
   {
      m_Points.push_back(new TPolyLinePoint(this, Point_));
      if(SetModified_) setModified();
      if(SendUpdate) update();
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::addBeginPoint( QPointF const& Point_, const bool SendUpdate /*= true*/, const bool SetModified_ /*= true*/ )
   {
      m_Points.push_front(new TPolyLinePoint(this, Point_));
      if(SetModified_) setModified();
      if(SendUpdate) update();
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::insertPoint( QPointF const& Point_, const unsigned long PointNumber_, const bool SendUpdate /*= true*/, const bool SetModified_ /*= true*/ )
   {
      if((int)PointNumber_ < (m_Points.count() - 1)) {
         m_Points.insert(PointNumber_ + 1, new TPolyLinePoint(this, Point_));
         if(SetModified_) setModified();
         if(SendUpdate) update();
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::deletePoint( const unsigned long PointNumber_, const bool SendUpdate /*= true*/, const bool SetModified_ /*= true*/ )
   {
      if(PointNumber_ > 0 && (int)PointNumber_ < (m_Points.count() - 1)) {
         m_Points.removeAt(PointNumber_);
         if(SetModified_) setModified();
         if(SendUpdate) update();
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setPoints( TCoordinatesList const& Points_, const bool SetModified_ )
   {
      if(m_Points.count()) foreach(TPolyLinePoint* Point, m_Points) delete Point;
      foreach(QPointF Point, Points_) addPoint(Point, true, SetModified_);
   }

   //-------------------------------------------------------------------------------------
   TPolyLine::TPolyLinePoints& TPolyLine::points()
   {
   return m_Points;
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
      if(!visible() || !w_MapAdapter) return;

      bool EditMode = false;
      if(layer()->layerManager()) EditMode = layer()->layerManager()->mapControl()->editMode();

      QPainterPath PainterPath;
      switch(type()) {
      case PolyLinePolygonalType:  polygonalPath(PainterPath);  break;
      case PolyLineTCBSplineType:  splinePath(PainterPath);     break;
      case PolyLineOrthogonalType: orthogonalPath(PainterPath); break;
      default:
         assert(!"TPolyline::drawGeometry: Polyline type don't known");
         return;
      }
      QRgb Color;
      // draw outline
      Color = m_OutlineColor;
      if(geometryType() == "PolyLine" && !focused() && !selected() && qAlpha(Color) > 0) {
         QPen Pen(QColor(qRed(Color), qGreen(Color), qBlue(Color), qAlpha(Color)), m_Thickness + SELECTION_PADDING * 2);
         Pen.setCapStyle(Qt::RoundCap);
         Pen.setJoinStyle(Qt::RoundJoin);
         Painter_->strokePath(PainterPath, Pen);
      }
      // draw polyline
      Color = m_Color;
      QPen Pen(QColor(qRed(Color), qGreen(Color), qBlue(Color), qAlpha(Color)), m_Thickness);
      Pen.setStyle(penStyle());
      Pen.setCapStyle(Qt::RoundCap);
      Pen.setJoinStyle(Qt::RoundJoin);
      Painter_->strokePath(PainterPath, Pen);
      // draw points and control points in edit mode
      if(EditMode && focused()) {
         QRgb MainColorPoint = TMapWidget::mainColorPointsInEditMode();
         QRgb SecondaryColorPoint = TMapWidget::secondaryColorPointsInEditMode();
         QPen Pen(QColor(MainColorPoint), 2);
         Pen.setJoinStyle(Qt::MiterJoin);
         // draw points
         QPainterPath PointsPath;
         pointsPath(PointsPath);
         Painter_->strokePath(PointsPath, Pen);
         // if spline, draw control points and lines to it
         if(type() == PolyLineTCBSplineType) {
            unsigned PointsCount = m_Points.count();
            if(PointsCount < 2) return;
            //
            QPen OldPen = Painter_->pen();
            QPen Pen(QColor(MainColorPoint), 1);
            Painter_->setPen(Pen);
            Painter_->setBrush(QBrush(QColor(MainColorPoint)));
            // draw lines to control points
            QPoint CurrentPoint = w_MapAdapter->coordinateToDisplay(m_Points[0]->coordinate());
            QPoint NextPoint    = w_MapAdapter->coordinateToDisplay(m_Points[1]->coordinate());
            //
            QPoint PreviousPoint;
            if(closed()) PreviousPoint = w_MapAdapter->coordinateToDisplay(m_Points[PointsCount - 1]->coordinate());
            else         PreviousPoint = 1.4 * CurrentPoint - 0.4 * NextPoint;
            //
            TFactors Factors = m_Points[0]->factors();
            for(unsigned i = 1; i <= PointsCount; ++i) {
               TPolyLinePoint* Point;
               if(i == PointsCount) {
                  if(closed()) {
                     Point = m_Points[0];
                     NextPoint = w_MapAdapter->coordinateToDisplay(Point->coordinate());
                  }
                  else NextPoint = 1.4 * CurrentPoint - 0.4 * PreviousPoint;
               }
               else {
                  Point = m_Points[i];
                  NextPoint = w_MapAdapter->coordinateToDisplay(Point->coordinate());
               }
               //
               if(m_Points[i - 1]->editState() == PointEditStateContinuity) 
                    Pen = QPen(QColor(SecondaryColorPoint), 1);
               else Pen = QPen(QColor(MainColorPoint), 1);
               Painter_->setPen(Pen);
               //
               QPoint TangentIn  = CurrentPoint  - NextPoint;
               QPoint TangentOut = PreviousPoint - CurrentPoint;
               if(i > 1 || closed()) {
                  QPoint CP(CurrentPoint + Factors.fIn1  * TangentIn + Factors.fIn2  * TangentOut);
                  Painter_->drawLine(CurrentPoint, CP);
               }
               if(i < PointsCount || closed()) {
                  QPoint CP(CurrentPoint - Factors.fOut1 * TangentIn - Factors.fOut2 * TangentOut);
                  Painter_->drawLine(CurrentPoint, CP);
               }
               //
               PreviousPoint = CurrentPoint;
               CurrentPoint = NextPoint;
               Factors = Point->factors();
            }
            Painter_->setPen(QPen(QColor((unsigned)0x00000000), 1));
            // draw control points
            QPainterPath ControlPointsPath;
            controlPointsPath(ControlPointsPath);
            Painter_->drawPath(ControlPointsPath);
            Painter_->setPen(OldPen);
         }
      }
   }

   //---------------------------------------------------------------------------
   void TPolyLine::drawSelection(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_)
   {
      if(!visible() || !w_MapAdapter) return;

      QRgb Color;
      if(focused())       Color = TMapWidget::colorFocused();
      else if(selected()) Color = TMapWidget::colorSelected();

      QPainterPath PainterPath;
      switch(type()) {
      case PolyLinePolygonalType:  polygonalPath(PainterPath);  break;
      case PolyLineTCBSplineType:  splinePath(PainterPath);     break;
      case PolyLineOrthogonalType: orthogonalPath(PainterPath); break;
      default:
         assert(!"TPolyline::drawSelection: Polyline type don't known");
         return;
      }
      // draw selection
      QPen Pen(QColor(qRed(Color), qGreen(Color), qBlue(Color), qAlpha(Color)), m_Thickness + SELECTION_PADDING * 2);
      Pen.setCapStyle(Qt::RoundCap);
      Pen.setJoinStyle(Qt::RoundJoin);
      Painter_->strokePath(PainterPath, Pen);
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::polygonalPath(QPainterPath& PainterPath_, const bool ConvertToDisplay_ /*= true*/)
   {
      bool First = true;
      QPointF Point;
      foreach(TPolyLinePoint* PolyLinePoint, m_Points) {
         Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay(PolyLinePoint->coordinate()) : PolyLinePoint->coordinate();
         if(First) {
            PainterPath_.moveTo(Point);
            First = false;
         }
         else PainterPath_.lineTo(Point);
      }
      if(closed()) {
         Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*m_Points.begin())->coordinate()) : (*m_Points.begin())->coordinate();
         PainterPath_.lineTo(Point);
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::splinePath(QPainterPath& PainterPath_, const bool ConvertToDisplay_ /*= true*/)
   {
      bool First = true;
      TPolyLinePoints Points = m_Points;
      unsigned PointsCount = m_Points.count();
      if(PointsCount < 2) return;
      //
      TPolyLinePoint BeginFictionPoint, EndFictionPoint;
      if(closed()) {
         TPolyLinePoint* Point;
         // add the fiction point to begin
         Point = m_Points[PointsCount - 1];
         BeginFictionPoint.setTensionContinuityBias(Point->tension(), Point->continuity(), Point->bias());
         BeginFictionPoint.setCoordinate(Point->coordinate());
         Points.push_front(&BeginFictionPoint);
         // add the fiction point to end
         Point = m_Points[0];
         EndFictionPoint.setTensionContinuityBias(Point->tension(), Point->continuity(), Point->bias());
         EndFictionPoint.setCoordinate(Point->coordinate());
         Points.push_back(&EndFictionPoint);
      }
      else {
         // add the fiction point to begin
         TPolyLinePoint *Point1 = m_Points[0], *Point2 = m_Points[1];
         QPointF PointF = 2 * Point1->coordinate() - Point2->coordinate();
         BeginFictionPoint.setCoordinate(PointF);
         Points.push_front(&BeginFictionPoint);
         // add the fiction point to end
         Point1 = m_Points[PointsCount - 2], Point2 = m_Points[PointsCount - 1];
         PointF = 2 * Point2->coordinate() - Point1->coordinate();
         EndFictionPoint.setCoordinate(PointF);
         Points.push_back(&EndFictionPoint);
      }
      //
      TPolyLinePoints::const_iterator it = Points.begin();
      QPointF PreviousPoint = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*it)->coordinate()) : (*it)->coordinate();
      ++it;
      QPointF CurrentPoint1 = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*it)->coordinate()) : (*it)->coordinate();
      TFactors CP1Factors = (*it)->factors();
      ++it;
      QPointF CurrentPoint2 = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*it)->coordinate()) : (*it)->coordinate();
      TFactors CP2Factors = (*it)->factors();
      QPointF NextPoint;
      unsigned CurvsCount = m_Points.count() - 1;
      for(unsigned i = 0; i < CurvsCount; ++i) {
         ++it;
         NextPoint = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*it)->coordinate()) : (*it)->coordinate();
         TFactors NPFactors = (*it)->factors();
         //
         QPointF TangentIn((CP1Factors.fIn1 * (CurrentPoint2 - CurrentPoint1) + CP1Factors.fIn2 * (CurrentPoint1 - PreviousPoint)) / 2.);
         QPointF TangentOut((CP2Factors.fOut1 * (NextPoint - CurrentPoint2) + CP2Factors.fOut2 * (CurrentPoint2 - CurrentPoint1)) / 2.);
         if(First) {
            PainterPath_.moveTo(CurrentPoint1);
            First = false;
         }
         PainterPath_.cubicTo(CurrentPoint1 + TangentIn, CurrentPoint2 - TangentOut, CurrentPoint2);
         PreviousPoint = CurrentPoint1;
         CurrentPoint1 = CurrentPoint2;
         CurrentPoint2 = NextPoint;
         CP1Factors = CP2Factors;
         CP2Factors = NPFactors;
      }
      if(closed()) {
         TPolyLinePoint* Point = m_Points[1];
         NextPoint = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay(Point->coordinate()) : Point->coordinate();
         TFactors NPFactors = Point->factors();
         //
         QPointF TangentIn((CP1Factors.fIn1 * (CurrentPoint2 - CurrentPoint1) + CP1Factors.fIn2 * (CurrentPoint1 - PreviousPoint)) / 2.);
         QPointF TangentOut((CP2Factors.fOut1 * (NextPoint - CurrentPoint2) + CP2Factors.fOut2 * (CurrentPoint2 - CurrentPoint1)) / 2.);
         if(First) {
            PainterPath_.moveTo(CurrentPoint1);
            First = false;
         }
         PainterPath_.cubicTo(CurrentPoint1 + TangentIn, CurrentPoint2 - TangentOut, CurrentPoint2);
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::orthogonalPath( QPainterPath& PainterPath_, const bool ConvertToDisplay_ /*= true*/ )
   {
      bool First = true;
      QPointF PreviousPoint, Point;
      //
      foreach(TPolyLinePoint* PolyLinePoint, m_Points) {
         Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay(PolyLinePoint->coordinate()) : PolyLinePoint->coordinate();
         if(First) {
            PainterPath_.moveTo(Point);
            First = false;
         }
         else {
            qreal X  = (PreviousPoint.x() + Point.x()) / 2.;
            qreal Y1 = PreviousPoint.y();
            qreal Y2 = Point.y();
            //
            QPointF MiddlePoint1(X, Y1);
            QPointF MiddlePoint2(X, Y2);
            //
            PainterPath_.lineTo(MiddlePoint1);
            PainterPath_.lineTo(MiddlePoint2);
            PainterPath_.lineTo(Point);
         }
         //
         PreviousPoint = Point;
      }
      if(closed()) {
         Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay((*m_Points.begin())->coordinate()) : (*m_Points.begin())->coordinate();
         qreal X  = (PreviousPoint.x() + Point.x()) / 2.;
         qreal Y1 = PreviousPoint.y();
         qreal Y2 = Point.y();
         //
         QPointF MiddlePoint1(X, Y1);
         QPointF MiddlePoint2(X, Y2);
         //
         PainterPath_.lineTo(MiddlePoint1);
         PainterPath_.lineTo(MiddlePoint2);
         PainterPath_.lineTo(Point);
      }
   }

   //-------------------------------------------------------------------------------------
   int TPolyLine::pointsCount() const
   {
      return m_Points.count();
   }

   //-------------------------------------------------------------------------------------
   bool TPolyLine::touches(const QPointF Point_)
   {
      m_LastTouchesSegment = -1;
      m_LastTouchesPoint = -1;
      bool RetValue = TParent::touches(Point_);
      //
      if(RetValue) {
         // find to point touches
         unsigned PointNumber = 0;
         foreach(TPolyLinePoint* Point, m_Points) {
            QPainterPath PointPath;
            pointPath(PointPath, Point->coordinate(), false);
            if(PointPath.contains(Point_)) {
               m_LastTouchesPoint = PointNumber;
               break;
            }
            ++PointNumber;
         }
         //
         if(m_LastTouchesPoint < 0) {
            // find touches segment number
            unsigned long PolyLineType = type();
            //
            QPainterPath PainterPath;
            if(painterPath(PainterPath, PolyLineType, false)) {
               // iterate segments
               unsigned SegmentsCount = m_Points.count() - 1;
               if(SegmentsCount > 0) {
                  for(unsigned SegmentNumber = 0; SegmentNumber < SegmentsCount; ++SegmentNumber) {
                     if(strokePath(subPath(PainterPath, PolyLineType, SegmentNumber)).contains(Point_)) {
                        m_LastTouchesSegment = SegmentNumber;
                        break;
                     }
                  }
               }
            }
         }
      }
      
      return RetValue;
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TPolyLine::subPath(const QPainterPath& PainterPath_, const unsigned long LineType_, const unsigned SegmentNumber_) {
      QPainterPath SegmentPainterPath;
      //
      switch(LineType_) {
         case PolyLinePolygonalType:
            if((int)SegmentNumber_ < PainterPath_.elementCount()) {
               QPainterPath::Element FirstElement(PainterPath_.elementAt(SegmentNumber_));
               QPointF FirstPoint(QPointF(FirstElement.x, FirstElement.y));
               QPainterPath::Element LastElement(PainterPath_.elementAt(SegmentNumber_ + 1));
               QPointF LastPoint(QPointF(LastElement.x, LastElement.y));
               SegmentPainterPath.moveTo(FirstPoint);
               SegmentPainterPath.lineTo(LastPoint);
            }
            break;
         case PolyLineTCBSplineType:
            {
               unsigned FirstElementNumber = SegmentNumber_ * 3;
               if((int)FirstElementNumber < (PainterPath_.elementCount() - 3)) {
                  QPainterPath::Element FirstElement(PainterPath_.elementAt(FirstElementNumber));
                  QPointF FirstPoint(QPointF(FirstElement.x, FirstElement.y));
                  QPainterPath::Element SecondElement(PainterPath_.elementAt(FirstElementNumber + 1));
                  QPointF SecondPoint(QPointF(SecondElement.x, SecondElement.y));
                  QPainterPath::Element ThirdElement(PainterPath_.elementAt(FirstElementNumber + 2));
                  QPointF ThirdPoint(QPointF(ThirdElement.x, ThirdElement.y));
                  QPainterPath::Element LastElement(PainterPath_.elementAt(FirstElementNumber + 3));
                  QPointF LastPoint(QPointF(LastElement.x, LastElement.y));
                  SegmentPainterPath.moveTo(FirstPoint);
                  SegmentPainterPath.cubicTo(SecondPoint, ThirdPoint, LastPoint);
               }
            }
            break;
         case PolyLineOrthogonalType: 
            {
               unsigned FirstElementNumber = SegmentNumber_ * 3;
               if((int)FirstElementNumber < (PainterPath_.elementCount() - 3)) {
                  QPainterPath::Element FirstElement(PainterPath_.elementAt(FirstElementNumber));
                  QPointF FirstPoint(QPointF(FirstElement.x, FirstElement.y));
                  QPainterPath::Element SecondElement(PainterPath_.elementAt(FirstElementNumber + 1));
                  QPointF SecondPoint(QPointF(SecondElement.x, SecondElement.y));
                  QPainterPath::Element ThirdElement(PainterPath_.elementAt(FirstElementNumber + 2));
                  QPointF ThirdPoint(QPointF(ThirdElement.x, ThirdElement.y));
                  QPainterPath::Element LastElement(PainterPath_.elementAt(FirstElementNumber + 3));
                  QPointF LastPoint(QPointF(LastElement.x, LastElement.y));
                  SegmentPainterPath.moveTo(FirstPoint);
                  SegmentPainterPath.lineTo(SecondPoint);
                  SegmentPainterPath.lineTo(ThirdPoint);
                  SegmentPainterPath.lineTo(LastPoint);
               }
            }
            break;
      }
      //
      return SegmentPainterPath;
   }

   //-------------------------------------------------------------------------------------
   bool TPolyLine::painterPath(QPainterPath& PainterPath_, const unsigned long LineType_, const bool ConvertToDisplay_ /*= true*/)
   {
      bool RetValue = true;

      switch(LineType_) {
         case PolyLinePolygonalType:  polygonalPath(PainterPath_, ConvertToDisplay_);  break;
         case PolyLineTCBSplineType:  splinePath(PainterPath_, ConvertToDisplay_);     break;
         case PolyLineOrthogonalType: orthogonalPath(PainterPath_, ConvertToDisplay_); break;
         default:
            assert(!"TPolyline::painterPath: Polyline type don't known");
            RetValue = false;
      }

      return RetValue;
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TPolyLine::strokePath(const QPainterPath& PainterPath_)
   {
      QPainterPathStroker Stroker;
      /// convert width in pixels to width in degree and set to stroker
      Stroker.setWidth(w_MapAdapter->displayToCoordinate(QPoint(m_Thickness + SELECTION_PADDING * 2, 0)).x() + 180.);
      Stroker.setJoinStyle(Qt::RoundJoin);
      Stroker.setCapStyle(Qt::RoundCap);
      return Stroker.createStroke(PainterPath_);
   }

   //-------------------------------------------------------------------------------------
   QPainterPath TPolyLine::painterPath(void)
   {
      bool EditMode = layer()->layerManager()->mapControl()->editMode();
      unsigned long PolyLineType = type();
      //
      QPainterPath PainterPath;
      if(!painterPath(PainterPath, PolyLineType, false)) return PainterPath;
      // increasing the width of the line
      QPainterPath StrokedPath = strokePath(PainterPath);

      // add paths for points and control points
      if(EditMode) {
         pointsPath(StrokedPath, false);
         if(PolyLineType == PolyLineTCBSplineType) controlPointsPath(StrokedPath, false);
      }
      //
      return StrokedPath;
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::pointPath( QPainterPath& PointPath_, const QPointF& Point_, const bool ConvertToDisplay_ /*= true*/ )
   {
      static QPoint DisplayShift(POINTSQUAREHALFHEIGHT, POINTSQUAREHALFHEIGHT);
      QPointF Shift;
      if(ConvertToDisplay_) Shift = DisplayShift;
      else {
         qreal ShiftX = w_MapAdapter->displayToCoordinate(DisplayShift).x() + 180.;
         Shift = QPointF(ShiftX, ShiftX);
      } 
      QPointF Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay(Point_) : Point_;
      PointPath_.addRect(QRectF(Point - Shift, Point + Shift));
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::controlPointPath( QPainterPath& PointPath_, const QPointF& Point_, const bool ConvertToDisplay_ /*= true*/ )
   {
      static QPoint DisplayShift(POINTSQUAREHALFHEIGHT, POINTSQUAREHALFHEIGHT);
      QPointF Shift;
      if(ConvertToDisplay_) Shift = DisplayShift;
      else {
         qreal ShiftX = w_MapAdapter->displayToCoordinate(DisplayShift).x() + 180.;
         Shift = QPointF(ShiftX, ShiftX);
      } 
      QPointF Point = ConvertToDisplay_ ? w_MapAdapter->coordinateToDisplay(Point_) : Point_;
      PointPath_.addEllipse(QRectF(Point - Shift, Point + Shift));
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::pointsPath( QPainterPath& PointsPath_, const bool ConvertToDisplay_ /*= true*/ )
   {
      foreach(TPolyLinePoint* PolyLinePoint, m_Points) {
         pointPath(PointsPath_, PolyLinePoint->coordinate(), ConvertToDisplay_);
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::controlPointsPath( QPainterPath& ControlPointsPath_, const bool ConvertToDisplay_ /*= true*/ )
   {
      unsigned PointsCount = m_Points.count();
      if(PointsCount < 2) return;
      //
      QPointF CurrentPoint  = m_Points[0]->coordinate();
      QPointF NextPoint     = m_Points[1]->coordinate();
      QPointF PreviousPoint;
      if(closed()) PreviousPoint = m_Points[PointsCount - 1]->coordinate();
      else         PreviousPoint = 1.4 * CurrentPoint - 0.4 * NextPoint;
      //
      TFactors Factors = m_Points[0]->factors();
      for(unsigned i = 1; i <= PointsCount; ++i) {
         TPolyLinePoint* Point;
         if(i == PointsCount) {
            if(closed()) {
               Point = m_Points[0];
               NextPoint = Point->coordinate();
            }
            else NextPoint = 1.4 * CurrentPoint - 0.4 * PreviousPoint;
         }
         else {
            Point = m_Points[i];
            NextPoint = Point->coordinate();
         }
         //
         QPointF TangentIn  = NextPoint    - CurrentPoint;
         QPointF TangentOut = CurrentPoint - PreviousPoint;
         if(i > 1 || closed()) {
            QPointF CP(CurrentPoint - Factors.fIn1  * TangentIn - Factors.fIn2  * TangentOut);
            controlPointPath(ControlPointsPath_, CP, ConvertToDisplay_);
         }
         if(i < PointsCount || closed()) {
            QPointF CP(CurrentPoint + Factors.fOut1 * TangentIn + Factors.fOut2 * TangentOut);
            controlPointPath(ControlPointsPath_, CP, ConvertToDisplay_);
         }
         //
         PreviousPoint = CurrentPoint;
         CurrentPoint = NextPoint;
         Factors = Point->factors();
      }
   }

   //---------------------------------------------------------------------------
   bool TPolyLine::touchesPoints(const QPointF Point_)
   {
      QPainterPath PointsPath;
      pointsPath(PointsPath, false);
      return PointsPath.contains(Point_);
   }

   //---------------------------------------------------------------------------
   bool TPolyLine::touchesControlPoints(const QPointF Point_)
   {
      QPainterPath PointsPath;
      controlPointsPath(PointsPath, false);
      return PointsPath.contains(Point_);
   }

   //---------------------------------------------------------------------------
   TPolyLine::TPointParameters TPolyLine::touchesPoint(const QPointF Point_)
   {
      TPointParameters PointParameters;
      //
      long i = 0;
      foreach(TPolyLinePoint* PolyLinePoint, m_Points) {
         QPainterPath PointPath;
         pointPath(PointPath, PolyLinePoint->coordinate(), false);
         if(PointPath.contains(Point_)) {
            PointParameters.Coordinate = PolyLinePoint->coordinate();
            PointParameters.PointNumber = i;
            PointParameters.PointType = PolyLineNodePoint;
            break;
         }
         ++i;
      }
      //
      return PointParameters;
   }

   //---------------------------------------------------------------------------
   TPolyLine::TPointParameters TPolyLine::touchesControlPoint(const QPointF Point_)
   {
      TPointParameters PointParameters;
      //
      unsigned PointsCount = m_Points.count();
      if(PointsCount < 2) return PointParameters;
      //
      QPointF CurrentPoint  = m_Points[0]->coordinate();
      QPointF NextPoint     = m_Points[1]->coordinate();
      QPointF PreviousPoint;
      if(closed()) PreviousPoint = m_Points[PointsCount - 1]->coordinate();
      else         PreviousPoint = 1.4 * CurrentPoint - 0.4 * NextPoint;
      //
      TFactors Factors = m_Points[0]->factors();
      for(unsigned i = 1; i <= PointsCount; ++i) {
         TPolyLinePoint* Point;
         if(i == PointsCount) {
            if(closed()) {
               Point = m_Points[0];
               NextPoint = Point->coordinate();
            }
            else NextPoint = 1.4 * CurrentPoint - 0.4 * PreviousPoint;
         }
         else {
            Point = m_Points[i];
            NextPoint = Point->coordinate();
         }
         //
         QPointF TangentIn  = CurrentPoint  - NextPoint;
         QPointF TangentOut = PreviousPoint - CurrentPoint;
         if(i > 1 || closed()) {
            QPainterPath ControlPointPath;
            QPointF CP(CurrentPoint + Factors.fIn1  * TangentIn + Factors.fIn2  * TangentOut);
            controlPointPath(ControlPointPath, CP, false);
            if(ControlPointPath.contains(Point_)) {
               PointParameters.Coordinate  = CP;
               PointParameters.PointNumber = i - 1;
               PointParameters.PointType   = PolyLineInputPoint;
               break;
            }
         }
         if(i < PointsCount || closed()) {
            QPainterPath ControlPointPath;
            QPointF CP(CurrentPoint - Factors.fOut1 * TangentIn - Factors.fOut2 * TangentOut);
            controlPointPath(ControlPointPath, CP, false);
            if(ControlPointPath.contains(Point_)) {
               PointParameters.Coordinate  = CP;
               PointParameters.PointNumber = i - 1;
               PointParameters.PointType   = PolyLineOutputPoint;
               break;
            }
         }
         //
         PreviousPoint = CurrentPoint;
         CurrentPoint = NextPoint;
         Factors = Point->factors();
      }
      //
      return PointParameters;
   }

   //-------------------------------------------------------------------------------------
   QRectF TPolyLine::boundingBoxF(int Zoom_)
   {
      if(!w_MapAdapter) return QRectF();
      //
      int CurrentZoom;
      if(Zoom_ >= 0) CurrentZoom = Zoom_;
      else           {
         CurrentZoom = w_MapAdapter->currentZoom();
         setNeedRecalcBoundingBox(true);
      }
      //
      bool EditMode = layer()->layerManager()->mapControl()->editMode();
      if(EditMode != m_OldEditMode || CurrentZoom != m_Zoom || needRecalcBoundingBox()) {
         m_BoundingBoxF = painterPath().controlPointRect();
         if(type() == PolyLineTCBSplineType) {
            m_BoundingBoxF = m_BoundingBoxF.united(painterPath().boundingRect());
         }
         
         // recompute the pixel coordinates
         setNeedRecalcBoundingBox(true);
         boundingBox(CurrentZoom);
         m_OldEditMode = EditMode;
         setNeedRecalcBoundingBox(false);
      }

   return m_BoundingBoxF;
   }

   //-------------------------------------------------------------------------------------
   QPointF TPolyLine::coordinate(const unsigned long PointNumber_)
   {
      QPointF Point(0, 0);
      if((int)PointNumber_ < m_Points.count()) {
         Point = m_Points.at(PointNumber_)->coordinate();
      }
      return Point;      
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setCoordinate(const unsigned long PointNumber_, const QPointF Coordinate_)
   {
      if((int)PointNumber_ < m_Points.count()) {
         m_Points.at(PointNumber_)->setCoordinate(Coordinate_);
         setModified();
      }
      setNeedRecalcBoundingBox();
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setTensionContinuityBias( const unsigned long PointNumber_, const QPointF Coordinate_, const long PointType_ )
   {
      static QPoint DisplayShift(POINTSQUAREHALFHEIGHT, POINTSQUAREHALFHEIGHT);
      //
      if((int)PointNumber_ < m_Points.count()) {
         TPolyLinePoint* PolyLinePoint;
         PolyLinePoint    = m_Points.at(PointNumber_);
         qreal Tension    = PolyLinePoint->tension();
         qreal Continuity = PolyLinePoint->continuity();
         qreal Bias       = PolyLinePoint->bias();
         //
         unsigned PointsCount = m_Points.count();
         QPointF PreviousPoint, CurrentPoint, NextPoint;
         CurrentPoint = PolyLinePoint->coordinate();
         if(PointNumber_) PreviousPoint = m_Points.at(PointNumber_ - 1)->coordinate();
         else {
            NextPoint = m_Points.at(1)->coordinate();
            if(closed()) PreviousPoint = m_Points[PointsCount - 1]->coordinate();
            else         PreviousPoint = 1.4 * CurrentPoint - 0.4 * NextPoint;
         }
         if(PointNumber_ == (PointsCount - 1)) {
            if(closed()) NextPoint = m_Points.at(0)->coordinate();
            else         NextPoint = 1.4 * CurrentPoint - 0.4 * PreviousPoint;
         }
         else            NextPoint = m_Points.at(PointNumber_ + 1)->coordinate();
         //
         QPointF TangentIn  = NextPoint    - CurrentPoint;
         QPointF TangentOut = CurrentPoint - PreviousPoint;
         //
         switch(PolyLinePoint->editState()) {
         case PointEditStateTensionBias: 
            {
               QPointF T, K1, K2, K12, K21;
               switch(PointType_) {
               case PolyLineInputPoint:
                  T = CurrentPoint - Coordinate_;
                  K1 = TangentIn  * (1. + Continuity) / 2.;
                  K2 = TangentOut * (1. - Continuity) / 2.;
                  break;
               case PolyLineOutputPoint:
                  T = - CurrentPoint + Coordinate_;
                  K1 = TangentIn  * (1. - Continuity) / 2.;
                  K2 = TangentOut * (1. + Continuity) / 2.;
                  break;
               default:
                  return;
               }
               // calculate the bias and the tension
               K12 = K1 + K2;
               K21 = K2 - K1;
               try {
                  Bias = (T.x() * K12.y() - T.y() * K12.x()) / (T.y() * K21.x() - T.x() * K21.y());
                  if(qAbs(Bias) > 1.) Bias = sgn(Bias);
                  if(qAbs(K12.x() + Bias * K21.x()) > qAbs(K12.y() + Bias * K21.y()))
                       Tension = 1. - T.x() / (K12.x() + Bias * K21.x());
                  else Tension = 1. - T.y() / (K12.y() + Bias * K21.y());
                  Tension = sgn(Tension) * std::max(qAbs(1. - T.x() / (K12.x() + Bias * K21.x())), qAbs(1. - T.y() / (K12.y() + Bias * K21.y())));
                  if(qAbs(Tension) > 1.) Tension = sgn(Tension);
               }
               catch(...) {}
               // check the tangent minimum length
               T = (1. - Tension) * (K12 + K21 * Bias);
               if(pow(pow(T.x(), 2) + pow(T.y(), 2), 0.5) > w_MapAdapter->displayToCoordinate(DisplayShift * 1.5).x() + 180.) {
                  PolyLinePoint->setTension(Tension);
                  PolyLinePoint->setBias(Bias);
                  setModified();
               }
            }
            break;
         case PointEditStateContinuity:
            {
               QPointF T, K1, K2;
               K1 = TangentIn  * (1. - Tension) * (1. - Bias) / 2.;
               K2 = TangentOut * (1. - Tension) * (1. + Bias) / 2.;
               // calculate the continuity
               switch(PointType_) {
               case PolyLineInputPoint:
                  try {
                     T = CurrentPoint - Coordinate_;
                     if(qAbs(K1.x() - K2.x()) > qAbs(K1.y() - K2.y()))
                          Continuity = (T.x() - K1.x() - K2.x()) / (K1.x() - K2.x());
                     else Continuity = (T.y() - K1.y() - K2.y()) / (K1.y() - K2.y());
                  }
                  catch(...) {}
                  break;
               case PolyLineOutputPoint:
                  try {
                     T = - CurrentPoint + Coordinate_;
                     if(qAbs(K2.x() - K1.x()) > qAbs(K2.y() - K1.y()))
                          Continuity = (T.x() - K1.x() - K2.x()) / (K2.x() - K1.x());
                     else Continuity = (T.y() - K1.y() - K2.y()) / (K2.y() - K1.y());
                  }
                  catch(...) {}
                  break;
               default:
                  return;
               }
               PolyLinePoint->setContinuity(Continuity);
               setModified();
            }
            break;
         }
         setNeedRecalcBoundingBox();
      }
   }

   //-------------------------------------------------------------------------------------
   qreal TPolyLine::tension(const unsigned long PointNumber_)
   {
      qreal Tension = 0;
      if((int)PointNumber_ < m_Points.count()) {
         Tension = m_Points.at(PointNumber_)->tension();
      }
      return Tension;      
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setTension( const unsigned long PointNumber_, const qreal Tension_, const bool SetModified_ /*= true*/ )
   {
      if((int)PointNumber_ < m_Points.count()) {
         m_Points.at(PointNumber_)->setTension(Tension_);
         if(SetModified_) setModified();
         setNeedRecalcBoundingBox();
         update();
      }
   }

   //-------------------------------------------------------------------------------------
   qreal TPolyLine::continuity(const unsigned long PointNumber_)
   {
      qreal Continuity = 0;
      if((int)PointNumber_ < m_Points.count()) {
         Continuity = m_Points.at(PointNumber_)->continuity();
      }
      return Continuity;      
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setContinuity( const unsigned long PointNumber_, const qreal Continuity_, const bool SetModified_ /*= true*/ )
   {
      if((int)PointNumber_ < m_Points.count()) {
         m_Points.at(PointNumber_)->setContinuity(Continuity_);
         if(SetModified_) setModified();
         setNeedRecalcBoundingBox();
         update();
      }
   }

   //-------------------------------------------------------------------------------------
   qreal TPolyLine::bias(const unsigned long PointNumber_)
   {
      qreal Bias = 0;
      if((int)PointNumber_ < m_Points.count()) {
         Bias = m_Points.at(PointNumber_)->bias();
      }
      return Bias;      
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::setBias( const unsigned long PointNumber_, const qreal Bias_, const bool SetModified_ /*= true*/ )
   {
      if((int)PointNumber_ < m_Points.count()) {
         m_Points.at(PointNumber_)->setBias(Bias_);
         if(SetModified_) setModified();
         setNeedRecalcBoundingBox();
         update();
      }
   }

   //-------------------------------------------------------------------------------------
   bool TPolyLine::linkPoint(const unsigned long PointNumber_, TGeometry* Geometry_)
   {
      if((int)PointNumber_ < m_Points.count()) {
         TBasePoint* LinePoint = m_Points.at(PointNumber_);
         LinePoint->addLinkedPoint(Geometry_);
         LinePoint->setCoordinate(Geometry_->coordinate());
         return true;
      }
      return false;
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::unlinkPoint(TBasePoint* Point_)
   {
      foreach(TPolyLinePoint* Point, m_Points) Point->removeLinkedPoint(Point_);
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::unlinkPointAll(const unsigned long PointNumber_)
   {
      if((int)PointNumber_ < m_Points.count()) {
         m_Points.at(PointNumber_)->unlinkAll();
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::unlinkAll(void)
   {
      foreach(TPolyLinePoint* Point, m_Points) Point->unlinkAll();
   }

   //-------------------------------------------------------------------------------------
   QPointF TPolyLine::pointAtPercent(const qreal Percent_)
   {
      if(Percent_ <= 100.) return painterPath().pointAtPercent(Percent_ / 100.);
      else return m_Points.last()->coordinate();
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::nextEditState( const unsigned long PointNumber_ )
   {
      if((int)PointNumber_ < m_Points.count()) {
         TPolyLinePoint* CurrentPoint = m_Points.at(PointNumber_);
         TPointEditState State = CurrentPoint->editState();
         foreach(TPolyLinePoint* Point, m_Points) { Point->setEditState(PointEditStateTensionBias); }
         CurrentPoint->setEditState(State);
         CurrentPoint->nextEditState();
         update();
      }
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::TPolyLinePoint::setTension( const qreal Tension_ /*= 0*/ )
   {
      if(qAbs(Tension_) > 1.) m_Tension = sgn(Tension_);
      else                   m_Tension = Tension_;
      m_Factors = TFactors(m_Tension, m_Continuity, m_Bias);
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::TPolyLinePoint::setContinuity( const qreal Continuity_ /*= 0*/ )
   {
      if(qAbs(Continuity_) > 0.9) m_Continuity = sgn(Continuity_) * 0.9; // 0.9 is special value that cuts off the boundary effects
      else                       m_Continuity = Continuity_;
      m_Factors = TFactors(m_Tension, m_Continuity, m_Bias);
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::TPolyLinePoint::setBias( const qreal Bias_ /*= 0*/ )
   {
      if(qAbs(Bias_) > 1.) m_Bias = sgn(Bias_);
      else                m_Bias = Bias_;
      m_Factors = TFactors(m_Tension, m_Continuity, m_Bias);
   }

   //-------------------------------------------------------------------------------------
   void TPolyLine::TPolyLinePoint::nextEditState( void )
   {
      switch(m_EditState) {
      case PointEditStateTensionBias:
         m_EditState = PointEditStateContinuity;
         break;
      case PointEditStateContinuity:
         m_EditState = PointEditStateTensionBias;
         break;
      }
   }
}
