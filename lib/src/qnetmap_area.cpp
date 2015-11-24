#include <QPainter>

#include "qnetmap_area.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap
{
   ////////////////////////////////////////////////////////////////////////////////////////
   TArea::TArea( const TCoordinatesList& Coordinates_, const QString& Name_ /*= QString()*/ ) 
      : TParent(Coordinates_, Name_), m_Color(0x6f000000)
   {
      setGeometryType("Area");
      TParent::setClosed();
      setBackPlane();
   }

   //-------------------------------------------------------------------------------------
   void TArea::drawBackPlane( QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ /*= QPoint()*/ )
   {
      Q_UNUSED(Viewport_)
      Q_UNUSED(Offset_)

      if(!visible() || !w_MapAdapter) return;

      QRgb AreaColor = areaColor();
      unsigned char Alpha = AreaColor >> 24;
      AreaColor &= 0x00ffffff;

      QPainterPath PainterPath;
      if(painterPath(PainterPath, type(), true)) {
         PainterPath.closeSubpath();
         PainterPath.setFillRule(Qt::WindingFill);
         //
         QPoint TopLeft = m_BoundingBox.normalized().topLeft();
         unsigned TopLeftX = TopLeft.x(), TopLeftY = TopLeft.y();
         // move PainterPath
         unsigned ElementCount = PainterPath.elementCount();
         for(unsigned i = 0; i < ElementCount; ++i) {
            QPainterPath::Element Element = PainterPath.elementAt(i);
            PainterPath.setElementPositionAt(i, Element.x - TopLeftX, Element.y - TopLeftY);
         }
         //
         // формируем рисунок с необходимой прозрачностью
         QRect Rect(QPoint(0, 0), m_BoundingBox.normalized().size());
         QPixmap TransparentPixmap(Rect.size());
         TransparentPixmap.fill(Qt::transparent);
         QPainter Painter(&TransparentPixmap);
         Painter.setCompositionMode(QPainter::CompositionMode_Source);
         Painter.setPen(Qt::NoPen);
         Painter.setBrush(QBrush(AreaColor));
         Painter.drawPath(PainterPath);
         // устанавливаем прозрачность
         Painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
         Painter.fillRect(TransparentPixmap.rect(), QColor(0, 0, 0, Alpha));
         //
         Painter.end();
         // рисуем
         Painter_->drawPixmap(TopLeft, TransparentPixmap);
      }
   }

   //-------------------------------------------------------------------------------------
   QPoint TArea::topLeftTextRect( QSize const& Size_ )
   {
      if(!w_MapAdapter) return QPoint();
      return alignedPoint(boundingBox().center(), Size_, Middle);
   }

   //-------------------------------------------------------------------------------------
   const QString& TArea::name( void ) const
   {
      static QString EmptyString = QString();
      // возвращаем имя только если максимальный размер области больше максимального размера надписи
      QRect NameRect(const_cast<TArea*>(this)->nameBoundingBox()), AreaRect(const_cast<TArea*>(this)->boundingBox());
      unsigned NameMax = std::max(NameRect.height(), NameRect.width());
      unsigned AreaMax = std::max(AreaRect.height(), AreaRect.width());
      if(NameMax < AreaMax) return TParent::name();
      else                  return EmptyString;
   }

   //-------------------------------------------------------------------------------------
   const QString& TArea::areaName( void ) const
   {
      return TParent::name();
   }
}