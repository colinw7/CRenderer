#include <CTclCanvasPixelRenderer.h>
#include <CTclCanvas.h>
#include <CLine2D.h>

CTclCanvasPixelRenderer::
CTclCanvasPixelRenderer(CTclCanvas *canvas) :
 CPixelRenderer(), canvas_(canvas)
{
  updateSize(100, 100);
}

CTclCanvasPixelRenderer *
CTclCanvasPixelRenderer::
dup() const
{
  return new CTclCanvasPixelRenderer(canvas_);
}

uint
CTclCanvasPixelRenderer::
getWidth() const
{
  return canvas_->getWidth();
}

uint
CTclCanvasPixelRenderer::
getHeight() const
{
  return canvas_->getHeight();
}

void
CTclCanvasPixelRenderer::
updateSize(int, int)
{
  canvas_width1_  = canvas_->getWidth () - 1;
  canvas_height1_ = canvas_->getHeight() - 1;
}

void
CTclCanvasPixelRenderer::
drawInit()
{
  setBackground(CRGBA(0,0,0));
  setForeground(CRGBA(1,1,1));

  startDoubleBuffer();

  clear();
}

void
CTclCanvasPixelRenderer::
drawTerm()
{
  endDoubleBuffer();

  flush();
}

void
CTclCanvasPixelRenderer::
startDoubleBuffer()
{
  canvas_->startDoubleBuffer();
}

void
CTclCanvasPixelRenderer::
endDoubleBuffer()
{
  canvas_->endDoubleBuffer();
}

void
CTclCanvasPixelRenderer::
setForeground(const CRGBA &fg)
{
  canvas_->setForeground(fg.getRGB());
}

void
CTclCanvasPixelRenderer::
setBackground(const CRGBA &bg)
{
  canvas_->setBackground(bg.getRGB());
}

void
CTclCanvasPixelRenderer::
setFont(CFontPtr font)
{
  canvas_->setFont(font);
}

void
CTclCanvasPixelRenderer::
getFont(CFontPtr &font) const
{
  canvas_->getFont(font);
}

void
CTclCanvasPixelRenderer::
clear()
{
  canvas_->clear();
}

void
CTclCanvasPixelRenderer::
drawClippedPoint(const CIPoint2D &p)
{
  canvas_->drawPoint(p.x, p.y);
}

void
CTclCanvasPixelRenderer::
drawChar(int x, int y, char c)
{
  std::string str;

  str += c;

  canvas_->drawText(x, y, str);
}

void
CTclCanvasPixelRenderer::
drawString(int x, int y, const std::string &str)
{
  canvas_->drawText(x, y, str);
}

void
CTclCanvasPixelRenderer::
drawLine(int x1, int y1, int x2, int y2)
{
  CLine2D line(x1, y1, x2, y2);

  CBBox2D bbox(0, 0, canvas_width1_, canvas_height1_);

  CLine2D cline;

  if (line.clip(bbox, cline))
    canvas_->drawLine(cline.start().x, cline.start().y,
                      cline.end  ().x, cline.end  ().y);
}

void
CTclCanvasPixelRenderer::
drawRectangle(int x1, int y1, int x2, int y2)
{
  canvas_->drawRectangle(x1, y1, x2 - x1, y2 - y1);
}

void
CTclCanvasPixelRenderer::
fillRectangle(int x1, int y1, int x2, int y2)
{
  canvas_->fillRectangle(x1, y1, x2 - x1, y2 - y1);
}

void
CTclCanvasPixelRenderer::
drawArc(int x, int y, int xr, int yr, double a1, double a2)
{
  canvas_->drawArc(x, y, xr, yr, a1, a2);
}

void
CTclCanvasPixelRenderer::
fillArc(int x, int y, int xr, int yr, double a1, double a2)
{
  canvas_->fillArc(x, y, xr, yr, a1, a2);
}

void
CTclCanvasPixelRenderer::
drawEllipse(int x, int y, int xr, int yr)
{
  canvas_->drawEllipse(x, y, xr, yr);
}

void
CTclCanvasPixelRenderer::
fillEllipse(int x, int y, int xr, int yr)
{
  canvas_->fillEllipse(x, y, xr, yr);
}

void
CTclCanvasPixelRenderer::
drawImage(int x, int y, CImagePtr image)
{
  canvas_->drawImage(image, x, y);
}

void
CTclCanvasPixelRenderer::
getClip(int *xmin, int *ymin, int *xmax, int *ymax)
{
  *xmin = 0;
  *ymin = 0;
  *xmax = canvas_width1_;
  *ymax = canvas_height1_;
}

int
CTclCanvasPixelRenderer::
getCharWidth()
{
  CFontPtr font;

  getFont(font);

  return font->getICharWidth();
}

int
CTclCanvasPixelRenderer::
getCharHeight()
{
  CFontPtr font;

  getFont(font);

  return font->getICharHeight();
}

void
CTclCanvasPixelRenderer::
flush()
{
  canvas_->flush();
}
