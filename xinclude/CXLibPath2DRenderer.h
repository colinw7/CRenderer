#ifndef CXLibPath2DRenderer_H
#define CXLibPath2DRenderer_H

#include <CPath2D.h>
#include <CXLibRenderer2D.h>

class CXLibPath2DRenderer : public CPath2DRenderer {
 public:
  CXLibPath2DRenderer(CXLibRenderer2D *renderer) :
   renderer_(renderer) {
  }

 ~CXLibPath2DRenderer() { }

  void setTransform(bool /*set*/) { }

  void adjustPoint(CPoint2D &p) { renderer_->adjustPoint(p); }

  void transformPoint(const CPoint2D &p1, CPoint2D &p2) {
    renderer_->transformPoint(p1, p2);
  }

  void untransformPoint(const CPoint2D &p1, CPoint2D &p2) {
    renderer_->untransformPoint(p1, p2);
  }

  void drawLine(const CPoint2D &p1, const CPoint2D &p2) {
    renderer_->drawLine(p1, p2);
  }

  void drawBezier2(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3) {
    renderer_->drawBezier2(p1, p2, p3);
  }

  void drawBezier3(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3,
                   const CPoint2D &p4) {
    renderer_->drawBezier3(p1, p2, p3, p4);
  }

  void drawText(const CPoint2D &p, const std::string &text) {
    renderer_->drawText(p, text);
  }

  void setTextSize(double w, double h) {
    renderer_->setTextSize(w, h);
  }

  void getTextLimits(const std::string &str, double *w, double *h) {
    renderer_->getTextLimits(str, w, h);
  }

  void capLine(const CPoint2D &p1, const CPoint2D &p2) {
    renderer_->capLine(p1, p2);
  }
  void joinLines(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3) {
    renderer_->joinLines(p1, p2, p3);
  }

  void initPolygons() {
    renderer_->initPolygons();
  }

  void addPolygon(const std::vector<CPoint2D> &points) {
    renderer_->addPolygon(points);
  }

  void fillPolygons() {
    renderer_->fillPolygons();
  }

  void eofillPolygons() {
    renderer_->eofillPolygons();
  }

  void imageFillPolygons(CImagePtr image) {
    renderer_->imageFillPolygons(image);
  }

  void imageEoFillPolygons(CImagePtr image) {
    renderer_->imageEoFillPolygons(image);
  }

  uint getNumClipPolygons() const {
    return renderer_->getNumClipPolygons();
  }

  const std::vector< std::vector<CPoint2D> > &getClipPolygons(uint ind) {
    return renderer_->getClipPolygons(ind);
  }

  void clipPolygons() {
    renderer_->clipPolygons();
  }

  void eoclipPolygons() {
    renderer_->eoclipPolygons();
  }

  const CMatrix2D *getIViewMatrix() {
    return renderer_->getIViewMatrix();
  }

  void getPixelSize(double *px, double *py) {
    renderer_->getPixelSize(px, py);
  }

 private:
  CXLibRenderer2D *renderer_;
};

#endif
