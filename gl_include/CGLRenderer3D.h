#ifndef CGL_RENDERER_3D_H
#define CGL_RENDERER_3D_H

class CGLPixelRenderer;
class CWindow;
class CGLTexture;
class CPath2D;

struct GLUtesselator;

#include <CGLPixelRenderer.h>
#include <CDisplayRange2D.h>
#include <CMatrix3D.h>
#include <CLine3D.h>
#include <CPlane3D.h>
#include <COptVal.h>

struct CGLPoint2D {
  CPoint2D           vertex;
  COptValT<CRGBA>    color;
  COptValT<CPoint2D> texture;

  CGLPoint2D(const CPoint2D &vertex1) :
   vertex(vertex1) {
  }

  CGLPoint2D(const CPoint2D &vertex1, const CRGBA &color1) :
   vertex(vertex1), color(color1) {
  }

  CGLPoint2D(const CPoint2D &vertex1, const CPoint2D &texture1) :
   vertex(vertex1), texture(texture1) {
  }

  CGLPoint2D(const CPoint2D &vertex1, const CRGBA &color1,
             const CPoint2D &texture1) :
   vertex(vertex1), color(color1), texture(texture1) {
  }
};

struct CGLPoint3D {
  CPoint3D           vertex;
  COptValT<CRGBA>    color;
  COptValT<CPoint2D> texture;

  CGLPoint3D(const CPoint3D &vertex1) :
   vertex(vertex1) {
  }

  CGLPoint3D(const CPoint3D &vertex1, const CRGBA &color1) :
   vertex(vertex1), color(color1) {
  }

  CGLPoint3D(const CPoint3D &vertex1, const CPoint2D &texture1) :
   vertex(vertex1), texture(texture1) {
  }

  CGLPoint3D(const CPoint3D &vertex1, const CRGBA &color1,
             const CPoint2D &texture1) :
   vertex(vertex1), color(color1), texture(texture1) {
  }
};

class CGLRenderer3D {
 public:
  typedef std::vector<CGLPoint3D> PointList;

 public:
  CGLRenderer3D(CGLWindow *window);
 ~CGLRenderer3D();

  CWindow *getWindow() const { return window_; }

  CGLPixelRenderer *getPixelRenderer() const { return renderer_; }

  void updateSize();

  void clear(const CRGBA &bg);

  void flush();

  void setForeground(const CRGBA &rgba);

  CFontPtr getFont() const;

  void setFont(CFontPtr font);

  void drawPoint(const CPoint3D &point);
  void drawPoint(double x, double y, double z);
  void drawPoint(const CPoint3D &point, CGLTexture *texture);

  void drawLine(const CLine3D &line);
  void drawLine(double x1, double y1, double z1,
                double x2, double y2, double z2);

  void fillXYRectangle(double x1, double y1, double x2, double y2, double z,
                       CGLTexture *texture=NULL);
  void fillXZRectangle(double x1, double z1, double x2, double z2, double y,
                       CGLTexture *texture=NULL);
  void fillYZRectangle(double y1, double z1, double y2, double z2, double x,
                       CGLTexture *texture=NULL);
  void fillZYRectangle(double z1, double y1, double z2, double y2, double x,
                       CGLTexture *texture=NULL);

  void fillTriangle(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3);

  void fillTriangleFan(const PointList &points);

  void fillRectangle(const CPoint3D &p1, const CPoint2D &t1,
                     const CPoint3D &p2, const CPoint2D &t2,
                     const CPoint3D &p3, const CPoint2D &t3,
                     const CPoint3D &p4, const CPoint2D &t4,
                     CGLTexture *texture);

  void fillPolygon(const std::vector<CPoint3D> &points);

  void drawSphere(const CPoint3D &p, double r, CGLTexture *texture);

  void drawPlane(const CPlane3D &plane);

  void drawText(double *x, double *y, const char *text);

  void drawPathText(double *x, double *y, const char *text);

  void drawPath(const CPath2D &path);

  void setOverlayPixelRange(int xmin, int ymin, int xmax, int ymax);
  void setOverlayDataRange(double xmin, double ymin, double xmax, double ymax);

  void setOverlayLayer(uint layer);

  void setOverlayZ();

  void drawOverlayRectangle(double x1, double y1, double x2, double y2);
  void fillOverlayRectangle(double x1, double y1, double x2, double y2);

  void fillOverlayTriangle(const CPoint2D &p1, const CPoint2D &p2,
                           const CPoint2D &p3);
  void fillOverlayTriangle(const CGLPoint2D &p1, const CGLPoint2D &p2,
                           const CGLPoint2D &p3);

  void drawOverlayLine(double x1, double y1, double x2, double y2);
  void drawOverlayPoint(double x, double y);

  void drawOverlayText(double x, double y, const std::string &str);

  CImagePtr imageString(const std::string &str);

  void drawOverlayImage(double x, double y, CImagePtr image);

  void drawImage(const CPoint3D &point, CImagePtr image);

  void drawCube(double xc, double yc, double zc, double s);

  void pathMoveTo(const CPoint3D &point);

  void pathLineTo(const CPoint3D &point);

  void pathBezierTo(const CPoint3D &point1, const CPoint3D &point2);

  void pathBezierTo(const CPoint3D &point1, const CPoint3D &point2,
                    const CPoint3D &point3);

  void pathClose();

  void initOverlay();
  void termOverlay();

 private:
  CWindow*              window_ { nullptr };
  CGLPixelRenderer*     renderer_ { nullptr };
  CDisplayRange2D       overlay_range_;
  uint                  overlay_layer_ { 1 };
  GLUtesselator*        tess_ { nullptr };
  CPoint3D              current_;
  std::vector<CPoint3D> pathPoints_;
};

#endif
