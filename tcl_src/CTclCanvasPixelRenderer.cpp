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

  startDoubleBuffer(false);

  clear();
}

void
CTclCanvasPixelRenderer::
drawTerm()
{
  endDoubleBuffer(false);

  flush();
}

void
CTclCanvasPixelRenderer::
startDoubleBuffer(bool)
{
  canvas_->startDoubleBuffer();
}

void
CTclCanvasPixelRenderer::
endDoubleBuffer(bool)
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
drawChar(const CIPoint2D &point, char c)
{
  std::string str;

  str += c;

  canvas_->drawText(point.x, point.y, str);
}

void
CTclCanvasPixelRenderer::
drawString(const CIPoint2D &point, const std::string &str)
{
  canvas_->drawText(point.x, point.y, str);
}

void
CTclCanvasPixelRenderer::
drawLine(const CIPoint2D &point1, const CIPoint2D &point2)
{
  CLine2D line(point1.x, point1.y, point2.x, point2.y);

  CBBox2D bbox(0, 0, canvas_width1_, canvas_height1_);

  CLine2D cline;

  if (line.clip(bbox, cline))
    canvas_->drawLine(cline.start().x, cline.start().y,
                      cline.end  ().x, cline.end  ().y);
}

void
CTclCanvasPixelRenderer::
drawRectangle(const CIBBox2D &bbox)
{
  canvas_->drawRectangle(bbox.getXMin(), bbox.getYMin(),
                         bbox.getXMax() - bbox.getXMin(), bbox.getYMax() - bbox.getYMin());
}

void
CTclCanvasPixelRenderer::
fillRectangle(const CIBBox2D &bbox)
{
  canvas_->fillRectangle(bbox.getXMin(), bbox.getYMin(),
                         bbox.getXMax() - bbox.getXMin(), bbox.getYMax() - bbox.getYMin());
}

void
CTclCanvasPixelRenderer::
drawArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  canvas_->drawArc(center.x, center.y, xr, yr, a1, a2);
}

void
CTclCanvasPixelRenderer::
fillArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  canvas_->fillArc(center.x, center.y, xr, yr, a1, a2);
}

void
CTclCanvasPixelRenderer::
drawEllipse(const CIPoint2D &center, int xr, int yr)
{
  canvas_->drawEllipse(center.x, center.y, xr, yr);
}

void
CTclCanvasPixelRenderer::
fillEllipse(const CIPoint2D &center, int xr, int yr)
{
  canvas_->fillEllipse(center.x, center.y, xr, yr);
}

void
CTclCanvasPixelRenderer::
drawImage(const CIPoint2D &point, CImagePtr image)
{
  canvas_->drawImage(image, point.x, point.y);
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
