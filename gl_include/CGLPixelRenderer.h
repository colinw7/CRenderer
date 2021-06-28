#ifndef CGLPIXEL_RENDERER_H
#define CGLPIXEL_RENDERER_H

#ifndef CPIXEL_RENDERER_VIRTUAL
#define CPIXEL_RENDERER_VIRTUAL 1
#endif

#include <CPixelRenderer.h>
#include <accessor.h>

class CGLWindow;

class CGLPixelRenderer : public CPixelRenderer {
 public:
  CGLPixelRenderer(CGLWindow *window);
 ~CGLPixelRenderer();

  CGLPixelRenderer *dup() const;

  ACCESSOR(Z         , double, z)
  ACCESSOR(ImageScale, double, img_scale)
  ACCESSOR(KeepAspect, bool  , keep_aspect)

  uint getWidth () const;
  uint getHeight() const;

  void updateSize(int, int);

  void startDoubleBuffer(bool);
  void endDoubleBuffer  (bool);
  void copyDoubleBuffer ();

  void setLineWidth(double line_width);

  double getLineWidth();

  void getLineWidthDetails(double *min_width, double *max_width,
                           double *dwidth);

  void setLineDash(const CILineDash &line_dash);

  void setPointSize(double size);
  double getPointSize();

  void getPointSizeDetails(double *min_size, double *max_size, double *dsize);

  void clear();
  void fill();

  void drawClippedPoint(const CIPoint2D &point);

  void drawClippedChar(const CIPoint2D &point, char c);

  void drawClippedString(const CIPoint2D &point, const std::string &str);

  void drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2);

  void drawClippedRectangle(const CIBBox2D &bbox);

  void fillClippedRectangle(const CIBBox2D &bbox);

  void fillClippedTriangle(const CIPoint2D &point1,
                           const CIPoint2D &point2,
                           const CIPoint2D &point3);

  void drawClippedPolygon(const IPointList &points);

  void fillClippedPolygon(const IPointList &points);

  void drawClippedArc(const CIPoint2D &point, int xr, int yr,
                      double a1, double a2);

  void fillClippedArc(const CIPoint2D &point, int xr, int yr,
                      double a1, double a2);

  void drawClippedImage(const CIPoint2D &point, CImagePtr image);

  void drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image);

  void drawClippedScaledImage(const CIPoint2D &point, CImagePtr image,
                              double sx, double sy);

  int getCharWidth  ();
  int getCharAscent ();
  int getCharDescent();
  int getCharHeight ();

  int getStringWidth(const std::string &str);

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  void flush();

  void saveImage(const std::string &fileName);

  void errorPrint();

 private:
  using CPixelRenderer::setLineWidth;

 private:
  CGLWindow *window_      { nullptr };
  double     z_           { 0.0 };
  double     img_scale_   { 1.0 };
  bool       keep_aspect_ { false };
};

#endif
