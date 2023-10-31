#ifndef CQT_PIXEL_RENDERER_H
#define CQT_PIXEL_RENDERER_H

#include <memory>
#include <QColor>

#ifndef CPIXEL_RENDERER_VIRTUAL
#define CPIXEL_RENDERER_VIRTUAL
#endif

#include <CPixelRenderer.h>

class QPainter;
class QPen;
class QBrush;
class QPixmap;
class QPainterPath;
class QImage;
class CQFont;

class CQPixelRenderer : public CPixelRenderer {
 public:
  CQPixelRenderer();

  virtual ~CQPixelRenderer();

  QPainter *getQPainter() const;
  void setQPainter(QPainter *painter);

  QPen   *getQPen  () const { return pen_  .get(); }
  QBrush *getQBrush() const { return brush_.get(); }

  uint getWidth () const override = 0;
  uint getHeight() const override = 0;

  void updateSize(int width, int height) override = 0;

  void beginDraw() override;
  void endDraw() override;

  void startDoubleBuffer(bool clear=true) override;
  void endDoubleBuffer  (bool copy=true) override;
  void copyDoubleBuffer () override;

  void setForeground(const CRGBA &fg) override;
  void setBackground(const CRGBA &bg) override;

  void setLineDash(const CILineDash &line_dash) override;
  void setLineCap(CLineCapType cap) override;
  void setLineJoin(CLineJoinType join) override;

  void clear() override;
  void fill() override;

  void drawClippedPoint(const CIPoint2D &point) override;

  void drawClippedChar(const CIPoint2D &point, char c) override;

  void drawClippedString(const CIPoint2D &point, const std::string &str) override;

  void drawStringInRect(const CIBBox2D &bbox, const std::string &str) override;

  void drawScaledStringInRect(const CIBBox2D &bbox, const std::string &str);

  void drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2) override;

  void drawClippedRectangle(const CIBBox2D &bbox) override;

  void fillClippedRectangle(const CIBBox2D &bbox) override;

  void fillClippedPolygon(const IPointList &points) override;

  void fillImageClippedPolygon(const IPointList &points, CImagePtr image) override;

  void fillGradientClippedPolygon(const IPointList &points,
                                  std::shared_ptr<CGenGradient> gradient) override;

  void drawClippedArc(const CIPoint2D &center, int xr, int yr,
                      double a1, double a2) override;

  void fillClippedArc(const CIPoint2D &center, int xr, int yr,
                      double a1, double a2) override;

  void drawClippedImage(const CIPoint2D &point, CImagePtr image) override;

  void drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image) override;

  virtual void drawQImage(const CIPoint2D &point, QImage &image);

  virtual void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  virtual void strokePath(QPainterPath *path);

  void fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                           const CIPoint2D &point3) override;

  void drawClippedPolygon(const IPointList &points) override;

  void flush() override;

  int getCharWidth() override;
  int getCharAscent() override;
  int getCharDescent() override;
  int getStringWidth(const std::string &str) override;

  bool mmToPixel(double mm, double *pixel) override;

  virtual CQFont *getQFont() const;

 private:
  void setQFont();

 protected:
  std::unique_ptr<QPainter> painter_;
  std::unique_ptr<QPen>     pen_;
  std::unique_ptr<QBrush>   brush_;
  std::unique_ptr<QPixmap>  pixmap_;
  uint                      pixmap_width_ { 0 };
  uint                      pixmap_height_ { 0 };
  QPainter*                 rep_painter_ { nullptr };
  QColor                    qfg_;
  QColor                    qbg_;
  uint                      font_id_ { INT_MAX };
  bool                      drawing_ { false };
};

#endif
