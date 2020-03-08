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

  void setDataRange(double xmin, double ymin, double xmax, double ymax) override;
  void resetDataRange() override;

  void setViewMatrix(const CMatrix2D &view_matrix) override;
  void unsetViewMatrix() override;

  virtual void setQTransform();
  virtual void unsetQTransform();

  void beginDraw() override;
  void endDraw() override;

  void startDoubleBuffer(bool clear=true) override;
  void endDoubleBuffer  (bool copy=true) override;
  void copyDoubleBuffer () override;

  void setForeground(const CRGBA &fg) override;
  void setBackground(const CRGBA &bg) override;

  void setLineWidth(double width) override;
  void setLineDash(const CLineDash &line_dash) override;
  void setLineCap(CLineCapType cap) override;
  void setLineJoin(CLineJoinType join) override;

  void setMitreLimit(double limit) override;
  virtual double getMitreLimit() const;

  void setFillType(CFillType fill_type) override;
  virtual CFillType getFillType() const;

  void clear() override;
  void fill() override;

  void drawPoint(const CPoint2D &point) override;

  void drawSymbol(const CPoint2D &point, CSymbol2D::Type type) override;

  void drawLine(const CPoint2D &point1, const CPoint2D &point2) override;

  void drawRectangle(const CBBox2D &bbox) override;
  void fillRectangle(const CBBox2D &bbox) override;

  void gradientFillRectangle(const CBBox2D &bbox, CGenGradient *gradient) override;

  void drawPolygon(const PointList &points) override;

  void fillPolygon(const PointList &points) override;
  void fillImagePolygon(const PointList &points, CImagePtr image) override;
  void fillGradientPolygon(const PointList &points, CRefPtr<CGenGradient> gradient) override;

  void fillArc(const CPoint2D &center, double xr, double yr,
               double angle1, double angle2) override;
  void fillImageArc(const CPoint2D &center, double xr, double yr,
                    double angle1, double angle2, CImagePtr image) override;
  void fillGradientArc(const CPoint2D &center, double xr, double yr,
                       double angle1, double angle2, CRefPtr<CGenGradient> gradient) override;

  void drawText(const CPoint2D &point, const std::string &text) override;
  void fillText(const CPoint2D &point, const std::string &text) override;

  void drawTextInRect(const CBBox2D &rect, const std::string &text) override;

  void setImage(CImagePtr image) override;
  void setAlphaImage(CImagePtr image) override;

  void drawImage(const CPoint2D &point, CImagePtr image) override;

  void drawAlphaImage(const CPoint2D &point, CImagePtr image) override;

  void drawImageInBox(const CBBox2D &bbox, CImagePtr image) override;

  void drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image) override;

  void drawBezier(C3Bezier2D &bezier) override;

  CRenderer2D *createImageRenderer() const override;

  CPath2D *createPath() const override;

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
