#ifndef CQ_IMAGE_RENDERER_2D_H
#define CQ_IMAGE_RENDERER_2D_H

#include <CQRenderer2D.h>
#include <CAutoPtr.h>

class QImage;
class CQImagePixelRenderer;

class CQImageRenderer2D : public CQRenderer2D {
 private:
  CAutoPtr<CQImagePixelRenderer> pixel_renderer_;

 public:
  CQImageRenderer2D(int width=100, int height=100);
 ~CQImageRenderer2D();

  CQImageRenderer2D *dup() const;

  CQImagePixelRenderer *getQImagePixelRenderer() const { return pixel_renderer_; }

  int getWidth ();
  int getHeight();

  QImage *getQImage() const;

  CPixelRenderer *getPixelRenderer() const;

  CImagePtr getImage();
  CImagePtr getAlphaImage();

  void setImage(CImagePtr image);
  void setAlphaImage(CImagePtr image);

  void updateSize(int width, int height);

  void beginDraw();
  void endDraw();

  void drawTextInRect(const CBBox2D &rect, const std::string &text);
};

#endif
