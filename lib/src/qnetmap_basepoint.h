#ifndef QNETMAP_BASEPOINT_H
#define QNETMAP_BASEPOINT_H

#include <QPointF>
#include <QSet>
#include <QAtomicInt>

#include "qnetmap_global.h"

namespace qnetmap
{
   class TBasePoint;
   class TGeometry;

   typedef QSet<TBasePoint*> TLinkedPoints;

   class QNETMAP_EXPORT TBasePoint
   {
   public:
      explicit TBasePoint(TGeometry* ParentGeometry_ = NULL, QPointF Coordinate_ = QPointF());
      virtual ~TBasePoint(void);

      //! \brief 
      void setCoordinate(const QPointF& Coordinate_, const bool SetLinkedPointsCoordinate_ = true, const bool Lock_ = true);
      //! \brief returns the coordinate of the point
      //!        The x component of the returned QPointF is the longitude value,
      //!        the y component the latitude
      //! \return the coordinate of a point
      QPointF coordinate(void) const { return m_Coordinate; }
      //! \brief returns the longitude of the point
      //! \return the longitude of the point
      qreal longitude(void) const { return m_Coordinate.x(); }
      //! \brief returns the latitude of the point
      //! \return the latitude of the point
      qreal latitude() const { return m_Coordinate.y(); }
      //! \brief 
      unsigned countLinkedPoint(void) { return m_LinkedPoints.count(); }
      //! \brief 
      void addLinkedPoint(TBasePoint* Point_, const bool SetFeedback_ = true, const bool Lock_ = true);
      //! \brief 
      bool removeLinkedPoint(TBasePoint* Point_, const bool RemoveFeedback_ = true, const bool Lock_ = true);
      //! \brief 
      void unlinkAll(void);
      //! \brief 
      /*
      void setUserPointer(void* UserPointer_, const bool NeedDelete_ = false)
      { 
         w_UserPointer = UserPointer_;
         m_UserPointerNeedDelete = NeedDelete_;
      }
      */
      //! returns the parent TGeometry of this TGeometry
      /*!
      * A TLineString is a composition of many Points. This methods returns the parent (the TLineString) of a TPoint
      * @return the parent TGeometry of this TGeometry
      */
      virtual TGeometry* parentGeometry(void) const { return w_ParentGeometry; }
      //! \brief setting a pointer to the parent geometry 
      virtual void setParentGeometry(TGeometry* Geometry_) { w_ParentGeometry = Geometry_; }

   protected:
      //! \brief 
      TLinkedPoints const& linkedPoints(void) const { return m_LinkedPoints; }

   private:
      //! \var 
      TGeometry* w_ParentGeometry;
      //! \var 
      QPointF m_Coordinate;
      //! \var 
      TLinkedPoints m_LinkedPoints;
      //! \var 
      static QAtomicInt m_BasePointAtomicInt;

      /*
      //! \var the user defined pointer
      void* w_UserPointer;
      //! \var a sign of the need to remove the user defined pointer in the destructor
      bool  m_UserPointerNeedDelete;
      */
   };
}
#endif
