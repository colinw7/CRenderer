#include <CGLWindow.h>
#include <CGLRenderer2D.h>

CGLRenderer2D::
CGLRenderer2D(CGLWindow *window) :
 CRenderer2D(NULL), window_(window), renderer_(NULL)
{
  renderer_ = new CGLPixelRenderer(window_);

  setPixelRenderer(renderer_);
}

CGLRenderer2D::
~CGLRenderer2D()
{
  delete renderer_;
}

CGLRenderer2D *
CGLRenderer2D::
dup() const
{
  return new CGLRenderer2D(window_);
}

CGLPixelRenderer *
CGLRenderer2D::
getPixelRenderer() const
{
  return renderer_;
}

void
CGLRenderer2D::
updateSize(int width, int height)
{
  renderer_->updateSize(width, height);

  setPixelRange(0, 0, renderer_->getWidth() - 1, renderer_->getHeight() - 1);
}

void
CGLRenderer2D::
drawLine(const CPoint2D &point1, const CPoint2D &point2)
{
  CRenderer2D::drawLine(point1, point2);
}

void
CGLRenderer2D::
fillPolygon(const PointList &points)
{
  CRenderer2D::fillPolygon(points);
}

void
CGLRenderer2D::
drawPolygon(const PointList &points)
{
  CRenderer2D::drawPolygon(points);
}

void
CGLRenderer2D::
drawText(const CPoint2D &point, const std::string &str)
{
  CRenderer2D::drawText(point, str);
}

void
CGLRenderer2D::
fillText(const CPoint2D &point, const std::string &str)
{
  CRenderer2D::fillText(point, str);
}

void
CGLRenderer2D::
drawImage(const CPoint2D &point, CImagePtr image)
{
  CRenderer2D::drawImage(point, image);
}

void
CGLRenderer2D::
drawAlphaImage(const CPoint2D &point, CImagePtr image)
{
  CRenderer2D::drawAlphaImage(point, image);
}

void
CGLRenderer2D::
drawImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CRenderer2D::drawImageInBox(bbox, image);
}

void
CGLRenderer2D::
drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CRenderer2D::drawAlphaImageInBox(bbox, image);
}

void
CGLRenderer2D::
drawPoint(const CPoint2D &point)
{
  CRenderer2D::drawPoint(point);
}

void
CGLRenderer2D::
fillRectangle(const CBBox2D &bbox)
{
  CRenderer2D::fillRectangle(bbox);
}

void
CGLRenderer2D::
gradientFillRectangle(const CBBox2D &bbox, CGenGradient *gradient)
{
  CRenderer2D::gradientFillRectangle(bbox, gradient);
}

void
CGLRenderer2D::
drawRectangle(const CBBox2D &bbox)
{
  CRenderer2D::drawRectangle(bbox);
}

void
CGLRenderer2D::
fillTriangle(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  std::vector<CPoint2D> points;

  points.push_back(p1);
  points.push_back(p2);
  points.push_back(p3);

  CRenderer2D::fillPolygon(points);
}
