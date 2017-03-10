#include <CQImagePixelRenderer.h>
#include <CQImage.h>
#include <CImageLib.h>
#include <QPainter>

CQImagePixelRenderer::
CQImagePixelRenderer(int width, int height) :
 CQPixelRenderer()
{
  image_ = CImageMgrInst->createImage();

  image_->setDataSize(width, height);
}

CQImagePixelRenderer::
~CQImagePixelRenderer()
{
}

CQImagePixelRenderer *
CQImagePixelRenderer::
dup() const
{
  return new CQImagePixelRenderer(getWidth(), getHeight());
}

QImage
CQImagePixelRenderer::
getQImage() const
{
  CQImage *qimage = image_.cast<CQImage>();

  return qimage->getQImage();
}

QImage *
CQImagePixelRenderer::
getQImageP() const
{
  CQImage *qimage = image_.cast<CQImage>();

  return qimage->getQImageP();
}

CImagePtr
CQImagePixelRenderer::
getImage()
{
  CQImage *qimage = image_.cast<CQImage>();

  qimage->updateCImage();

  return image_;
}

CImagePtr
CQImagePixelRenderer::
getAlphaImage()
{
  return getImage();
}

void
CQImagePixelRenderer::
setImage(CImagePtr image)
{
  // TODO:
  //CQImage *qimage1 = image_.cast<CQImage>();
  //CQImage *qimage2 = image .cast<CQImage>();

  //*qimage1 = *qimage2;

  image_ = image;
}

void
CQImagePixelRenderer::
setAlphaImage(CImagePtr image)
{
  setImage(image);
}

uint
CQImagePixelRenderer::
getWidth() const
{
  return image_->getWidth();
}

uint
CQImagePixelRenderer::
getHeight() const
{
  return image_->getHeight();
}

void
CQImagePixelRenderer::
updateSize(int width, int height)
{
  image_->setDataSize(width, height);
}

void
CQImagePixelRenderer::
beginDraw()
{
  CQImage *qimage = image_.cast<CQImage>();

  painter_->begin(qimage->getQImageP());

  CQPixelRenderer::beginDraw();
}

void
CQImagePixelRenderer::
endDraw()
{
  painter_->end();

  CQPixelRenderer::endDraw();
 }
