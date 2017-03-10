#include <CQWidgetRenderer2D.h>
#include <CQWidgetPixelRenderer.h>
#include <CQImageRenderer2D.h>
#include <CQImage.h>
#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>

CQWidgetRenderer2D::
CQWidgetRenderer2D(QWidget *widget) :
 CQRenderer2D()
{
  pixel_renderer_ = new CQWidgetPixelRenderer(widget);

  setPixelRenderer(pixel_renderer_);

  pixel_renderer_->setQPainter(getQPainter());
}

CQWidgetRenderer2D::
~CQWidgetRenderer2D()
{
}

CQWidgetRenderer2D *
CQWidgetRenderer2D::
dup() const
{
  return new CQWidgetRenderer2D(pixel_renderer_->getQWidget());
}

QImage *
CQWidgetRenderer2D::
getQImage() const
{
  return NULL;
}

CPixelRenderer *
CQWidgetRenderer2D::
getPixelRenderer() const
{
  return pixel_renderer_;
}

CImagePtr
CQWidgetRenderer2D::
getImage()
{
  return pixel_renderer_->getImage();
}

CImagePtr
CQWidgetRenderer2D::
getAlphaImage()
{
  return pixel_renderer_->getImage();
}

int
CQWidgetRenderer2D::
getWidth()
{
  return pixel_renderer_->getWidth();
}

int
CQWidgetRenderer2D::
getHeight()
{
  return pixel_renderer_->getHeight();
}

void
CQWidgetRenderer2D::
updateSize(int width, int height)
{
  pixel_renderer_->updateSize(width, height);

  setPixelRange(0, 0, getWidth() - 1, getHeight() - 1);
}

void
CQWidgetRenderer2D::
beginDraw()
{
  getQPainter()->begin(pixel_renderer_->getQWidget());

  CQRenderer2D::beginDraw();
}

void
CQWidgetRenderer2D::
endDraw()
{
  getQPainter()->end();

  CQRenderer2D::endDraw();
}
