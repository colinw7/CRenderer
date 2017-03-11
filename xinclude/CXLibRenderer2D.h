#ifndef CXLIB_RENDERER_2D_H
#define CXLIB_RENDERER_2D_H

#include <CRenderer2D.h>
#include <CXLibPixelRenderer.h>

class CXWindow;

class CXLibRenderer2D : public CRenderer2D {
 protected:
  CXWindow           *window_;
  CXLibPixelRenderer *renderer_;

 public:
  CXLibRenderer2D(CXWindow *window);

  virtual ~CXLibRenderer2D();

  CXLibRenderer2D *dup() const;

  CPixelRenderer *getPixelRenderer() const { return renderer_; }

  CXLibPixelRenderer *getXRenderer() const { return renderer_; }

  void updateSize(int width, int height);

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

  CPath2DRenderer *createPathRenderer() const;
};

#endif
