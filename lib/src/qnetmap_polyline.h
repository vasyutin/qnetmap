/*
A Kochanek–Bartels spline or Kochanek–Bartels curve is a cubic Hermite spline with 
tension, bias, and continuity parameters defined to change the behavior of the tangents.

Given n + 1 knots,

P_0, ..., P_n,

to be interpolated with n cubic Hermite curve segments, for each curve we have a starting point 
P_i and an ending point P_i+1 with starting tangent T_iO and ending tangent T_iI defined by

T_iI = ((1-t)(1+c)(1-b)(P_i+1 - P_i) + (1-t)(1-c)(1+b)(P_i - P_i-1)) / 2
T_iO = ((1-t)(1-c)(1-b)(P_i+1 - P_i) + (1-t)(1+c)(1+b)(P_i - P_i-1)) / 2

where t is the tension, b is the bias, and c is the continuity parameter.

The tension parameter, t, changes the length of the tangent vector. 
The bias parameter, b, primarily changes the direction of the tangent vector. 
The continuity parameter, c, changes the sharpness in change between tangents.

Setting each parameter to zero would give a Catmull–Rom spline.

Tension 	   T = +1 --> Tight 	            T = ?1 --> Round 	
Bias 	      B = +1 --> Post shoot 	      B = ?1 --> Pre shoot
Continuity 	C = +1 --> Inverted corners 	C = ?1 --> Box corners
*/

#ifndef QNETMAP_POLYLINE_H
#define QNETMAP_POLYLINE_H

#include "qnetmap_global.h"
#include "qnetmap_basepoint.h"
#include "qnetmap_geometry.h"
#include "qnetmap_point.h"

#include <set>
#include <cmath>

namespace qnetmap
{
   enum TPolyLineType {
      PolyLinePolygonalType  = 1,
      PolyLineTCBSplineType  = 2,
      PolyLineOrthogonalType = 3
   };

   enum TPolyLinePointType {
      NoPolyLinePoint     = -1,
      PolyLineNodePoint   = 0,
      PolyLineInputPoint  = 1,
      PolyLineOutputPoint = 2
   };

   //! \class TPolyLine
   //! \brief A collection of TPoint objects to describe a line
   class QNETMAP_EXPORT TPolyLine : public TGeometry
   {
      //Q_OBJECT

      typedef TGeometry TParent;
      typedef std::set<TGeometry*> TLinkedPoints;

   public:
      struct TPointParameters 
      {
         TPointParameters() : Coordinate(QPointF()), PointNumber(-1), PointType(0) {}
         TPointParameters(QPointF Coordinate_, unsigned PointNumber_, long PointType_ = 0) 
            : Coordinate(Coordinate_), PointNumber(PointNumber_), PointType(PointType_) {}
         TPointParameters(const TPointParameters& PointParameters_)
         {
            Coordinate = PointParameters_.Coordinate;
            PointNumber = PointParameters_.PointNumber;
            PointType = PointParameters_.PointType;
         }

         QPointF  Coordinate;
         long     PointNumber;
         long     PointType;
      };

   private:
      //! \struct
      //! \brief Factors for calculate tangents
      struct TFactors { 
         TFactors(qreal Tension_, qreal Continuity_, qreal Bias_)
         {
            fIn1  = (1. - Tension_) * (1. + Continuity_) * (1. - Bias_) / 2.;
            fIn2  = (1. - Tension_) * (1. - Continuity_) * (1. + Bias_) / 2.;
            fOut1 = (1. - Tension_) * (1. - Continuity_) * (1. - Bias_) / 2.;
            fOut2 = (1. - Tension_) * (1. + Continuity_) * (1. + Bias_) / 2.;
         }

         qreal fIn1; qreal fIn2; qreal fOut1; qreal fOut2; 
      };

      enum TPointEditState {
         PointEditStateTensionBias,
         PointEditStateContinuity
      };

