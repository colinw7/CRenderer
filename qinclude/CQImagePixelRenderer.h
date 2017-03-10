#ifndef CQIMAGE_PIXEL_RENDERER_H
#define CQIMAGE_PIXEL_RENDERER_H

#include <CQPixelRenderer.h>
#include <CAutoPtr.h>

class QImage;

class CQImagePixelRenderer : public CQPixelRenderer {
 private:
  CImagePtr image_;

 public:
  CQImagePixelRenderer(int width, int height);
 ~CQImagePixelRenderer();

  CQImagePixelRenderer *dup() const;

  uint getWidth () const;
  uint getHeight() const;

  QImage getQImage() const;
  QImage *getQImageP() const;

  CImagePtr getImage();
  CImagePtr getAlphaImage();

  void setImage(CImagePtr image);
  void setAlphaImage(CImagePtr image);

  void updateSize(int width, int height);

  void beginDraw();
  void endDraw();
};

#endif
