#include <CQImageRenderer2D.h>
#include <CQImagePixelRenderer.h>
#include <CQImage.h>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QPainter>

CQImageRenderer2D::
CQImageRenderer2D(int width, int height) :
 CQRenderer2D()
{
  pixel_renderer_ = new CQImagePixelRenderer(width, height);

  setPixelRenderer(pixel_renderer_);
}

CQImageRenderer2D::
~CQImageRenderer2D()
{
}

CQImageRenderer2D *
CQImageRenderer2D::
dup() const
{
  return new CQImageRenderer2D(pixel_renderer_->getWidth(), pixel_renderer_->getHeight());
}

QImage *
CQImageRenderer2D::
getQImage() const
{
  return pixel_renderer_->getQImageP();
}

CPixelRenderer *
CQImageRenderer2D::
getPixelRenderer() const
{
  return pixel_renderer_;
}

CImagePtr
CQImageRenderer2D::
getImage()
{
  return pixel_renderer_->getImage();
}

CImagePtr
CQImageRenderer2D::
getAlphaImage()
{
  return pixel_renderer_->getImage();
}

void
CQImageRenderer2D::
setImage(CImagePtr image)
{
  double xmin, ymin, xmax, ymax;

  getDataRange(&xmin, &ymin, &xmax, &ymax);

  CMatrix2D view_matrix = *getViewMatrix();

  resetDataRange();

  unsetViewMatrix();

  //pixel_renderer_->setImage(image);
  drawImage(CPoint2D(0, 0), image);

  setViewMatrix(view_matrix);

  setDataRange(xmin, ymin, xmax, ymax);
}

void
CQImageRenderer2D::
setAlphaImage(CImagePtr image)
{
  double xmin, ymin, xmax, ymax;

  getDataRange(&xmin, &ymin, &xmax, &ymax);

  CMatrix2D view_matrix = *getViewMatrix();

  resetDataRange();

  unsetViewMatrix();

  //pixel_renderer_->setImage(image);
  drawAlphaImage(CPoint2D(0, 0), image);

  setViewMatrix(view_matrix);

  setDataRange(xmin, ymin, xmax, ymax);
}

int
CQImageRenderer2D::
getWidth()
{
  return pixel_renderer_->getWidth();
}

int
CQImageRenderer2D::
getHeight()
{
  return pixel_renderer_->getHeight();
}

void
CQImageRenderer2D::
updateSize(int width, int height)
{
  pixel_renderer_->updateSize(width, height);

  setPixelRange(0, 0, width - 1, height - 1);
}

void
CQImageRenderer2D::
beginDraw()
{
  getQPainter()->begin(getQImage());

  CQRenderer2D::beginDraw();
}

void
CQImageRenderer2D::
endDraw()
{
  getQPainter()->end();

  CQRenderer2D::endDraw();
}

void
CQImageRenderer2D::
drawTextInRect(const CBBox2D &rect, const std::string &text)
{
  CPoint2D ll, ur;

  pixelToWindow(rect.getLL(), ll);
  pixelToWindow(rect.getUR(), ur);

  int h = getHeight();

  QTransform transform = getQPainter()->worldTransform();

  QTransform transform1 = transform;

  transform1.scale(1, -1);

  getQPainter()->setWorldTransform(transform1);

  pixel_renderer_->setQPainter(getQPainter());

  pixel_renderer_->drawStringInRect(
    CIBBox2D(ll.x, ll.y - h, ur.x, ur.y - h), text);
#if 0
  int flags = Qt::AlignCenter;

  QRect qrect(ll.x, ll.y - h, fabs(ur.x - ll.x), fabs(ur.y - ll.y));

  QString qstr(text.c_str());

  getQPainter()->drawText(qrect, flags, qstr);
#endif

  pixel_renderer_->setQPainter(NULL);

  getQPainter()->setWorldTransform(transform);
}
