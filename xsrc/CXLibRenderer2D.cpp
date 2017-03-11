#include <CXLibRenderer2D.h>
#include <CXLibPath2DRenderer.h>
#include <CXWindow.h>

CXLibRenderer2D::
CXLibRenderer2D(CXWindow *window) :
 CRenderer2D(renderer_ = new CXLibPixelRenderer(window)), window_(window)
{
}

CXLibRenderer2D::
~CXLibRenderer2D()
{
  delete renderer_;
}

CXLibRenderer2D *
CXLibRenderer2D::
dup() const
{
  return new CXLibRenderer2D(window_);
}

void
CXLibRenderer2D::
updateSize(int width, int height)
{
  renderer_->updateSize(width, height);

  setPixelRange(0, 0, renderer_->getWidth() - 1, renderer_->getHeight() - 1);
}

void
CXLibRenderer2D::
drawLine(const CPoint2D &point1, const CPoint2D &point2)
{
  CRenderer2D::drawLine(point1, point2);
}

void
CXLibRenderer2D::
fillPolygon(const PointList &points)
{
  CRenderer2D::fillPolygon(points);
}

void
CXLibRenderer2D::
drawPolygon(const PointList &points)
{
  CRenderer2D::drawPolygon(points);
}

void
CXLibRenderer2D::
drawText(const CPoint2D &point, const std::string &str)
{
  CRenderer2D::drawText(point, str);
}

void
CXLibRenderer2D::
fillText(const CPoint2D &point, const std::string &str)
{
  CRenderer2D::fillText(point, str);
}

void
CXLibRenderer2D::
drawImage(const CPoint2D &point, CImagePtr image)
{
  CRenderer2D::drawImage(point, image);
}

void
CXLibRenderer2D::
drawAlphaImage(const CPoint2D &point, CImagePtr image)
{
  CRenderer2D::drawAlphaImage(point, image);
}

void
CXLibRenderer2D::
drawImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CRenderer2D::drawImageInBox(bbox, image);
}

void
CXLibRenderer2D::
drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CRenderer2D::drawAlphaImageInBox(bbox, image);
}

void
CXLibRenderer2D::
drawPoint(const CPoint2D &point)
{
  CRenderer2D::drawPoint(point);
}

void
CXLibRenderer2D::
fillRectangle(const CBBox2D &bbox)
{
  CRenderer2D::fillRectangle(bbox);
}

void
CXLibRenderer2D::
gradientFillRectangle(const CBBox2D &bbox, CGenGradient *gradient)
{
  CRenderer2D::gradientFillRectangle(bbox, gradient);
}

void
CXLibRenderer2D::
drawRectangle(const CBBox2D &bbox)
{
  CRenderer2D::drawRectangle(bbox);
}

CPath2DRenderer *
CXLibRenderer2D::
createPathRenderer() const
{
  return new CXLibPath2DRenderer(const_cast<CXLibRenderer2D *>(this));
}
