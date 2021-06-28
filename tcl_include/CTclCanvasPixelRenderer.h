#ifndef CXLIB_PIXEL_RENDERER_H
#define CXLIB_PIXEL_RENDERER_H

#include <CPixelRenderer.h>

class CTclCanvas;

class CTclCanvasPixelRenderer : public CPixelRenderer {
 public:
  CTclCanvasPixelRenderer(CTclCanvas *canvas);
 ~CTclCanvasPixelRenderer() { }

  CTclCanvasPixelRenderer *dup() const override;

  uint getWidth () const override;
  uint getHeight() const override;

  void updateSize(int, int) override;

  void drawInit();
  void drawTerm();

  void startDoubleBuffer(bool) override;
  void endDoubleBuffer(bool) override;

  void setForeground(const CRGBA &rgb) override;
  void setBackground(const CRGBA &rgb) override;

  void setFont(CFontPtr font) override;
  void getFont(CFontPtr &font) const;

  void clear() override;

  void drawClippedPoint(const CIPoint2D &p) override;

  void drawChar(const CIPoint2D &point, char c) override;

  void drawString(const CIPoint2D &point, const std::string &str) override;

  void drawLine(const CIPoint2D &point1, const CIPoint2D &point2) override;

  void drawRectangle(const CIBBox2D &bbox) override;
  void fillRectangle(const CIBBox2D &bbox) override;

  void drawArc(const CIPoint2D &center, int xr, int yr, double a1, double a2) override;
  void fillArc(const CIPoint2D &center, int xr, int yr, double a1, double a2) override;

  void drawEllipse(const CIPoint2D &center, int xr, int yr) override;
  void fillEllipse(const CIPoint2D &center, int xr, int yr) override;

  void drawImage(const CIPoint2D &point, CImagePtr image) override;

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  int getCharWidth () override;
  int getCharHeight() override;

  CFontPtr lookupFont(const std::string &name, CFontStyle style, int size);

  void flush() override;

 private:
  CTclCanvas* canvas_         { nullptr };
  int         canvas_width1_  { 0 };
  int         canvas_height1_ { 0 };
};

#endif
