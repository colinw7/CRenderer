#ifndef CGL_RENDERER_2D_H
#define CGL_RENDERER_2D_H

#define CRENDERER_2D_VIRTUAL

#include <CRenderer2D.h>
#include <CGLPixelRenderer.h>

class CGLWindow;

class CGLRenderer2D : public CRenderer2D {
 public:
  CGLRenderer2D(CGLWindow *window);
 ~CGLRenderer2D();

  CGLRenderer2D *dup() const;

  CGLPixelRenderer *getRenderer2D() const { return renderer_; }

  CGLPixelRenderer *getPixelRenderer() const;

  void updateSize(int, int);

  virtual void drawLine(const CPoint2D &point1, const CPoint2D &point2);

  virtual void fillPolygon(const PointList &points);

  virtual void drawPolygon(const PointList &points);

  virtual void drawText(const CPoint2D &point, const std::string &str);

  virtual void fillText(const CPoint2D &point, const std::string &str);

  virtual void drawImage(const CPoint2D &point, CImagePtr image);

  virtual void drawAlphaImage(const CPoint2D &point, CImagePtr image);

  virtual void drawImageInBox(const CBBox2D &point, CImagePtr image);

  virtual void drawAlphaImageInBox(const CBBox2D &point, CImagePtr image);

  virtual void drawPoint(const CPoint2D &point);

  virtual void fillRectangle(const CBBox2D &bbox);

  virtual void gradientFillRectangle(const CBBox2D &bbox, CGenGradient *gradient);

  virtual void drawRectangle(const CBBox2D &bbox);

  virtual void fillTriangle(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3);

 private:
  CGLWindow        *window_ { nullptr };
  CGLPixelRenderer *renderer_ { nullptr };
};

#endif
