#ifndef CTCL_CANVAS_RENDERER_2D_H
#define CTCL_CANVAS_RENDERER_2D_H

#include <CTclCanvasPixelRenderer/CTclCanvasPixelRenderer.h>
#include <CRenderer2D/CRenderer2D.h>
#include <CMath/CTransform2D.h>

class CTclCanvas;

class CTclCanvasRenderer2D : public CRenderer2D {
 private:
  CTclCanvasPixelRenderer *renderer_;
  CTclCanvas              *canvas_;

 public:
  CTclCanvasRenderer2D(CTclCanvas *canvas);
 ~CTclCanvasRenderer2D();

  CTclCanvasRenderer2D *dup() const;

  CTclCanvasPixelRenderer *getPixelRenderer() const;

  void updateSize(int, int);
};

#endif
