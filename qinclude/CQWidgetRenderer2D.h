#ifndef CQWIDGET_RENDERER_2D_H
#define CQWIDGET_RENDERER_2D_H

#include <CQRenderer2D.h>
#include <CAutoPtr.h>

class QImage;
class QWidget;

class CQWidgetPixelRenderer;

class CQWidgetRenderer2D : public CQRenderer2D {
 private:
  CAutoPtr<CQWidgetPixelRenderer> pixel_renderer_;

 public:
  CQWidgetRenderer2D(QWidget *widget);
 ~CQWidgetRenderer2D();

  CQWidgetRenderer2D *dup() const;

  CQWidgetPixelRenderer *getQWidgetPixelRenderer() const { return pixel_renderer_; }

  int getWidth ();
  int getHeight();

  QImage *getQImage() const;

  CPixelRenderer *getPixelRenderer() const;

  CImagePtr getImage();
  CImagePtr getAlphaImage();

  void updateSize(int width, int height);

  void beginDraw();
  void endDraw();
};

#endif