   public:
		//! \class
		class TPolyLinePoint : public TBasePoint
		{
		public:
			explicit TPolyLinePoint(TGeometry* ParentGeometry_ = NULL, QPointF Point_ = QPointF()) 
				: TBasePoint(ParentGeometry_, Point_), m_Tension(0), m_Continuity(0), m_Bias(0), m_Factors(0, 0, 0), m_EditState(PointEditStateTensionBias) {}
			virtual ~TPolyLinePoint(void) {}

			//! \brief
			void setTension(const qreal Tension_ = 0);
			//! \brief
			inline qreal tension(void) const { return m_Tension; }
			//! \brief
			void setContinuity(const qreal Continuity_ = 0);
			//! \brief
			inline qreal continuity(void) const { return m_Continuity; }
			//! \brief
			void setBias(const qreal Bias_ = 0);
			//! \brief
			inline qreal bias(void) const { return m_Bias; }
			//! \brief
			inline void setEditState(const TPointEditState EditState_) { m_EditState = EditState_; }
			//! \brief
			inline TPointEditState editState(void) const { return m_EditState; }
			//! \brief
			void nextEditState(void);
			//! \brief
			inline TFactors& factors(void) { return m_Factors; }
			//! \brief
			inline void setTensionContinuityBias(const qreal Tension_, const qreal Continuity_, const qreal Bias_) 
			{
				m_Tension = Tension_;
				m_Continuity = Continuity_;
				m_Bias = Bias_;
			}
      
		private:
			Q_DISABLE_COPY(TPolyLinePoint)

			qreal    m_Tension;
			qreal    m_Continuity;
			qreal    m_Bias;
			//! \var factors for calculate tangents
			TFactors m_Factors;
			TPointEditState m_EditState;
		};
		typedef QList<TPolyLinePoint*> TPolyLinePoints;

		explicit TPolyLine(const TCoordinatesList& Coordinates_, const QString& name = QString());
		virtual ~TPolyLine();

