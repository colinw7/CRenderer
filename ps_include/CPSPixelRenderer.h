#ifndef CPS_PIXEL_RENDERER_H
#define CPS_PIXEL_RENDERER_H

#include <CPrint.h>
#include <CILineDash.h>

#define CPIXEL_RENDERER_VIRTUAL

#include <CPixelRenderer.h>

class CPSPixelRenderer : public CPixelRenderer {
 private:
  CPixelRenderer *prenderer_;
  CISize2D        size_;
  CPrint          print_;

 public:
  CPSPixelRenderer(CPixelRenderer *prenderer_);
  CPSPixelRenderer(const CISize2D &size);
 ~CPSPixelRenderer();

  CPSPixelRenderer *dup() const;

  CPixelRenderer *getPixelRenderer() const;

  uint getWidth () const;
  uint getHeight() const;

  void updateSize(int, int);

  void startDoubleBuffer(bool clear=true);
  void endDoubleBuffer(bool copy=true);

  void setForeground(const CRGBA &rgb);
  void setBackground(const CRGBA &rgb);

  void clear();

  void drawClippedPoint(const CIPoint2D &point);

  void drawClippedChar(const CIPoint2D &point, char c);
  void drawClippedString(const CIPoint2D &point, const std::string &str);

  void drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2);

  void drawClippedRectangle(const CIBBox2D &bbox);
  void fillClippedRectangle(const CIBBox2D &bbox);

  void fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                           const CIPoint2D &point3);

  void drawClippedPolygon(const IPointList &points);
  void fillClippedPolygon(const IPointList &points);

  void drawClippedArc(const CIPoint2D &p, int xr, int yr, double a1, double a2);
  void fillClippedArc(const CIPoint2D &p, int xr, int yr, double a1, double a2);

  void drawImage(const CIPoint2D &p, CImagePtr image);
  void drawClippedImage(const CIPoint2D &p, CImagePtr image);

  void drawAlphaImage(const CIPoint2D &p, CImagePtr image);
  void drawClippedAlphaImage(const CIPoint2D &p, CImagePtr image);

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  int getCharWidth ();
  int getCharAscent();
  int getCharDescent();
  int getCharHeight();

  int getStringWidth(const std::string &str);

  void flush();

  bool mmToPixel(double mm, double *pixel);
};

#endif
