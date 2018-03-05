#ifndef CQT_RENDERER_2D_H
#define CQT_RENDERER_2D_H

#define CRENDERER_2D_VIRTUAL

#include <CRenderer2D.h>
#include <QColor>
#include <memory>

class QPainter;
class QPen;
class QBrush;
class QPixmap;
class QPainterPath;
class CQFont;
class CQPath2D;

class CGenGradient;

class CQRenderer2D : public CRenderer2D {
 public:
  CQRenderer2D(QPainter *painter=nullptr);

  virtual ~CQRenderer2D();

  QPainter *getQPainter() const { return painter_; }

  QPen   *getQPen  () const { return pen_  .get(); }
  QBrush *getQBrush() const { return brush_.get(); }

  CQPath2D *getQPath() const;

  virtual int getWidth () = 0;
  virtual int getHeight() = 0;

  virtual void setDataRange(double xmin, double ymin, double xmax, double ymax);
  virtual void resetDataRange();

  void setViewMatrix(const CMatrix2D &view_matrix);
  void unsetViewMatrix();

  virtual void setQTransform();
  virtual void unsetQTransform();

  void beginDraw() override;
  void endDraw() override;

  virtual void startDoubleBuffer(bool clear=true);
  virtual void endDoubleBuffer  (bool copy=true);
  virtual void copyDoubleBuffer ();

  virtual void setForeground(const CRGBA &fg);
  virtual void setBackground(const CRGBA &bg);

  virtual void setLineWidth(double width);
  virtual void setLineDash(const CLineDash &line_dash);
  virtual void setLineCap(CLineCapType cap);
  virtual void setLineJoin(CLineJoinType join);

  virtual void   setMitreLimit(double limit);
  virtual double getMitreLimit() const;

  virtual void      setFillType(CFillType fill_type);
  virtual CFillType getFillType() const;

  virtual void clear();
  virtual void fill();

  virtual void drawPoint(const CPoint2D &point);

  virtual void drawSymbol(const CPoint2D &point, CSymbol2D::Type type);

  virtual void drawLine(const CPoint2D &point1, const CPoint2D &point2);

  virtual void drawRectangle(const CBBox2D &bbox);
  virtual void fillRectangle(const CBBox2D &bbox);

  virtual void gradientFillRectangle(const CBBox2D &bbox,
                                     CGenGradient *gradient);

  virtual void drawPolygon(const PointList &points);

  virtual void fillPolygon(const PointList &points);
  virtual void fillImagePolygon(const PointList &points, CImagePtr image);
  virtual void fillGradientPolygon(const PointList &points,
                                   CRefPtr<CGenGradient> gradient);

  virtual void fillArc(const CPoint2D &center, double xr, double yr,
                       double angle1, double angle2);
  virtual void fillImageArc(const CPoint2D &center, double xr, double yr,
                            double angle1, double angle2, CImagePtr image);
  virtual void fillGradientArc(const CPoint2D &center, double xr, double yr,
                               double angle1, double angle2,
                               CRefPtr<CGenGradient> gradient);

  virtual void drawText(const CPoint2D &point, const std::string &text);
  virtual void fillText(const CPoint2D &point, const std::string &text);

  virtual void drawTextInRect(const CBBox2D &rect, const std::string &text);

  virtual void setImage(CImagePtr image);
  virtual void setAlphaImage(CImagePtr image);

  virtual void drawImage(const CPoint2D &point, CImagePtr image);

  virtual void drawAlphaImage(const CPoint2D &point, CImagePtr image);

  virtual void drawImageInBox(const CBBox2D &bbox, CImagePtr image);

  virtual void drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image);

  virtual void drawBezier(C3Bezier2D &bezier);

  virtual CRenderer2D *createImageRenderer() const;

  virtual CPath2D *createPath() const;

  virtual void strokePath(QPainterPath *path);

  virtual CQFont *getQFont() const;

 private:
  typedef std::unique_ptr<QPainter> PainterP;
  typedef std::unique_ptr<QPen>     PenP;
  typedef std::unique_ptr<QBrush>   BrushP;
  typedef std::unique_ptr<QPixmap>  PixmapP;

  QPainter* painter_ { nullptr };
  PainterP  painterP_;
  PenP      pen_;
  BrushP    brush_;
  PixmapP   pixmap_;
  int       pixmapWidth_  { 0 };
  int       pixmapHeight_ { 0 };
  QColor    qfg_, qbg_;
  bool      drawing_ { false };
  bool      hasTransform_ { false };
};

#endif
