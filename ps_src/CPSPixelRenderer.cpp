#include <CPSPixelRenderer.h>

CPSPixelRenderer::
CPSPixelRenderer(CPixelRenderer *prenderer) :
 CPixelRenderer(), prenderer_(prenderer), size_(0,0)
{
  int xmin = 0;
  int ymin = 0;
  int xmax = getWidth () - 1;
  int ymax = getHeight() - 1;

  print_.setSize(xmin, ymin, xmax, ymax);
}

CPSPixelRenderer::
CPSPixelRenderer(const CISize2D &size) :
 CPixelRenderer(), prenderer_(NULL), size_(size)
{
  int xmin = 0;
  int ymin = 0;
  int xmax = getWidth () - 1;
  int ymax = getHeight() - 1;

  print_.setSize(xmin, ymin, xmax, ymax);
}

CPSPixelRenderer::
~CPSPixelRenderer()
{
}

CPSPixelRenderer *
CPSPixelRenderer::
dup() const
{
  return new CPSPixelRenderer(prenderer_);
}

CPixelRenderer *
CPSPixelRenderer::
getPixelRenderer() const
{
  return prenderer_;
}

uint
CPSPixelRenderer::
getWidth() const
{
  if (prenderer_)
    return prenderer_->getWidth();
  else
    return size_.getWidth();
}

uint
CPSPixelRenderer::
getHeight() const
{
  if (prenderer_)
    return prenderer_->getHeight();
  else
    return size_.getHeight();
}

void
CPSPixelRenderer::
updateSize(int,int)
{
  int xmin = 0;
  int ymin = 0;
  int xmax = getWidth () - 1;
  int ymax = getHeight() - 1;

  print_.setSize(xmin, ymin, xmax, ymax);
}

void
CPSPixelRenderer::
startDoubleBuffer(bool)
{
  print_.init();
}

void
CPSPixelRenderer::
endDoubleBuffer(bool)
{
  print_.term();
}

void
CPSPixelRenderer::
setForeground(const CRGBA &fg)
{
  print_.setForeground(fg.getRGB());

  CPixelRenderer::setForeground(fg);
}

void
CPSPixelRenderer::
setBackground(const CRGBA &bg)
{
  print_.setBackground(bg.getRGB());

  CPixelRenderer::setBackground(bg);
}

void
CPSPixelRenderer::
clear()
{
  print_.clear();
}

void
CPSPixelRenderer::
drawClippedPoint(const CIPoint2D &point)
{
  print_.drawPoint(point.x, point.y);
}

void
CPSPixelRenderer::
drawClippedChar(const CIPoint2D &point, char c)
{
  std::string str;

  str += c;

  print_.drawText(point.x, point.y, str);
}

void
CPSPixelRenderer::
drawClippedString(const CIPoint2D &point, const std::string &str)
{
  print_.drawText(point.x, point.y, str);
}

void
CPSPixelRenderer::
drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2)
{
  static CILineDash solid_line;

  if (! getLineDash().isSolid())
    print_.setLineDash(getLineDash());

  print_.drawLine(point1.x, point1.y, point2.x, point2.y);

  if (! getLineDash().isSolid())
    print_.setLineDash(solid_line);
}

void
CPSPixelRenderer::
drawClippedRectangle(const CIBBox2D &bbox)
{
  print_.drawRectangle(bbox.getXMin(), bbox.getYMin(),
                       bbox.getWidth(), bbox.getHeight());
}

void
CPSPixelRenderer::
fillClippedRectangle(const CIBBox2D &bbox)
{
  print_.fillRectangle(bbox.getXMin(), bbox.getYMin(),
                       bbox.getWidth(), bbox.getHeight());
}

void
CPSPixelRenderer::
fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2,
                    const CIPoint2D &point3)
{
  CPixelRenderer::fillClippedTriangle(point1, point2, point3);
}

void
CPSPixelRenderer::
drawClippedPolygon(const IPointList &points)
{
  CPixelRenderer::drawClippedPolygon(points);
}

void
CPSPixelRenderer::
fillClippedPolygon(const IPointList &points)
{
  CPixelRenderer::fillClippedPolygon(points);
}

void
CPSPixelRenderer::
drawClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  print_.drawArc(center.x, center.y, xr, yr, a1, a2);
}

void
CPSPixelRenderer::
fillClippedArc(const CIPoint2D &center, int xr, int yr, double a1, double a2)
{
  print_.fillArc(center.x, center.y, xr, yr, a1, a2);
}

void
CPSPixelRenderer::
drawImage(const CIPoint2D &p, CImagePtr image)
{
  print_.drawImage(image, p.x, p.y);
}

void
CPSPixelRenderer::
drawClippedImage(const CIPoint2D &p, CImagePtr image)
{
  print_.drawImage(image, p.x, p.y);
}

void
CPSPixelRenderer::
drawAlphaImage(const CIPoint2D &p, CImagePtr image)
{
  print_.drawImage(image, p.x, p.y);
}

void
CPSPixelRenderer::
drawClippedAlphaImage(const CIPoint2D &p, CImagePtr image)
{
  print_.drawImage(image, p.x, p.y);
}

void
CPSPixelRenderer::
getClip(int *xmin, int *ymin, int *xmax, int *ymax)
{
  *xmin = 0;
  *ymin = 0;
  *xmax = getWidth ();
  *ymax = getHeight();
}

int
CPSPixelRenderer::
getCharWidth()
{
  CFontPtr font;

  print_.getFont(font);

  if (font.isValid())
    return font->getICharWidth();
  else
    return 8;
}

int
CPSPixelRenderer::
getCharHeight()
{
  CFontPtr font;

  print_.getFont(font);

  if (font.isValid())
    return font->getICharHeight();
  else
    return 12;
}

int
CPSPixelRenderer::
getCharAscent()
{
  CFontPtr font;

  print_.getFont(font);

  if (font.isValid())
    return font->getICharAscent();
  else
    return 12;
}

int
CPSPixelRenderer::
getCharDescent()
{
  CFontPtr font;

  print_.getFont(font);

  if (font.isValid())
    return font->getICharDescent();
  else
    return 0;
}

int
CPSPixelRenderer::
getStringWidth(const std::string &str)
{
  CFontPtr font;

  print_.getFont(font);

  if (font.isValid())
    return font->getIStringWidth(str);
  else
    return str.size()*getCharWidth();
}

void
CPSPixelRenderer::
flush()
{
}

bool
CPSPixelRenderer::
mmToPixel(double mm, double *pixel)
{
  static double ppmm = 72/25.4;

  *pixel= mm*ppmm;

  return true;
}
