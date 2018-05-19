#ifndef CXLIB_PIXEL_RENDERER_H
#define CXLIB_PIXEL_RENDERER_H

#include <CPixelRenderer.h>

class CTclCanvas;

class CTclCanvasPixelRenderer : public CPixelRenderer {
 public:
  CTclCanvasPixelRenderer(CTclCanvas *canvas);
 ~CTclCanvasPixelRenderer() { }

  CTclCanvasPixelRenderer *dup() const;

  uint getWidth () const;
  uint getHeight() const;

  void updateSize(int, int);

  void drawInit();
  void drawTerm();

  void startDoubleBuffer();
  void endDoubleBuffer();

  void setForeground(const CRGBA &rgb);
  void setBackground(const CRGBA &rgb);

  void setFont(CFontPtr font);
  void getFont(CFontPtr &font) const;

  void clear();

  void drawClippedPoint(const CIPoint2D &p);

  void drawChar(int x, int y, char c);
  void drawString(int x, int y, const std::string &str);

  void drawLine(int x1, int y1, int x2, int y2);

  void drawRectangle(int x1, int y1, int x2, int y2);
  void fillRectangle(int x1, int y1, int x2, int y2);

  void drawArc(int x, int y, int xr, int yr, double a1, double a2);
  void fillArc(int x, int y, int xr, int yr, double a1, double a2);

  void drawEllipse(int x, int y, int xr, int yr);
  void fillEllipse(int x, int y, int xr, int yr);

  void drawImage(int x, int y, CImagePtr image);

  void getClip(int *xmin, int *ymin, int *xmax, int *ymax);

  int getCharWidth ();
  int getCharHeight();

  CFontPtr lookupFont(const std::string &name, CFontStyle style, int size);

  void flush();

 private:
  CTclCanvas* canvas_         { nullptr };
  int         canvas_width1_  { 0 };
  int         canvas_height1_ { 0 };
};

#endif
