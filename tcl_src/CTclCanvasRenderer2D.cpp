#include "std_c++.h"
#include "CTclCanvas/CTclCanvas.h"
#include "CTclCanvasPixelRenderer/CTclCanvasPixelRenderer.h"
#include "CTclCanvasRenderer2D/CTclCanvasRenderer2D.h"

CTclCanvasRenderer2D::
CTclCanvasRenderer2D(CTclCanvas *canvas) :
 CRenderer2D(renderer_ = new CTclCanvasPixelRenderer(canvas)), canvas_(canvas)
{
}

CTclCanvasRenderer2D::
~CTclCanvasRenderer2D()
{
  delete renderer_;
}

CTclCanvasRenderer2D *
CTclCanvasRenderer2D::
dup() const
{
  return new CTclCanvasRenderer2D(canvas_);
}

CTclCanvasPixelRenderer *
CTclCanvasRenderer2D::
getPixelRenderer() const
{
  return renderer_;
}

void
CTclCanvasRenderer2D::
updateSize(int width, int height)
{
  renderer_->updateSize(width, height);
}
