#ifndef QNETMAP_AREA_H
#define QNETMAP_AREA_H

#include "qnetmap_polyline.h"

namespace qnetmap
{
   //! \class TArea
   //! \brief An area, based on the closed polyline
   class QNETMAP_EXPORT TArea : public TPolyLine
   {
      //Q_OBJECT

      typedef TPolyLine TParent;

   public:
      explicit TArea(const TCoordinatesList& Coordinates_, const QString& Name_ = QString());
      virtual ~TArea() {};

      //! \brief
      virtual unsigned borderColor(void) const { return TParent::color(); }
      //! \brief
      virtual void setBorderColor(const unsigned Color_) { TParent::setColor(Color_); }
      //! \brief
      virtual unsigned areaColor(void) const { return m_Color; }
      //! \brief
      virtual void setAreaColor(const unsigned Color_) { m_Color = Color_; update(); }
      //! \brief fills the area with the specified color
      virtual void drawBackPlane(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
      //! \brief return the name of the geometry
      virtual const QString& name(void) const;
      virtual const QString& areaName(void) const;

   protected:
      //! \brief top left corner of the rectangle surrounding the text
      //! \param QSize& Size_ - size of the rectangle surrounding the text
      virtual QPoint topLeftTextRect(QSize const& Size_);

   private:
      Q_DISABLE_COPY(TArea)

      //! \brief dummy
      virtual unsigned color(void) const { return TParent::color(); }
      //! \brief dummy
      virtual void setColor(const unsigned Color_) {}
      //! \brief dummy
      virtual void setClosed(const bool) {}

      QRgb m_Color;
   };
}
#endif // QNETMAP_AREA_H
