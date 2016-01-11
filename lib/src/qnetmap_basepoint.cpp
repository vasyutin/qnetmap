#include "qnetmap_basepoint.h"
#include "qnetmap_geometry.h"

namespace qnetmap
{
   QAtomicInt TBasePoint::m_BasePointAtomicInt = 0;

   // -----------------------------------------------------------------------
   TBasePoint::TBasePoint(TGeometry* ParentGeometry_, QPointF Coordinate_) 
      : w_ParentGeometry(ParentGeometry_)/*, w_UserPointer(NULL), m_UserPointerNeedDelete(false)*/
   {
      setCoordinate(Coordinate_);
   }

   // -----------------------------------------------------------------------
   TBasePoint::~TBasePoint(void)
   {
      TLock Lock(m_BasePointAtomicInt, 2);
      //
      foreach(TBasePoint* Point, m_LinkedPoints) Point->removeLinkedPoint(this, false, false);
      //
      /*if(w_UserPointer && m_UserPointerNeedDelete) delete w_UserPointer;*/
   }

   // -----------------------------------------------------------------------
   void TBasePoint::setCoordinate( const QPointF& Coordinate_, const bool SetLinkedPointsCoordinate_, const bool Lock_ )
   {
      if(Lock_) while(!m_BasePointAtomicInt.testAndSetAcquire(0, 1));
      //
      m_Coordinate = Coordinate_;
      if(SetLinkedPointsCoordinate_) foreach(TBasePoint* Point, m_LinkedPoints) Point->setCoordinate(Coordinate_, false, false);
      //
      if(Lock_) m_BasePointAtomicInt.fetchAndStoreAcquire(0);
      //
      if(w_ParentGeometry) {
         w_ParentGeometry->setNeedRecalcBoundingBox();
         w_ParentGeometry->update();
      }
   }

   // -----------------------------------------------------------------------
   void TBasePoint::addLinkedPoint( TBasePoint* Point_, const bool SetFeedback_, const bool Lock_ )
   {
      if(Lock_) while(!m_BasePointAtomicInt.testAndSetAcquire(0, 1));
      //
      m_LinkedPoints.insert(Point_);
      if(SetFeedback_) Point_->addLinkedPoint(this, false, false);
      //
      if(Lock_) m_BasePointAtomicInt.fetchAndStoreAcquire(0);
   }

   // -----------------------------------------------------------------------
   bool TBasePoint::removeLinkedPoint( TBasePoint* Point_, const bool RemoveFeedback_, const bool Lock_ )
   {
      bool RetValue = false;
      //
      if(Lock_) while(!m_BasePointAtomicInt.testAndSetAcquire(0, 1));
      //
      RetValue = m_LinkedPoints.remove(Point_);
      if(RemoveFeedback_) RetValue = RetValue && Point_->removeLinkedPoint(this, false, false);
      //
      if(Lock_) m_BasePointAtomicInt.fetchAndStoreAcquire(0);
      //
      return RetValue;
   }

   // -----------------------------------------------------------------------
  void TBasePoint::unlinkAll( void )
   {
      while(!m_BasePointAtomicInt.testAndSetAcquire(0, 1));
      //
      foreach(TBasePoint* Point, m_LinkedPoints) Point->removeLinkedPoint(this, false, false);
      m_LinkedPoints.clear();
      //
      m_BasePointAtomicInt.fetchAndStoreAcquire(0);
   }
}