		//! \brief returns the points of the TPolyLine
		virtual TPolyLinePoints& points();
		//! \brief adds a point at the end of the TPolyLine
		virtual void addPoint(QPointF const& Point_, const bool SendUpdate = true, const bool SetModified_ = true);
		//! \brief delete a point from polyline
		virtual void deletePoint(const unsigned long PointNumber_, const bool SendUpdate = true, const bool SetModified_ = true);
		//! \brief adds a point at the begin of the TPolyLine
		virtual void addBeginPoint(QPointF const& Point_, const bool SendUpdate = true, const bool SetModified_ = true);
		//! \brief insert a point after PointNumber_ point of the TPolyLine
		virtual void insertPoint(QPointF const& Point_, const unsigned long PointNumber_, const bool SendUpdate = true, const bool SetModified_ = true);
		//! \brief sets the given coordinates list as points of the TPolyLine
		virtual void setPoints(TCoordinatesList const& Points_, const bool SetModified_);
		//! \brief returns the number of Points the TPolyLine consists of
		virtual int pointsCount() const;
		//! \brief
		virtual QRectF boundingBoxF(const int Zoom_ = -1);
		//! \brief return offset painter path for polyline with his thickness
		virtual QPainterPath painterPath(void);
		//! \brief
		inline unsigned long type(void) const { return m_Type; }
		//! \brief
		inline void setType(const unsigned long Type_) { m_Type = Type_; update(); }
		//! \brief
		virtual QPointF coordinate(const unsigned long PointNumber_);
		//! \brief
		virtual void setCoordinate(const unsigned long PointNumber_, const QPointF Coordinate_);
		//! \brief
		virtual void setTensionContinuityBias(const unsigned long PointNumber_, const QPointF Coordinate_, const long PointType_);
		//! \brief
		virtual qreal tension(const unsigned long PointNumber_);
		//! \brief
		virtual void setTension(const unsigned long PointNumber_, const qreal Tension_, const bool SetModified_ = true);
		//! \brief
		virtual qreal continuity(const unsigned long PointNumber_);
		//! \brief
		virtual void setContinuity(const unsigned long PointNumber_, const qreal Continuity_, const bool SetModified_ = true);
		//! \brief
		virtual qreal bias(const unsigned long PointNumber_);
		//! \brief
		virtual void setBias(const unsigned long PointNumber_, const qreal Bias_, const bool SetModified_ = true);
		//! \brief
		virtual void nextEditState(const unsigned long PointNumber_);
		//! \brief
		virtual bool linkPoint(const unsigned long PointNumber_, TGeometry* Geometry_);
		//! \brief
		virtual void unlinkPoint(TBasePoint* Point_);
		//! \brief
		virtual void unlinkPointAll(const unsigned long PointNumber_);
		//! \brief
		virtual void unlinkAll(void);
		//! \brief
		virtual unsigned color(void) const { return m_Color; }
		//! \brief
		virtual void setColor(const unsigned Color_) { m_Color = Color_; update(); }
		//! \brief
		virtual unsigned outlineColor(void) const { return m_OutlineColor; }
		//! \brief
		virtual void setOutlineColor(const unsigned Color_) { m_OutlineColor = Color_; update(); }
		//! \brief
		virtual unsigned long thickness(void) const { return m_Thickness; }
		//! \brief
		virtual void setThickness(const unsigned long Thickness_) { m_Thickness = Thickness_; update(); }
		//! \brief Returns the point at the percentage Percent_ of the current path. 
		//!        The argument Percent_ has to be between 0 and 100.
		//!
		//!        Note that similarly to other percent methods, the percentage measurement is not 
		//!        linear with regards to the length, if curves are present in the path. When curves 
		//!        are present the percentage argument is mapped to the t parameter of the Bezier equations.
		virtual QPointF pointAtPercent(const qreal Percent_);
		//! \brief
		virtual bool touchesPoints(const QPointF Point_);
		//! \brief
		virtual bool touchesControlPoints(const QPointF Point_);
		//! \brief
		virtual TPointParameters touchesPoint(const QPointF Point_);
		//! \brief
		virtual TPointParameters touchesControlPoint(const QPointF Point_);
		//! \brief
		virtual bool touches(const QPointF Point_);
		//! \brief
		virtual long lastTouchesSegment(void) const { return m_LastTouchesSegment; }
		//! \brief
		virtual long lastTouchesPoint(void) const { return m_LastTouchesPoint; }
		//! \brief
		inline void setClosed(const bool Closed_ = true) { m_Closed = Closed_; update(); }
		//! \brief
		inline bool closed(void) const { return m_Closed; }

   protected:
      //! \brief
      virtual bool painterPath(QPainterPath& PainterPath_, const unsigned long LineType_, const bool ConvertToDisplay_ = true);

   private:
      Q_DISABLE_COPY(TPolyLine)

      //! \brief
      virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_);
      //! \brief
      virtual void drawSelection(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_);
      //! \brief
      virtual void splinePath(QPainterPath& PainterPath_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void polygonalPath(QPainterPath& PainterPath_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void orthogonalPath(QPainterPath& PainterPath_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void pointPath(QPainterPath& PointPath_, const QPointF& Point_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void controlPointPath(QPainterPath& PointPath_, const QPointF& Point_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void pointsPath(QPainterPath& PointsPath_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual void controlPointsPath(QPainterPath& ControlPointsPath_, const bool ConvertToDisplay_ = true);
      //! \brief
      virtual QPainterPath subPath(const QPainterPath& PainterPath_, const unsigned long LineType_, const unsigned SegmentNumber_);
      //! \brief increasing the width of the line
      virtual QPainterPath strokePath(const QPainterPath& PainterPath_);

      TPolyLinePoints m_Points;
      unsigned long   m_Type;
      QRgb            m_Color;
      QRgb            m_OutlineColor;
      unsigned long   m_Thickness;
      long            m_LastTouchesSegment;
      long            m_LastTouchesPoint;
      bool            m_Closed;              //! \var an indication that the polyline is closed
   };
}
#endif // QNETMAP_POLYLINE_H
