#include "qnetmap_label.h"
#include "qnetmap_mapadapter.h"

#include <QFont>
#include <QPainter>

namespace qnetmap
{
   // -----------------------------------------------------------------------
   TLabel::TLabel(const QPointF Point_, const QString& Text_, 
                  const enum TAlignment Alignment_ /*= Middle*/, 
                  const QString& Name_ /*= QString()*/ ) 
      : TGeometry(Name_), m_Text(Text_), m_Alignment(Alignment_), m_BoundingBoxChanged(true)
   {
      setGeometryType("Label");
      setCoordinate(Point_, false);
      setStatic(true);
   }


   // -----------------------------------------------------------------------
   void TLabel::drawGeometry(QPainter* Painter_, const QRect &Viewport_, const QPoint Offset_)
   {
      Q_UNUSED(Viewport_)

      if(!visible() || !w_MapAdapter) return;

      drawText(Painter_, m_Text);
   }

   // -----------------------------------------------------------------------
   QPoint TLabel::topLeftTextRect( const QSize& Size_ )
   {
      if(!w_MapAdapter) return QPoint();
      return alignedPoint(w_MapAdapter->coordinateToDisplay(coordinate()), Size_, m_Alignment);
   }

   // -----------------------------------------------------------------------
   void TLabel::calculateBoundingBox( void )
   {
      QImage TmpImage(QSize(2000, 200), QImage::Format_ARGB32_Premultiplied);
      QPainter Painter(&TmpImage);
      QFont Font(textFontName(), textFontSize());
      Font.setBold(false);
      Painter.setFont(Font);
      QRect RectText = Painter.boundingRect(QRect(0, 0, 2000, 200), Qt::AlignLeft, m_Text);
      QRect OldBoundingBox = m_BoundingBox;
      m_BoundingBox = QRect(topLeftTextRect(RectText.size()), RectText.size());
      m_BoundingBoxChanged = OldBoundingBox != m_BoundingBox;
      boundingBoxF();
   }

   // -----------------------------------------------------------------------
   QRectF TLabel::boundingBoxF( const int Zoom_ /*= -1*/ )
   {
      if(!w_MapAdapter) return QRectF();
      //
      int CurrentZoom;
      if(Zoom_ >= 0) CurrentZoom = Zoom_;
      else           CurrentZoom = w_MapAdapter->currentZoom();

      if(m_BoundingBoxChanged || CurrentZoom != m_Zoom || needRecalcBoundingBox()) {
         m_BoundingBoxF = QRectF(w_MapAdapter->displayToCoordinate(m_BoundingBox.topLeft()),
                                 w_MapAdapter->displayToCoordinate(m_BoundingBox.bottomRight())).normalized();
         m_Zoom = CurrentZoom;
         setNeedRecalcBoundingBox(false);
      }
      //
      return m_BoundingBoxF;
   }

   // -----------------------------------------------------------------------
   QRect TLabel::boundingBox( const int Zoom_ /*= -1*/ )
   {
      if(!w_MapAdapter) return QRect();
      //
      int CurrentZoom;
      if(Zoom_ >= 0) CurrentZoom = Zoom_;
      else           CurrentZoom = w_MapAdapter->currentZoom();

      if(CurrentZoom != m_Zoom) calculateBoundingBox();
      //
      return m_BoundingBox;
   }

   // -----------------------------------------------------------------------
   void TLabel::setCoordinate( QPointF const& Coordinate_, const bool SetModified_ /*= true*/ )
   { 
      TParent::setCoordinate(Coordinate_, SetModified_); 
      calculateBoundingBox(); 
   }

}
