#ifndef CXLIB_PIXEL_RENDERER_H
#define CXLIB_PIXEL_RENDERER_H

#include <CILineDash.h>
#include <CImagePtr.h>

#define CPIXEL_RENDERER_VIRTUAL

#include <CPixelRenderer.h>

class CXWindow;

class CXLibPixelRenderer : public CPixelRenderer {
 private:
  CXWindow *window_;
  CISize2D  window_size_;

 public:
  CXLibPixelRenderer(CXWindow *window);
 ~CXLibPixelRenderer() { }

  CXLibPixelRenderer *dup() const override;

  uint getWidth () const override { return window_size_.getWidth (); }
  uint getHeight() const override { return window_size_.getHeight(); }

  void setWindow(CXWindow *window);

  void updateSize(int width, int height) override;

  void startDoubleBuffer(bool clear=true) override;
  void endDoubleBuffer(bool copy=true) override;
  void copyDoubleBuffer() override;

  void setForeground(const CRGBA &rgba) override;
  void setBackground(const CRGBA &rgba) override;

  void setFont(CFontPtr font) override;

  void clear() override;

  void fill() override;

  void drawClippedPoint(const CIPoint2D &point) override;

  void drawClippedChar(const CIPoint2D &point, char c) override;

  void drawClippedCharToRenderer(CPixelRenderer *renderer,
                                 const CIPoint2D &point, char c);

  void drawClippedString(const CIPoint2D &point, const std::string &str) override;

  void drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2) override;

  void drawClippedRectangle(const CIBBox2D &bbox) override;
  void fillClippedRectangle(const CIBBox2D &bbox) override;

  void fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                           const CIPoint2D &point3) override;

  void drawClippedPolygon(const IPointList &points) override;
  void fillClippedPolygon(const IPointList &points) override;

  void drawClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2) override;
  void fillClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2) override;

  void drawClippedImage(const CIPoint2D &point, CImagePtr image) override;

  void drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image) override;

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  int getCharWidth () override;
  int getCharAscent() override;
  int getCharDescent() override;

  int getCharHeight() override;

  int getStringWidth(const std::string &str) override;

  int getCharWidth(char c) override {
    std::string str(&c, 1);

    return getStringWidth(str);
  }

  void flush() override;

  bool mmToPixel(double mm, double *pixel) override;
};

#endif
