#ifndef QNETMAP_LABEL_H
#define QNETMAP_LABEL_H

#include "qnetmap_global.h"
#include "qnetmap_geometry.h"

#include <QColor>

namespace qnetmap
{
   //! \class TLabel
   //! \brief 
    class QNETMAP_EXPORT TLabel : public TGeometry
    {
        Q_OBJECT

    public:
       typedef TGeometry TParent;

       TLabel(const QPointF Point_, const QString& Text_, 
              const enum TAlignment alignment = Middle, const QString& Name_ = QString());
       virtual ~TLabel() {};

       //! \brief draw the geometry
       virtual void drawGeometry(QPainter* Painter_, const QRect& Viewport_, const QPoint Offset_ = QPoint());
       //! \brief 
       virtual const QString& text(void) const { return m_Text; }
       //! \brief 
       virtual void setText(QString& Text_) { m_Text = Text_; calculateBoundingBox(); }
       //! \brief surrounding rectangle in geographical coordinates
       virtual QRectF boundingBoxF(const int Zoom_ = -1);
       //! \brief surrounding rectangle in pixel coordinates on a given scale
       virtual QRect boundingBox(const int Zoom_ = -1);
       //! \brief 
       virtual void calculateBoundingBox(void);
       //! \brief 
       virtual void setTextFontName(QString& FontName_) { TParent::setTextFontName(FontName_); calculateBoundingBox(); }
       //! \brief 
       virtual void setTextFontSize(const unsigned FontSize_) { TParent::setTextFontSize(FontSize_); calculateBoundingBox(); }
       //! \brief 
       virtual void setCoordinate(QPointF const& Coordinate_, const bool SetModified_ = true);
       //! \brief sets the pointer to the adapter card, which draws label
       virtual void setMapAdapter(TMapAdapter* MapAdapter_) { TParent::setMapAdapter(MapAdapter_); calculateBoundingBox(); }

    private:
       Q_DISABLE_COPY(TLabel)

       //! var the text alignment
       TAlignment m_Alignment;
       //! var 
       QString m_Text;
       //! var 
       bool m_BoundingBoxChanged;

    protected:
       //! \brief top left corner of the rectangle surrounding the text
       //! \param QSize& Size_ - size of the rectangle surrounding the text
       virtual QPoint topLeftTextRect(const QSize& Size_);
    };
}
#endif
