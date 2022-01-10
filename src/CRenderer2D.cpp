#include <CRenderer2D.h>
#include <CRendererRegion2D.h>
#include <CFreeTypeRenderer2D.h>
#include <CImageRenderer2D.h>
#include <CMathGen.h>
#include <CMathMacros.h>
#include <CLineList2D.h>
#include <CGenGradient.h>
#include <COrientation.h>
#include <CStrUtil.h>
#include <CFreeType.h>
#include <CMathRound.h>

const uint MAX_BEZIER_DEPTH = 9;

class GenGradient2DFill : public CRenderer2DFiller {
 public:
  GenGradient2DFill(CGenGradient *gradient, double x1, double y1, double x2, double y2) :
   gradient_(gradient), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
  }

  void fill(CRenderer2D *renderer) {
    renderer->fillFilledRectangle(CBBox2D(x1_, y1_, x2_, y2_), *this);
  }

  void getColor(const CPoint2D &point, CRGBA &rgba) const {
    double xt = (point.x - x1_)/(x2_ - x1_);
    double yt = (point.y - y1_)/(y2_ - y1_);

    rgba = gradient_->getColor(xt, yt);
  }

 private:
  CGenGradient *gradient_ { nullptr };
  double        x1_       { 0.0 };
  double        y1_       { 0.0 };
  double        x2_       { 0.0 };
  double        y2_       { 0.0 };
};

//--------------

CRenderer2D::
CRenderer2D()
{
  init();
}

CRenderer2D::
CRenderer2D(CPixelRenderer *renderer) :
 pixelRenderer_(renderer)
{
  init();
}

void
CRenderer2D::
init()
{
  displayRange_.setPixelRange (0, 0, 100, 100);
  displayRange_.setWindowRange(0, 0, 1, 1);

  pen_.init();

  transformFlag_ = true;
  antiAlias_     = false;

  //----

  unsetViewMatrix();

  //----

  enabled_ = true;
}

CRenderer2D::
CRenderer2D(const CRenderer2D &renderer)
{
  enabled_       = true;
  pixelRenderer_ = renderer.pixelRenderer_;
  displayRange_  = renderer.displayRange_;
  viewMatrix_    = renderer.viewMatrix_;
  viewIMatrix_   = renderer.viewIMatrix_;
  transformFlag_ = renderer.transformFlag_;
  antiAlias_     = renderer.antiAlias_;
  pen_           = renderer.pen_;
  brush_         = renderer.brush_;
  symbol_        = renderer.symbol_;
}

CRenderer2D::
~CRenderer2D()
{
}

CRenderer2D &
CRenderer2D::
operator=(const CRenderer2D &renderer)
{
  enabled_       = true;
  pixelRenderer_ = renderer.pixelRenderer_;
  displayRange_  = renderer.displayRange_;
  viewMatrix_    = renderer.viewMatrix_;
  viewIMatrix_   = renderer.viewIMatrix_;
  transformFlag_ = renderer.transformFlag_;
  antiAlias_     = renderer.antiAlias_;
  pen_           = renderer.pen_;
  brush_         = renderer.brush_;
  symbol_        = renderer.symbol_;

  path_   = nullptr;
  region_ = RendererRegion2DP();

  pathStack_.clear();

  ppath_ = PixelRendererPathP();

  return *this;
}

void
CRenderer2D::
setPixelRenderer(CPixelRenderer *renderer)
{
  pixelRenderer_ = renderer;

  resetDataRange();
}

CPixelRenderer *
CRenderer2D::
getPixelRenderer() const
{
  return pixelRenderer_;
}

bool
CRenderer2D::
getContentsChanged()
{
  return pixelRenderer_->getContentsChanged();
}

void
CRenderer2D::
setContentsChanged(bool flag)
{
  pixelRenderer_->setContentsChanged(flag);
}

uint
CRenderer2D::
getPixelWidth() const
{
  return pixelRenderer_->getWidth();
}

uint
CRenderer2D::
getPixelHeight() const
{
  return pixelRenderer_->getHeight();
}

void
CRenderer2D::
setPixelOffset(const CIPoint2D &offset)
{
  pixelRenderer_->setOffset(offset);
}

const CIPoint2D &
CRenderer2D::
getPixelOffset() const
{
  return pixelRenderer_->getOffset();
}

void
CRenderer2D::
applyPixelOffset(CIPoint2D &point) const
{
  pixelRenderer_->applyOffset(point);
}

void
CRenderer2D::
unapplyPixelOffset(CIPoint2D &point) const
{
  pixelRenderer_->unapplyOffset(point);
}

const CPixelClip &
CRenderer2D::
getClip() const
{
  return pixelRenderer_->getClip();
}

//------

// pixel/data ranges

void
CRenderer2D::
setPixelRange(int xmin, int ymin, int xmax, int ymax)
{
  displayRange_.setPixelRange(xmin, ymin, xmax, ymax);
}

void
CRenderer2D::
getPixelRange(int *xmin, int *ymin, int *xmax, int *ymax)
{
  displayRange_.getPixelRange(xmin, ymin, xmax, ymax);
}

void
CRenderer2D::
setDataRange(double xmin, double ymin, double xmax, double ymax)
{
  resetDataRange();

  displayRange_.setWindowRange(xmin, ymin, xmax, ymax);
}

void
CRenderer2D::
getDataRange(double *xmin, double *ymin, double *xmax, double *ymax)
{
  displayRange_.getWindowRange(xmin, ymin, xmax, ymax);
}

void
CRenderer2D::
resetDataRange()
{
  int pwidth  = pixelRenderer_->getWidth ();
  int pheight = pixelRenderer_->getHeight();

  setPixelRange(0, 0, pwidth - 1, pheight - 1);
}

//------

void
CRenderer2D::
updateSize(int width, int height)
{
  pixelRenderer_->updateSize(width, height);
}

//------

// pen

void
CRenderer2D::
setLineColor(const CRGBA &rgba)
{
  pen_.setColor(rgba);

  setForeground(rgba);
}

const CRGBA &
CRenderer2D::
getLineColor() const
{
  return pen_.getColor();
}

void
CRenderer2D::
setLineWidth(double width)
{
  pen_.setWidth(width);
}

double
CRenderer2D::
getLineWidth() const
{
  return pen_.getWidth();
}

void
CRenderer2D::
setLineDash(const CLineDash &dash)
{
  pen_.setLineDash(dash);
}

const CLineDash &
CRenderer2D::
getLineDash() const
{
  return pen_.getLineDash();
}

void
CRenderer2D::
setLineCap(CLineCapType cap)
{
  pixelRenderer_->setLineCap(cap);
}

CLineCapType
CRenderer2D::
getLineCap() const
{
  return pixelRenderer_->getLineCap();
}

void
CRenderer2D::
setLineJoin(CLineJoinType join)
{
  pixelRenderer_->setLineJoin(join);
}

CLineJoinType
CRenderer2D::
getLineJoin() const
{
  return pixelRenderer_->getLineJoin();
}

void
CRenderer2D::
setMitreLimit(double limit)
{
  pen_.setMitreLimit(limit);
}

double
CRenderer2D::
getMitreLimit() const
{
  return pen_.getMitreLimit();
}

void
CRenderer2D::
setFlatness(double flatness)
{
  pen_.setFlatness(flatness);
}

double
CRenderer2D::
getFlatness() const
{
  return pen_.getFlatness();
}

//------

// brush

void
CRenderer2D::
setFillType(CFillType fill_type)
{
  initPath();

  brush_.setFillRule(fill_type);

  if (getPath())
    getPath()->setFillType(fill_type);
}

CFillType
CRenderer2D::
getFillType() const
{
  return brush_.getFillRule();
}

void
CRenderer2D::
setBrushColor(const CRGBA &rgba)
{
  brush_.setColor(rgba);

  setForeground(rgba);
}

const CRGBA &
CRenderer2D::
getBrushColor() const
{
  return brush_.getColor();
}

void
CRenderer2D::
setBrushStyle(CBrushStyle style)
{
  brush_.setStyle(style);
}

CBrushStyle
CRenderer2D::
getBrushStyle() const
{
  return brush_.getStyle();
}

void
CRenderer2D::
setBrushTexture(CImagePtr texture)
{
  brush_.setTexture(texture);
}

CImagePtr
CRenderer2D::
getBrushTexture() const
{
  return brush_.getTexture();
}

void
CRenderer2D::
setBrushGradient(CRefPtr<CGenGradient> gradient)
{
  brush_.setGradient(gradient);
}

CRefPtr<CGenGradient>
CRenderer2D::
getBrushGradient() const
{
  return brush_.getGradient();
}

//-------

void
CRenderer2D::
beginDraw()
{
  pixelRenderer_->beginDraw();
}

void
CRenderer2D::
endDraw()
{
  pixelRenderer_->endDraw();
}

void
CRenderer2D::
startDoubleBuffer(bool clear)
{
  pixelRenderer_->startDoubleBuffer(clear);
}

void
CRenderer2D::
endDoubleBuffer(bool copy)
{
  pixelRenderer_->endDoubleBuffer(copy);
}

void
CRenderer2D::
copyDoubleBuffer()
{
  pixelRenderer_->copyDoubleBuffer();
}

void
CRenderer2D::
disableCanvas()
{
  enabled_ = false;
}

void
CRenderer2D::
enableCanvas()
{
  enabled_ = true;
}

void
CRenderer2D::
setViewMatrix(const CMatrix2D &viewMatrix)
{
  CMatrix2D imatrix;

  if (viewMatrix.invert(imatrix)) {
    viewMatrix_  = viewMatrix;
    viewIMatrix_ = imatrix;
  }
  else {
    std::cerr << "CRenderer2D::setViewMatrix - Invalid View Matrix" << std::endl;
  }
}

void
CRenderer2D::
unsetViewMatrix()
{
  viewMatrix_ .setIdentity();
  viewIMatrix_.setIdentity();
}

void
CRenderer2D::
setEqualScale(bool flag)
{
  displayRange_.setEqualScale(flag);
}

bool
CRenderer2D::
getEqualScale() const
{
  return displayRange_.getEqualScale();
}

void
CRenderer2D::
setScaleMin(bool flag)
{
  displayRange_.setScaleMin(flag);
}

bool
CRenderer2D::
getScaleMin() const
{
  return displayRange_.getScaleMin();
}

void
CRenderer2D::
setAlign(CDisplayRange2D::HAlign halign, CDisplayRange2D::VAlign valign)
{
  return displayRange_.setAlign(halign, valign);
}

CDisplayRange2D::HAlign
CRenderer2D::
getHAlign() const
{
  return displayRange_.getHAlign();
}

CDisplayRange2D::VAlign
CRenderer2D::
getVAlign() const
{
  return displayRange_.getVAlign();
}

void
CRenderer2D::
setBackground(const CRGBA &rgba)
{
  if (pixelRenderer_ != 0)
    pixelRenderer_->setBackground(rgba);
}

void
CRenderer2D::
setForeground(const CRGBA &rgba)
{
  if (pixelRenderer_ != 0)
    pixelRenderer_->setForeground(rgba);
}

//-------------------

CImagePtr
CRenderer2D::
getImage()
{
  return pixelRenderer_->getImage();
}

CImagePtr
CRenderer2D::
getAlphaImage()
{
  return pixelRenderer_->getAlphaImage();
}

void
CRenderer2D::
setImage(CImagePtr image)
{
  pixelRenderer_->setImage(image);
}

void
CRenderer2D::
setImage(CRenderer2D *renderer)
{
  pixelRenderer_->setImage(renderer->getImage());
}

void
CRenderer2D::
setAlphaImage(CImagePtr image)
{
  pixelRenderer_->setAlphaImage(image);
}

//-------------------

void
CRenderer2D::
initPath() const
{
  if (! getPath()) {
    auto *th = const_cast<CRenderer2D *>(this);

    th->path_ = PathP(createPath());
  }
}

void
CRenderer2D::
pathInit()
{
  initPath();

  if (! getPath())
    return;

  CFillType fill_type = getPath()->getFillType();

  pathStack_.push_back(path_.release());

  path_ = PathP(createPath());

  getPath()->setFillType(fill_type);
}

void
CRenderer2D::
pathTerm()
{
  assert(pathStack_.size() > 0);

  path_ = PathP(pathStack_.back());

  pathStack_.pop_back();
}

void
CRenderer2D::
pathRoundedRectangle(const CPoint2D &p1, const CPoint2D &p2, double xr, double yr)
{
  initPath();

  getPath()->roundedRectangle(p1, p2, xr, yr);
}

void
CRenderer2D::
pathMoveTo(const CPoint2D &p)
{
  initPath();

  getPath()->moveTo(p);
}

bool
CRenderer2D::
pathRMoveTo(const CPoint2D &p)
{
  initPath();

  return getPath()->rmoveTo(p);
}

bool
CRenderer2D::
pathLineTo(const CPoint2D &p)
{
  initPath();

  return getPath()->lineTo(p);
}

bool
CRenderer2D::
pathRLineTo(const CPoint2D &p)
{
  initPath();

  return getPath()->rlineTo(p);
}

bool
CRenderer2D::
pathArcTo(const CPoint2D &p1, const CPoint2D &p2, double xr, double yr)
{
  initPath();

  return getPath()->arcTo(p1, p2, xr, yr);
}

bool
CRenderer2D::
pathBezier2To(const CPoint2D &p1, const CPoint2D &p2)
{
  initPath();

  return getPath()->bezier2To(p1, p2);
}

bool
CRenderer2D::
pathRBezier2To(const CPoint2D &p1, const CPoint2D &p2)
{
  initPath();

  return getPath()->rbezier2To(p1, p2);
}

bool
CRenderer2D::
pathBezier3To(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  initPath();

  return getPath()->bezier3To(p1, p2, p3);
}

bool
CRenderer2D::
pathRBezier3To(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  initPath();

  return getPath()->rbezier3To(p1, p2, p3);
}

void
CRenderer2D::
pathClose()
{
  initPath();

  getPath()->close();
}

bool
CRenderer2D::
pathGetCurrentPoint(CPoint2D &p)
{
  initPath();

  return getPath()->getCurrentPoint(p);
}

void
CRenderer2D::
pathStroke()
{
  initPath();

  getPath()->stroke();
}

void
CRenderer2D::
pathStrokePath()
{
  initPath();

  getPath()->strokePath();
}

void
CRenderer2D::
pathFill()
{
  initPath();

  getPath()->fill();
}

void
CRenderer2D::
pathEofill()
{
  initPath();

  // TODO
  getPath()->fill();
}

void
CRenderer2D::
pathPatternFill(CBrushPattern pattern)
{
  initPath();

  getPath()->fillImage(CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
pathImageFill(CImagePtr image)
{
  initPath();

  getPath()->fillImage(image);
}

void
CRenderer2D::
pathGradientFill(CRefPtr<CGenGradient> gradient)
{
  initPath();

  getPath()->fillGradient(gradient);
}

void
CRenderer2D::
pathClip()
{
  initPath();

  getPath()->clip();
}

void
CRenderer2D::
pathEoclip()
{
  initPath();

  getPath()->eoclip();
}

void
CRenderer2D::
pathClipPath()
{
  initPath();

  getPath()->clipPath();
}

void
CRenderer2D::
pathFlatten()
{
  initPath();

  getPath()->flatten();
}

void
CRenderer2D::
pathReverse()
{
  initPath();

  getPath()->reverse();
}

void
CRenderer2D::
pathProcess(CPath2DVisitor &visitor)
{
  initPath();

  getPath()->process(visitor);
}

void
CRenderer2D::
pathShowChar(int c, CMatrix2D *matrix, double *x, double *y)
{
  initPath();

  getPath()->showChar(c, matrix, x, y);
}

void
CRenderer2D::
pathGetBounds(double *xmin, double *ymin, double *xmax, double *ymax)
{
  initPath();

  getPath()->getBounds(xmin, ymin, xmax, ymax);
}

void
CRenderer2D::
pathSetStrokeAdjust(bool stroke_adjust)
{
  initPath();

  getPath()->setStrokeAdjust(stroke_adjust);
}

void
CRenderer2D::
pathBBox(CBBox2D &bbox)
{
  initPath();

  getPath()->bbox(bbox);
}

void
CRenderer2D::
pathPrint()
{
  initPath();

  getPath()->print();
}

//-------------------

void
CRenderer2D::
clipPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (ppath_)
    pixelRenderer_->clipPolygons(ppath_->getIPolygons());
}

void
CRenderer2D::
eoclipPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (ppath_)
    pixelRenderer_->eoclipPolygons(ppath_->getIPolygons());
}

uint
CRenderer2D::
getNumClipPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return 0;

  return pixelRenderer_->getNumClipPolygons();
}

const CRenderer2D::PointListList &
CRenderer2D::
getClipPolygons(uint i)
{
  static PointListList poly_points_list;

  if (! enabled_ || ! pixelRenderer_)
    return poly_points_list;

  const CPixelRenderer::IPointListList &ipolygon_points = pixelRenderer_->getClipPolygons(i);

  uint num_ipolygon_points = ipolygon_points.size();

  poly_points_list.resize(num_ipolygon_points);

  for (uint ip = 0; ip < num_ipolygon_points; ++ip) {
    PointList &points = poly_points_list[ip];

    uint num_points = (*ipolygon_points[ip]).size();

    points.resize(num_points);

    for (uint j = 0; j < num_points; ++j) {
      const CIPoint2D &p1 = (*ipolygon_points[ip])[j];

      untransformPoint(CPoint2D(p1.x, p1.y), points[j]);
    }
  }

  return poly_points_list;
}

void
CRenderer2D::
initPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (ppath_)
    ppath_->initPolygons();
}

void
CRenderer2D::
addPolygon(const RPointList &points)
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (! ppath_)
    ppath_ = PixelRendererPathP(new CPixelRendererPath);

  uint num_points = points.size();

  if (antiAlias_) {
    std::vector<CPoint2D> rpoints;

    rpoints.resize(num_points);

    for (uint i = 0; i < num_points; ++i) {
      CPoint2D p;

      transformPointToPixel(points[i], p);

      rpoints[i] = p;
    }

    ppath_->addPathPolygon(rpoints);
  }
  else {
    std::vector<CIPoint2D> ipoints;

    ipoints.resize(num_points);

    for (uint i = 0; i < num_points; ++i) {
      CIPoint2D p;

      transformPointToPixel(points[i], p);

      ipoints[i] = p;
    }

    ppath_->addPathPolygon(ipoints);
  }
}

void
CRenderer2D::
fillPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  CImagePtr image;

  fillPolygons(image);
}

void
CRenderer2D::
fillPolygons(CImagePtr image)
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (ppath_) {
    pixelRenderer_->fillPathPolygons(ppath_->getIPolygons(), image, FILL_TYPE_WINDING);
    pixelRenderer_->fillPathPolygons(ppath_->getRPolygons(), image, FILL_TYPE_WINDING);
  }
}

void
CRenderer2D::
eofillPolygons()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  CImagePtr image;

  eofillPolygons(image);
}

void
CRenderer2D::
eofillPolygons(CImagePtr image)
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  if (ppath_) {
    pixelRenderer_->fillPathPolygons(ppath_->getIPolygons(), image, FILL_TYPE_EVEN_ODD);
    pixelRenderer_->fillPathPolygons(ppath_->getRPolygons(), image, FILL_TYPE_EVEN_ODD);
  }
}

//-------------------

void
CRenderer2D::
arcToBeziers(const CPoint2D &center, double xr, double yr,
             double angle1, double angle2, BezierList &arc_beziers)
{
  uint num_beziers = calcNumArcBeziers(center, xr, yr, angle1, angle2);

  arc_beziers.resize(num_beziers);

  //------

  double angle_diff = angle2 - angle1;

  double da = angle_diff/num_beziers;

  double rx1 = 4.0*xr*tan(da*0.25)/3.0;
  double ry1 = 4.0*yr*tan(da*0.25)/3.0;

  double s1 = sin(angle1);
  double c1 = cos(angle1);

  double x1 = center.x + xr*c1;
  double y1 = center.y + yr*s1;

  for (uint i = 1; i <= num_beziers; ++i) {
    double angle = angle1 + i*da;

    double s2 = sin(angle);
    double c2 = cos(angle);

    //------

    double x4 = center.x + xr *c2; double y4 = center.y + yr *s2;
    double x2 = x1       - rx1*s1; double y2 = y1       + ry1*c1;
    double x3 = x4       + rx1*s2; double y3 = y4       - ry1*c2;

    arc_beziers[i - 1].setPoints(x1, y1, x2, y2, x3, y3, x4, y4);

    //------

    x1 = x4; y1 = y4;
    s1 = s2; c1 = c2;
  }
}

void
CRenderer2D::
arcNToBeziers(const CPoint2D &center, double xr, double yr,
              double angle1, double angle2, BezierList &arc_beziers)
{
  uint num_beziers = calcNumArcBeziers(center, xr, yr, angle1, angle2);

  arc_beziers.resize(num_beziers);

  //------

  double angle_diff = angle2 - angle1;

  double da = angle_diff/num_beziers;

  double rx1 = 4.0*xr*tan(da*0.25)/3.0;
  double ry1 = 4.0*yr*tan(da*0.25)/3.0;

  double s1 = sin(angle1);
  double c1 = cos(angle1);

  double x1 = center.x + xr*c1;
  double y1 = center.y + yr*s1;

  for (uint i = 1; i <= num_beziers; ++i) {
    double angle = angle1 - i*da;

    double s2 = sin(angle);
    double c2 = cos(angle);

    //------

    double x4 = center.x + xr *c2; double y4 = center.y + yr *s2;
    double x2 = x1       + rx1*s1; double y2 = y1       - ry1*c1;
    double x3 = x4       - rx1*s2; double y3 = y4       + ry1*c2;

    arc_beziers[i - 1].setPoints(x1, y1, x2, y2, x3, y3, x4, y4);

    //------

    x1 = x4; y1 = y4;
    s1 = s2; c1 = c2;
  }
}

uint
CRenderer2D::
calcNumArcBeziers(const CPoint2D &, double xr, double yr, double angle1, double angle2)
{
  double angle_diff = fabs(angle2 - angle1);

  if (angle_diff > 2.0*M_PI)
    angle_diff = 2.0*M_PI;

  //------

  uint num_beziers = CMathRound::Round(angle_diff/M_PI);

  if (num_beziers <= 0)
    num_beziers = 1;

  //------

  double xsize, ysize;

  windowLengthToPixelLength(xr, &xsize);
  windowLengthToPixelLength(yr, &ysize);

  int size = CMathRound::Round(std::max(xsize, ysize));

  if      (size <= 27)
    ;
  else if (size <= 1834)
    num_beziers *= 2;
  else if (size <= 20953)
    num_beziers *= 3;
  else if (size <= 117778)
    num_beziers *= 4;
  else
    num_beziers *= 8;

  if (num_beziers > 16)
    num_beziers = 16;

  //------

  return num_beziers;
}

void
CRenderer2D::
bezierToLines(const C3Bezier2D &bezier, PointList &points, RealList &gradients)
{
  double x, y;

  bezier.getFirstPoint(&x, &y);

  points   .push_back(CPoint2D(x, y));
  gradients.push_back(bezier.gradientStart());

  bezierToLines1(bezier, 1, points, gradients);
}

void
CRenderer2D::
bezierToLines1(const C3Bezier2D &bezier, uint depth, PointList &points, RealList &gradients)
{
  if (depth >= MAX_BEZIER_DEPTH) {
    double x, y;

    bezier.getLastPoint(&x, &y);

    points   .push_back(CPoint2D(x, y));
    gradients.push_back(bezier.gradientEnd());

    return;
  }

  //-----

  double x1, y1, x4, y4;

  bezier.getFirstPoint(&x1, &y1);
  bezier.getLastPoint (&x4, &y4);

  double a =  y4   - y1;
  double b =  x1   - x4;
  double c = -x1*a - y1*b;

  double s = a*a + b*b;

  if (s == 0.0)
    return;

  double x2, y2, x3, y3;

  bezier.getControlPoint1(&x2, &y2);
  bezier.getControlPoint2(&x3, &y3);

  double f2 = a*x2 + b*y2 + c;
  double f3 = a*x3 + b*y3 + c;

  double s2 = f2*f2/s;
  double s3 = f3*f3/s;

  double ps2 = 0.0, ps3 = 0.0;

  windowLengthToPixelLength(s2, &ps2);
  windowLengthToPixelLength(s3, &ps3);

  if (ps2 > 0.1 || ps3 > 0.1) {
    double xm1 = (x2  + x3 )/2.0;
    double ym1 = (y2  + y3 )/2.0;

    double x12 = (x1  + x2 )/2.0;
    double y12 = (y1  + y2 )/2.0;
    double x13 = (x12 + xm1)/2.0;
    double y13 = (y12 + ym1)/2.0;

    double x23 = (x3  + x4 )/2.0;
    double y23 = (y3  + y4 )/2.0;
    double x22 = (xm1 + x23)/2.0;
    double y22 = (ym1 + y23)/2.0;

    double xm2 = (x13 + x22)/2.0;
    double ym2 = (y13 + y22)/2.0;

    C3Bezier2D bezier1(x1 , y1 , x12, y12, x13, y13, xm2, ym2);

    bezierToLines1(bezier1, depth + 1, points, gradients);

    C3Bezier2D bezier2(xm2, ym2, x22, y22, x23, y23, x4 , y4 );

    bezierToLines1(bezier2, depth + 1, points, gradients);
  }
  else {
    double x, y;

    bezier.getLastPoint(&x, &y);

    points   .push_back(CPoint2D(x, y));
    gradients.push_back(bezier.gradientEnd());
  }
}

void
CRenderer2D::
bezierToLines(const C2Bezier2D &bezier, PointList &points, RealList &gradients)
{
  double x, y;

  bezier.getFirstPoint(&x, &y);

  points   .push_back(CPoint2D(x, y));
  gradients.push_back(bezier.gradientStart());

  bezierToLines1(bezier, 1, points, gradients);
}

void
CRenderer2D::
bezierToLines1(const C2Bezier2D &bezier, uint depth, PointList &points, RealList &gradients)
{
  if (depth >= MAX_BEZIER_DEPTH) {
    double x, y;

    bezier.getLastPoint(&x, &y);

    points   .push_back(CPoint2D(x, y));
    gradients.push_back(bezier.gradientEnd());

    return;
  }

  //-----

  double x1, y1, x3, y3;

  bezier.getFirstPoint(&x1, &y1);
  bezier.getLastPoint (&x3, &y3);

  double a =  y3   - y1;
  double b =  x1   - x3;
  double c = -x1*a - y1*b;

  double s = a*a + b*b;

  if (s == 0.0)
    return;

  double x2, y2;

  bezier.getControlPoint(&x2, &y2);

  double f2 = a*x2 + b*y2 + c;

  double s2 = f2*f2/s;

  double ps2;

  windowLengthToPixelLength(s2, &ps2);

  if (ps2 > 0.1) {
    double x12 = (x1 + x2 )/2.0;
    double y12 = (y1 + y2 )/2.0;

    double x23 = (x2 + x3 )/2.0;
    double y23 = (y2 + y3 )/2.0;

    double xm = (x12 + x23)/2.0;
    double ym = (y12 + y23)/2.0;

    C2Bezier2D bezier1(x1, y1, x12, y12, xm, ym);

    bezierToLines1(bezier1, depth + 1, points, gradients);

    C2Bezier2D bezier2(xm, ym, x23, y23, x3, y3);

    bezierToLines1(bezier2, depth + 1, points, gradients);
  }
  else {
    double x, y;

    bezier.getLastPoint(&x, &y);

    points   .push_back(CPoint2D(x, y));
    gradients.push_back(bezier.gradientEnd());
  }
}

//-------------------

#if 0
void
CRenderer2D::
drawLine(const CLine2D &line)
{
  drawLine(line.start(), line.end());
}
#endif

void
CRenderer2D::
drawLine(const CPoint2D &point1, const CPoint2D &point2)
{
  if (! enabled_)
    return;

  if (! getLineDash().isSolid())
    drawDashLine (point1, point2);
  else
    drawSolidLine(point1, point2);
}

void
CRenderer2D::
drawLineG2(const CPoint2D &point1, const CPoint2D &point2, double g1, double g2)
{
  if (! getLineDash().isSolid())
    drawDashLineG2(point1, point2, g1, g2);
  else
    drawSolidLineG2(point1, point2, g1, g2);
}

void
CRenderer2D::
drawDashLine(const CPoint2D &point1, const CPoint2D &point2)
{
  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  double length = sqrt(x21*x21 + y21*y21);

  double pos = 0.0;

  double xx1 = point1.x;
  double yy1 = point1.y;

  while (pos < length) {
    bool draw = getLineDash().isDraw();

    double delta = getLineDash().getDelta();

    if (pos + delta > length)
      delta = length - pos;

    getLineDash().step();

    double xx2 = xx1 + delta*x21/length;
    double yy2 = yy1 + delta*y21/length;

    if (draw) {
      if (pos > 0.0)
        capLine(CPoint2D(xx1, yy1), CPoint2D(xx2, yy2));

      drawSolidLine(CPoint2D(xx1, yy1), CPoint2D(xx2, yy2));

      if (pos + delta < length)
        capLine(CPoint2D(xx2, yy2), CPoint2D(xx1, yy1));
    }

    xx1 = xx2;
    yy1 = yy2;

    pos += delta;
  }
}

void
CRenderer2D::
drawDashLineG2(const CPoint2D &point1, const CPoint2D &point2, double g1, double g2)
{
  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  double length = sqrt(x21*x21 + y21*y21);

  double pos = 0.0;

  double xx1 = point1.x;
  double yy1 = point1.y;

  while (pos < length) {
    bool draw = getLineDash().isDraw();

    double delta = getLineDash().getDelta();

    if (pos + delta > length)
      delta = length - pos;

    getLineDash().step();

    double xx2 = xx1 + delta*x21/length;
    double yy2 = yy1 + delta*y21/length;

    if (draw) {
      if (pos > 0.0)
        capLine(CPoint2D(xx1, yy1), CPoint2D(xx2, yy2));

      drawSolidLineG2(CPoint2D(xx1, yy1), CPoint2D(xx2, yy2), g1, g2);

      if (pos + delta < length)
        capLine(CPoint2D(xx2, yy2), CPoint2D(xx1, yy1));
    }

    xx1 = xx2;
    yy1 = yy2;

    pos += delta;
  }
}

void
CRenderer2D::
drawSolidLine(const CPoint2D &point1, const CPoint2D &point2)
{
  if (getLineWidth() > 0.0)
    drawWideSolidLine(point1, point2);
  else
    drawThinSolidLine(point1, point2);
}

void
CRenderer2D::
drawSolidLineG2(const CPoint2D &point1, const CPoint2D &point2, double g1, double g2)
{
  if (getLineWidth() > 0.0)
    drawWideSolidLineG2(point1, point2, g1, g2);
  else
    drawThinSolidLine(point1, point2);
}

void
CRenderer2D::
drawWideSolidLine(const CPoint2D &point1, const CPoint2D &point2)
{
  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  if (x21 == 0.0 && y21 == 0.0)
    return;

  double g = CMathGen::atan2(x21, y21);

  CPoint2D p[4];

  addWidthToPointG1(point1, g, p[0], p[3]);
  addWidthToPointG1(point2, g, p[1], p[2]);

  if (antiAlias_) {
    CPoint2D rpoints[4];

    for (uint i = 0; i < 4; ++i)
      transformPointToPixel(p[i], rpoints[i]);

    pixelRenderer_->fillAAPolygon(rpoints, 4);
  }
  else {
    IPointList ipoints;

    ipoints.resize(4);

    for (uint i = 0; i < 4; ++i)
      transformPointToPixel(p[i], ipoints[i]);

    pixelRenderer_->fillPolygon(ipoints);
  }
}

void
CRenderer2D::
drawWideSolidLineG2(const CPoint2D &point1, const CPoint2D &point2, double g1, double g2)
{
  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  if (x21 == 0.0 && y21 == 0.0)
    return;

  double g = CMathGen::atan2(x21, y21);

  PointList points;

  points.resize(4);

  addWidthToPointG2(point1, g, g1, points[0], points[3]);
  addWidthToPointG2(point2, g, g2, points[1], points[2]);

  fillPolygon(points);
}

void
CRenderer2D::
drawThinSolidLine(const CPoint2D &point1, const CPoint2D &point2)
{
  CIPoint2D ipoint1, ipoint2;

  transformPointToPixel(point1, ipoint1);
  transformPointToPixel(point2, ipoint2);

  pixelRenderer_->drawLine(ipoint1, ipoint2);
}

//-----------

void
CRenderer2D::
fillPolygon(const PointList &points)
{
  if (brush_.getStyle() == CBRUSH_STYLE_NONE)
    return;

  static IPointList ipoints;
  static PointList  rpoints;

  if (! enabled_)
    return;

  uint num_points = points.size();

  if (antiAlias_) {
    rpoints.resize(num_points);

    for (uint i = 0; i < num_points; ++i)
      transformPointToPixel(points[i], rpoints[i]);

    pixelRenderer_->fillAAPolygon(rpoints);
  }
  else {
    ipoints.resize(num_points);

    for (uint i = 0; i < num_points; ++i)
      transformPointToPixel(points[i], ipoints[i]);

    fillBrushPolygon(ipoints, brush_);
  }
}

void
CRenderer2D::
fillFilledPolygon(const PointList &points, const CRenderer2DFiller &filler)
{
  static IPointList ipoints;
  static PointList  rpoints;

  class Filler : public CPixelRendererFiller {
   private:
    CRenderer2D             *renderer_;
    const CRenderer2DFiller &filler_;

   public:
    Filler(CRenderer2D *renderer, const CRenderer2DFiller &filler) :
     renderer_(renderer), filler_(filler) {
    }

    void getColor(const CIPoint2D &p, CRGBA &rgba) const {
      CPoint2D p1;

      renderer_->transformPixelToPoint(p, p1);

      filler_.getColor(p1, rgba);
    }
  };

  uint num_xy = points.size();

  if (antiAlias_) {
    rpoints.resize(num_xy);

    for (uint i = 0; i < num_xy; ++i)
      transformPointToPixel(points[i], rpoints[i]);

    Filler pfiller(this, filler);

    pixelRenderer_->fillFilledAAPolygon(rpoints, pfiller);
  }
  else {
    ipoints.resize(num_xy);

    for (uint i = 0; i < num_xy; ++i)
      transformPointToPixel(points[i], ipoints[i]);

    Filler pfiller(this, filler);

    pixelRenderer_->fillFilledPolygon(ipoints, pfiller);
  }
}

void
CRenderer2D::
fillBrushPolygon(const IPointList &ipoints, const CBrush &brush)
{
  switch (brush.getStyle()) {
    case CBRUSH_STYLE_SOLID:
      pixelRenderer_->fillPolygon(ipoints);
      break;
    case CBRUSH_STYLE_PATTERN:
      fillPatternPolygon(ipoints, brush.getPattern());
      break;
    case CBRUSH_STYLE_GRADIENT:
      fillGradientPolygon(ipoints, brush_.getGradient());
      break;
    case CBRUSH_STYLE_TEXTURE:
      fillImagePolygon(ipoints, brush_.getTexture());
      break;
    default:
      break;
  }
}

void
CRenderer2D::
fillPatternPolygon(const PointList &points, CBrushPattern pattern)
{
  fillImagePolygon(points, CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
fillPatternPolygon(const IPointList &ipoints, CBrushPattern pattern)
{
  fillImagePolygon(ipoints, CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
fillGradientPolygon(const PointList &points, CRefPtr<CGenGradient> gradient)
{
  uint num_points = points.size();

  IPointList ipoints;

  ipoints.resize(num_points);

  for (uint i = 0; i < num_points; ++i)
    transformPointToPixel(points[i], ipoints[i]);

  fillGradientPolygon(ipoints, gradient);
}

void
CRenderer2D::
fillGradientPolygon(const IPointList &ipoints, CRefPtr<CGenGradient> gradient)
{
  pixelRenderer_->fillGradientPolygon(ipoints, gradient);
}

void
CRenderer2D::
fillImagePolygon(const PointList &points, CImagePtr image)
{
  uint num_points = points.size();

  IPointList ipoints;

  ipoints.resize(num_points);

  for (uint i = 0; i < num_points; ++i)
    transformPointToPixel(points[i], ipoints[i]);

  fillImagePolygon(ipoints, image);
}

void
CRenderer2D::
fillImagePolygon(const IPointList &ipoints, CImagePtr image)
{
  pixelRenderer_->fillImagePolygon(ipoints, image);
}

void
CRenderer2D::
drawPolygon(const PointList &points)
{
  static IPointList ipoints;

  if (! enabled_)
    return;

  uint num_points = points.size();

  if (num_points < 3)
    return;

  pathInit();

  pathMoveTo(points[0]);

  for (uint i = 1; i < num_points; ++i)
    pathLineTo(points[i]);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
drawCircle(const CPoint2D &center, double r)
{
  drawArc(center, r, r, 0, 2*M_PI);
}

void
CRenderer2D::
drawEllipse(const CPoint2D &center, double xr, double yr)
{
  drawArc(center, xr, yr, 0, 2*M_PI);
}

void
CRenderer2D::
drawArc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  std::vector<C3Bezier2D> beziers;

  arcToBeziers(center, xr, yr, angle1, angle2, beziers);

  uint num_beziers = beziers.size();

  if (num_beziers == 0)
    return;

  //------

  pathInit();

  if (getPath()) {
    CPoint2D p1 = beziers[0].getFirstPoint();

    pathMoveTo(p1);

    for (uint i = 0; i < num_beziers; ++i) {
      CPoint2D p2 = beziers[i].getControlPoint1();
      CPoint2D p3 = beziers[i].getControlPoint2();
      CPoint2D p4 = beziers[i].getLastPoint    ();

      pathBezier3To(p2, p3, p4);
    }

    pathClose();

    pathStroke();
  }
  else {
    for (uint i = 0; i < num_beziers; ++i)
      drawBezier(beziers[i]);
  }
}

void
CRenderer2D::
drawArcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  std::vector<C3Bezier2D> beziers;

  arcNToBeziers(center, xr, yr, angle1, angle2, beziers);

  uint num_beziers = beziers.size();

  if (num_beziers == 0)
    return;

  //------

  pathInit();

  CPoint2D p1 = beziers[0].getFirstPoint();

  pathMoveTo(p1);

  for (uint i = 0; i < num_beziers; ++i) {
    CPoint2D p2 = beziers[i].getControlPoint1();
    CPoint2D p3 = beziers[i].getControlPoint2();
    CPoint2D p4 = beziers[i].getLastPoint    ();

    pathBezier3To(p2, p3, p4);
  }

  pathClose();

  pathStroke();

#if 0
  for (uint i = 0; i < num_beziers; ++i)
    drawBezier(beziers[i]);
#endif
}

void
CRenderer2D::
fillCircle(const CPoint2D &center, double r)
{
  fillArc(center, r, r, 0, 2*M_PI);
}

void
CRenderer2D::
fillEllipse(const CPoint2D &center, double xr, double yr)
{
  fillArc(center, xr, yr, 0, 2*M_PI);
}

void
CRenderer2D::
fillImageCircle(const CPoint2D &center, double r, CImagePtr image)
{
  fillImageEllipse(center, r, r, image);
}

void
CRenderer2D::
fillImageEllipse(const CPoint2D &center, double xr, double yr, CImagePtr image)
{
  fillImageArc(center, xr, yr, 0, 360, image);
}

void
CRenderer2D::
fillPatternCircle(const CPoint2D &center, double r, CBrushPattern pattern)
{
  fillImageCircle(center, r, CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
fillPatternEllipse(const CPoint2D &center, double xr, double yr, CBrushPattern pattern)
{
  fillImageEllipse(center, xr, yr, CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
fillGradientCircle(const CPoint2D &center, double r, CRefPtr<CGenGradient> gradient)
{
  fillGradientEllipse(center, r, r, gradient);
}

void
CRenderer2D::
fillGradientEllipse(const CPoint2D &center, double xr, double yr, CRefPtr<CGenGradient> gradient)
{
  fillGradientArc(center, xr, yr, 0, 360, gradient);
}

void
CRenderer2D::
fillArc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  PointList fill_points;

  fill_points.push_back(center);

  //------

  std::vector<C3Bezier2D> beziers;

  arcToBeziers(center, xr, yr, angle1, angle2, beziers);

  uint num_beziers = beziers.size();

  //------

  for (uint i = 0; i < num_beziers; ++i) {
    std::vector<CPoint2D> points;
    std::vector<double>   gradients;

    bezierToLines(beziers[i], points, gradients);

    uint num_points = points.size();

    //----

    if (i == 0)
      fill_points.push_back(points[0]);

    for (uint j = 1; j < num_points; ++j)
      fill_points.push_back(points[j]);
  }

  fill_points.push_back(center);

  //------

  fillPolygon(fill_points);
}

void
CRenderer2D::
fillArcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  PointList fill_points;

  fill_points.push_back(center);

  //------

  std::vector<C3Bezier2D> beziers;

  arcNToBeziers(center, xr, yr, angle1, angle2, beziers);

  uint num_beziers = beziers.size();

  //------

  for (uint i = 0; i < num_beziers; ++i) {
    std::vector<CPoint2D> points;
    std::vector<double>   gradients;

    bezierToLines(beziers[i], points, gradients);

    uint num_points = points.size();

    //----

    if (i == 0)
      fill_points.push_back(points[0]);

    for (uint j = 1; j < num_points; ++j)
      fill_points.push_back(points[j]);
  }

  fill_points.push_back(center);

  //------

  fillPolygon(fill_points);
}

void
CRenderer2D::
fillImageArc(const CPoint2D &center, double xr, double yr,
             double angle1, double angle2, CImagePtr image)
{
  PointList fill_points;

  arcToPoints(center, xr, yr, angle1, angle2, fill_points);

  fillImagePolygon(fill_points, image);
}

void
CRenderer2D::
fillPatternArc(const CPoint2D &center, double xr, double yr,
               double angle1, double angle2, CBrushPattern pattern)
{
  fillImageArc(center, xr, yr, angle1, angle2, CPixelRenderer::getPatternImage(pattern));
}

void
CRenderer2D::
fillGradientArc(const CPoint2D &center, double xr, double yr,
                double angle1, double angle2, CRefPtr<CGenGradient> gradient)
{
  PointList fill_points;

  arcToPoints(center, xr, yr, angle1, angle2, fill_points);

  fillGradientPolygon(fill_points, gradient);
}

void
CRenderer2D::
arcToPoints(const CPoint2D &center, double xr, double yr,
            double angle1, double angle2, PointList &fill_points)
{
  fill_points.push_back(center);

  //------

  std::vector<C3Bezier2D> beziers;

  arcToBeziers(center, xr, yr, angle1, angle2, beziers);

  uint num_beziers = beziers.size();

  //------

  for (uint i = 0; i < num_beziers; ++i) {
    std::vector<CPoint2D> points;
    std::vector<double>   gradients;

    bezierToLines(beziers[i], points, gradients);

    uint num_points = points.size();

    //----

    if (i == 0)
      fill_points.push_back(points[0]);

    for (uint j = 1; j < num_points; ++j)
      fill_points.push_back(points[j]);
  }

  fill_points.push_back(center);
}

//------

void
CRenderer2D::
drawBezier2(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  C2Bezier2D bezier(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);

  drawBezier(bezier);
}

void
CRenderer2D::
drawBezier3(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3, const CPoint2D &p4)
{
  C3Bezier2D bezier(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y);

  drawBezier(bezier);
}

void
CRenderer2D::
drawBezier(C2Bezier2D &bezier)
{
  if (! enabled_)
    return;

  std::vector<CPoint2D> points;
  std::vector<double>   gradients;

  bezierToLines(bezier, points, gradients);

  uint num_points = points.size();

  if (num_points <= 1)
    return;

  for (uint i = 0; i < num_points - 1; ++i)
    drawLineG2(points[i], points[i + 1], gradients[i], gradients[i + 1]);
}

void
CRenderer2D::
drawBezier(C3Bezier2D &bezier)
{
  if (! enabled_)
    return;

  std::vector<CPoint2D> points;
  std::vector<double>   gradients;

  bezierToLines(bezier, points, gradients);

  uint num_points = points.size();

  if (num_points <= 1)
    return;

  for (uint i = 0; i < num_points - 1; ++i)
    drawLineG2(points[i], points[i + 1], gradients[i], gradients[i + 1]);
}

//-----------

void
CRenderer2D::
lineToPolygon(const CPoint2D &p1, const CPoint2D &p2, PointList &points)
{
  double x21 = p2.x - p1.x;
  double y21 = p2.y - p1.y;

  if (x21 == 0.0 && y21 == 0.0) {
    points.resize(0);
    return;
  }

  double save_pen_width = getLineWidth();

  if (getLineWidth() <= 0.0)
    setLineWidth(1.0);

  double g = CMathGen::atan2(x21, y21);

  points.resize(4);

  addWidthToPointG1(p1, g, points[0], points[3]);
  addWidthToPointG1(p2, g, points[1], points[2]);

  setLineWidth(save_pen_width);
}

//-----------

bool
CRenderer2D::
image(char *image_data, int width, int height, int bits_per_sample, CMatrix2D *matrix,
      CMatrix2D *ctmMatrix, double *decode_array, int decode_size)
{
  if (! enabled_)
    return true;

#ifdef DEBUG
  debugPrintImage(image_data, width, height, bits_per_sample,
                  matrix, ctmMatrix, decode_array, decode_size);
#endif

  setTransform(false);

  CMatrix2D imatrix;

  bool flag = matrix->invert(imatrix);

  if (! flag) {
    setTransform(true);

    return false;
  }

  int num_colors = 1 << bits_per_sample;

  int bit_num  = 7;
  int byte_num = 0;

  CRGBA rgba;

  double y = 0.0;

  for (int i = 0; i < height; ++i) {
    double x = 0.0;

    int bit_mask = 1 << bit_num;

    for (int j = 0; j < width; ++j) {
      int color_bit = bits_per_sample - 1;

      int color = 0;

      for (int k = 0; k < bits_per_sample; ++k) {
        if (image_data[byte_num] & bit_mask)
          color |= 1 << color_bit;

        bit_num--;

        if (bit_num < 0) {
          bit_num = 7;

          ++byte_num;
        }

        bit_mask = 1 << bit_num;
      }

      if (color < decode_size)
        rgba.setGray(decode_array[color]);
      else
        rgba.setGray(((double) color)/(num_colors - 1));

      setForeground(rgba);

      double x11, y11, x21, y21;

      imatrix.multiplyPoint(x    , y    , &x11, &y11);
      imatrix.multiplyPoint(x + 1, y + 1, &x21, &y21);

      double x12, y12, x22, y22;

      ctmMatrix->multiplyPoint(x11, y11, &x12, &y12);
      ctmMatrix->multiplyPoint(x21, y21, &x22, &y22);

      PointList points;

      points[0].x = x12; points[0].y = y12;
      points[1].x = x22; points[1].y = y12;
      points[2].x = x22; points[2].y = y22;
      points[3].x = x12; points[3].y = y22;

      fillPolygon(points);

      x += 1.0;
    }

    if (bit_num != 7) {
      bit_num = 7;

      ++byte_num;
    }

    y += 1.0;
  }

  setTransform(true);

  return true;
}

void
CRenderer2D::
debugPrintImage(char *image_data, int width, int height, int bits_per_sample,
                CMatrix2D *, CMatrix2D *, double *, int)
{
  CStrUtil::printf("Image\n");
  CStrUtil::printf("  Width  = %d\n", width);
  CStrUtil::printf("  Height = %d\n", height);
  CStrUtil::printf("  Bits   = %d\n", bits_per_sample);
  CStrUtil::printf("  Data :-\n");

  int width1 = width;

  if ((width % 8) != 0)
    width1 += 8 - (width % 8);

  int num_bits  = width1*height*bits_per_sample;
  int num_bytes = num_bits/8;

  int i = 0;

  for ( ; i < num_bytes; ++i) {
    if (i % 8 == 0)
      CStrUtil::printf("    ");
    else
      CStrUtil::printf(", ");

    CStrUtil::printf("%x", image_data[i] & 0xFF);

    if (i % 8 == 7)
      CStrUtil::printf("\n");
  }

  if (i % 8 != 0)
    CStrUtil::printf("\n");
}

bool
CRenderer2D::
imageMask(char *image_data, int width, int height, int polarity,
          CMatrix2D *matrix, CMatrix2D *ctmMatrix)
{
  if (! enabled_)
    return true;

#ifdef DEBUG
  debugPrintImageMask(image_data, width, height, polarity, matrix, ctmMatrix);
#endif

  setTransform(false);

  CMatrix2D imatrix;

  bool flag = matrix->invert(imatrix);

  if (! flag) {
    setTransform(true);

    return false;
  }

  int bit_num  = 7;
  int byte_num = 0;

  double y = 0.0;

  for (int i = 0; i < height; ++i) {
    double x = 0.0;

    int bit_mask = 1 << bit_num;

    for (int j = 0; j < width; ++j) {
      int color = 0;

      if (image_data[byte_num] & bit_mask)
        color = 1;

      bit_num--;

      if (bit_num < 0) {
        bit_num = 7;

        ++byte_num;
      }

      bit_mask = 1 << bit_num;

      double x11, y11, x21, y21;

      imatrix.multiplyPoint(x    , y    , &x11, &y11);
      imatrix.multiplyPoint(x + 1, y + 1, &x21, &y21);

      double x12, y12, x22, y22;

      ctmMatrix->multiplyPoint(x11, y11, &x12, &y12);
      ctmMatrix->multiplyPoint(x21, y21, &x22, &y22);

      PointList points;

      points.resize(4);

      points[0].x = x12; points[0].y = y12;
      points[1].x = x22; points[1].y = y12;
      points[2].x = x22; points[2].y = y22;
      points[3].x = x12; points[3].y = y22;

      if      (color == 0 && ! polarity)
        fillPolygon(points);
      else if (color == 1 &&   polarity)
        fillPolygon(points);

      x += 1.0;
    }

    if (bit_num != 7) {
      bit_num = 7;

      ++byte_num;
    }

    y += 1.0;
  }

  setTransform(true);

  return true;
}

void
CRenderer2D::
debugPrintImageMask(char *image_data, int width, int height, int polarity,
                    CMatrix2D *, CMatrix2D *)
{
  CStrUtil::printf("ImageMask\n");
  CStrUtil::printf("  Width    = %d\n", width);
  CStrUtil::printf("  Height   = %d\n", height);
  CStrUtil::printf("  Polarity = %d\n", polarity);
  CStrUtil::printf("  Data :-\n");

  int width1 = width;

  if ((width % 8) != 0)
    width1 += 8 - (width % 8);

  int num_bits  = width1*height;
  int num_bytes = num_bits/8;

  int i = 0;

  for ( ; i < num_bytes; ++i) {
    if (i % 8 == 0)
      CStrUtil::printf("    ");
    else
      CStrUtil::printf(", ");

    CStrUtil::printf("%x", image_data[i] & 0xFF);

    if (i % 8 == 7)
      CStrUtil::printf("\n");
  }

  if (i % 8 != 0)
    CStrUtil::printf("\n");
}

//-----------

void
CRenderer2D::
joinLines(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  if (! enabled_)
    return;

  if (getLineWidth() <= 0.0)
    return;

  if (! getLineDash().isDraw())
    return;

  bool save_anti_alias = antiAlias_;

  antiAlias_ = false;

  if      (getLineJoin() == LINE_JOIN_TYPE_MITRE)
    mitreJoinLines(p1, p2, p3);
  else if (getLineJoin() == LINE_JOIN_TYPE_ROUND)
    roundJoinLines(p1, p2, p3);
  else
    bevelJoinLines(p1, p2, p3);

  antiAlias_ = save_anti_alias;
}

void
CRenderer2D::
mitreJoinLines(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  if (getMitreLimit() == 1.0) {
    bevelJoinLines(p1, p2, p3);
    return;
  }

  PointList points;

  points.resize(3);

  points[0] = p1;
  points[1] = p2;
  points[2] = p3;

  CLineList2D path(points);

  double theta = path.includedAngle();

  double s = sin(theta/2.0);

  if (s == 0.0 || 1.0/s > getMitreLimit()) {
    bevelJoinLines(p1, p2, p3);

    return;
  }

  //------

  double x_diff = p2.x - p1.x;
  double y_diff = p2.y - p1.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  double g = CMathGen::atan2(x_diff, y_diff);

  CPoint2D pp1[4];

  addWidthToPointG1(p1, g, pp1[0], pp1[3]);
  addWidthToPointG1(p2, g, pp1[1], pp1[2]);

  //------

  x_diff = p3.x - p2.x;
  y_diff = p3.y - p2.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  g = CMathGen::atan2(x_diff, y_diff);

  CPoint2D pp2[4];

  addWidthToPointG1(p2, g, pp2[0], pp2[3]);
  addWidthToPointG1(p3, g, pp2[1], pp2[2]);

  //------

  CPoint2D pi[4];

  CLine2D line1(pp1[0], pp1[1]);
  CLine2D line2(pp2[0], pp2[1]);
  CLine2D line3(pp2[3], pp2[2]);
  CLine2D line4(pp1[3], pp1[2]);

  if (! line1.intersect(line2, pi[0])) return;
  if (! line1.intersect(line3, pi[1])) return;
  if (! line4.intersect(line2, pi[3])) return;
  if (! line4.intersect(line3, pi[2])) return;

  //------

  PointList points1;

  points1.resize(4);

  points1[0].x = pi [0].x; points1[0].y = pi [0].y;
  points1[1].x = pp1[1].x; points1[1].y = pp1[1].y;
  points1[2].x = p2    .x; points1[2].y = p2    .y;
  points1[3].x = pp2[0].x; points1[3].y = pp2[0].y;

  fillPolygon(points1);

  points1[0].x = p2    .x; points1[0].y = p2    .y;
  points1[1].x = pp2[3].x; points1[1].y = pp2[3].y;
  points1[2].x = pi [2].x; points1[2].y = pi [2].y;
  points1[3].x = pp1[2].x; points1[3].y = pp1[2].y;

  fillPolygon(points1);
}

void
CRenderer2D::
roundJoinLines(const CPoint2D &/*p1*/, const CPoint2D &p2, const CPoint2D &/*p3*/)
{
  double x_line_width, y_line_width;

  getTransformedLineWidth(&x_line_width, &y_line_width);

  fillEllipse(p2, x_line_width/2.0, y_line_width/2.0);
}

void
CRenderer2D::
bevelJoinLines(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  PointList points;

  points.resize(4);

  //------

  double x_diff = p2.x - p1.x;
  double y_diff = p2.y - p1.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  double g = CMathGen::atan2(x_diff, y_diff);

  addWidthToPointG1(p2, g, points[0], points[2]);

  //------

  x_diff = p2.x - p3.x;
  y_diff = p2.y - p3.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  g = CMathGen::atan2(x_diff, y_diff);

  addWidthToPointG1(p2, g, points[3], points[1]);

  //------

  fillPolygon(points);
}

void
CRenderer2D::
capLine(const CPoint2D &p1, const CPoint2D &p2)
{
  if (! enabled_)
    return;

  if (getLineWidth() <= 0.0)
    return;

  if      (getLineCap() == LINE_CAP_TYPE_BUTT )
    buttCapLine(p1, p2);
  else if (getLineCap() == LINE_CAP_TYPE_ROUND)
    roundCapLine(p1, p2);
  else if (getLineCap() == LINE_CAP_TYPE_SQUARE)
    squareCapLine(p1, p2);
}

void
CRenderer2D::
buttCapLine(const CPoint2D &/*p1*/, const CPoint2D &/*p2*/)
{
/*
  double x21 = p2.x - p1.x;
  double y21 = p2.y - p1.y;

  if (x21 == 0.0 && y21 == 0.0)
    return;

  double l = sqrt(x21*x21 + y21*y21)*2.0;

  double x_line_width, y_line_width;

  getTransformedLineWidth(&x_line_width, &y_line_width);

  double xd1 = y21*x_line_width/l;
  double yd1 = x21*y_line_width/l;

  double xd2 = x21*x_line_width/l;
  double yd2 = y21*y_line_width/l;

  PointList points;

  points.resize(4);

  points[1].x = p1.x + xd1; points[1].y = p1.y - yd1;
  points[2].x = p1.x - xd1; points[2].y = p1.y + yd1;

  points[0].x = points[1].x + xd2; points[0].y = points[1].y + yd2;
  points[3].x = points[2].x + xd2; points[3].y = points[2].y + yd2;

  fillPolygon(points);
*/
}

void
CRenderer2D::
roundCapLine(const CPoint2D &p1, const CPoint2D &/*p2*/)
{
  double x_line_width, y_line_width;

  getTransformedLineWidth(&x_line_width, &y_line_width);

  fillEllipse(p1, x_line_width/2.0, y_line_width/2.0);
}

void
CRenderer2D::
squareCapLine(const CPoint2D &p1, const CPoint2D &p2)
{
  double x_line_width, y_line_width;

  getTransformedLineWidth(&x_line_width, &y_line_width);

  double g = CMathGen::atan2(p2.x - p1.x, p2.y - p1.y);

  double s = sin(g);
  double c = cos(g);

  double xd = s*x_line_width/2.0;
  double yd = c*y_line_width/2.0;

  PointList points;

  points.resize(4);

  points[1].x = p1.x + xd; points[1].y = p1.y - yd;
  points[2].x = p1.x - xd; points[2].y = p1.y + yd;

  xd = c*x_line_width/2.0;
  yd = s*y_line_width/2.0;

  points[0].x = points[1].x - xd; points[0].y = points[1].y - yd;
  points[3].x = points[2].x - xd; points[3].y = points[2].y - yd;

  fillPolygon(points);
}

//-------

void
CRenderer2D::
lineJoinToPolygon(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3, PointList &points)
{
  if (getLineWidth() <= 0.0) {
    points.resize(0);
    return;
  }

  if      (getLineJoin() == LINE_JOIN_TYPE_MITRE)
    mitreLineJoinToPolygon(p1, p2, p3, points);
  else if (getLineJoin() == LINE_JOIN_TYPE_ROUND)
    roundLineJoinToPolygon(p1, p2, p3, points);
  else
    bevelLineJoinToPolygon(p1, p2, p3, points);
}

void
CRenderer2D::
mitreLineJoinToPolygon(const CPoint2D &p1, const CPoint2D &p2,
                       const CPoint2D &p3, PointList &points)
{
  if (getMitreLimit() == 1.0) {
    bevelLineJoinToPolygon(p1, p2, p3, points);
    return;
  }

  double theta = CMathGeom2D::IncludedAngle(p1, p2, p3);

  double s = sin(theta/2.0);

  if (s == 0.0 || 1.0/s > getMitreLimit()) {
    bevelLineJoinToPolygon(p1, p2, p3, points);

    return;
  }

  //------

  double x_diff = p2.x - p1.x;
  double y_diff = p2.y - p1.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  double g = CMathGen::atan2(x_diff, y_diff);

  CPoint2D pp1[4];

  addWidthToPointG1(p1, g, pp1[0], pp1[3]);
  addWidthToPointG1(p2, g, pp1[1], pp1[2]);

  //------

  x_diff = p3.x - p2.x;
  y_diff = p3.y - p2.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  g = CMathGen::atan2(x_diff, y_diff);

  CPoint2D pp2[4];

  addWidthToPointG1(p2, g, pp2[0], pp2[3]);
  addWidthToPointG1(p3, g, pp2[1], pp2[2]);

  //------

  CPoint2D pi[4];

  CLine2D line1(pp1[0], pp1[1]);
  CLine2D line2(pp2[0], pp2[1]);
  CLine2D line3(pp2[3], pp2[2]);
  CLine2D line4(pp1[3], pp1[2]);

  if (! line1.intersect(line2, pi[0])) return;
  if (! line1.intersect(line3, pi[1])) return;
  if (! line4.intersect(line2, pi[3])) return;
  if (! line4.intersect(line3, pi[2])) return;

  //------

  points.resize(7);

  points[0].x = pp2[0].x; points[0].y = pp2[0].y;
  points[1].x = pi [0].x; points[1].y = pi [0].y;
  points[2].x = pp1[1].x; points[2].y = pp1[1].y;
  points[3].x = p2    .x; points[3].y = p2    .y;
  points[4].x = pp2[3].x; points[4].y = pp2[3].y;
  points[5].x = pi [2].x; points[5].y = pi [2].y;
  points[6].x = pp1[2].x; points[6].y = pp1[2].y;
}

void
CRenderer2D::
roundLineJoinToPolygon(const CPoint2D &/*p1*/, const CPoint2D &/*p2*/,
                       const CPoint2D &/*p3*/, PointList &points)
{
  // TODO
  points.resize(0);
}

void
CRenderer2D::
bevelLineJoinToPolygon(const CPoint2D &p1, const CPoint2D &p2,
                       const CPoint2D &p3, PointList &points)
{
  double x_diff = p2.x - p1.x;
  double y_diff = p2.y - p1.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  double g = CMathGen::atan2(x_diff, y_diff);

  addWidthToPointG1(p2, g, points[0], points[2]);

  //------

  x_diff = p2.x - p3.x;
  y_diff = p2.y - p3.y;

  if (x_diff == 0.0 && y_diff == 0.0)
    return;

  g = CMathGen::atan2(x_diff, y_diff);

  addWidthToPointG1(p2, g, points[3], points[1]);
}

//-------

void
CRenderer2D::
addWidthToPointG1(const CPoint2D &point, double g1, CPoint2D &point1, CPoint2D &point2)
{
  untransformPoint1(CPoint2D(      0,       0), point1);
  untransformPoint1(CPoint2D(cos(g1), sin(g1)), point2);

  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  double gg = CMathGen::atan2(x21, y21);

  double dx = getLineWidth()*sin(gg)/2.0;
  double dy = getLineWidth()*cos(gg)/2.0;

  untransformPoint1(point, point2);

  point1.x = point2.x + dx;
  point1.y = point2.y - dy;
  point2.x = point2.x - dx;
  point2.y = point2.y + dy;

  transformPoint1(point1, point1);
  transformPoint1(point2, point2);
}

void
CRenderer2D::
addWidthToPointG2(const CPoint2D &point, double g1, double g2, CPoint2D &point1, CPoint2D &point2)
{
  untransformPoint1(CPoint2D(      0,       0), point1);
  untransformPoint1(CPoint2D(cos(g1), sin(g1)), point2);

  double x21 = point2.x - point1.x;
  double y21 = point2.y - point1.y;

  double gg = CMathGen::atan2(x21, y21);

  g2 -= g1 - gg;

  double dx = getLineWidth()*sin(g2)/2.0;
  double dy = getLineWidth()*cos(g2)/2.0;

  untransformPoint1(point, point2);

  point1.x = point2.x + dx;
  point1.y = point2.y - dy;
  point2.x = point2.x - dx;
  point2.y = point2.y + dy;

  transformPoint1(point1, point1);
  transformPoint1(point2, point2);
}

void
CRenderer2D::
adjustPoint(CPoint2D &p)
{
  CPoint2D p1;

  windowToPixel(p, p1);

  p1.x = CMathRound::RoundDown(p1.x - 0.25) + 0.25;
  p1.y = CMathRound::RoundDown(p1.y - 0.25) + 0.25;

  pixelToWindow(p1, p);
}

//-------

void
CRenderer2D::
clear()
{
  pixelRenderer_->clear();
}

void
CRenderer2D::
fill()
{
  pixelRenderer_->fill();
}

//---------

void
CRenderer2D::
textBounds(const std::string &text, CBBox2D &box)
{
  CFontPtr font;

  getFont(font);

  double x = 0.0, y = 0.0;

  CFreeTypeMgrInst->setFont(font);

  CFreeTypeRenderer2D renderer(this);

  CFreeTypeMgrInst->pathString(&renderer, text.c_str(), &x, &y);

  CPath2D *path = renderer.getPath();

  path->bbox(box);
}

void
CRenderer2D::
drawText(const CPoint2D &point, const std::string &text)
{
  if (! enabled_)
    return;

  CIPoint2D p;

  transformPointToPixel(point, p);

  pixelRenderer_->drawString(p, text);
}

void
CRenderer2D::
drawTextInRect(const CBBox2D &rect, const std::string &text)
{
  if (! enabled_)
    return;

  CIPoint2D p1, p2;

  transformPointToPixel(rect.getMin(), p1);
  transformPointToPixel(rect.getMax(), p2);

  CIBBox2D irect(p1, p2);

  pixelRenderer_->drawStringInRect(irect, text);
}

void
CRenderer2D::
fillText(const CPoint2D &point, const std::string &text)
{
  if (! enabled_)
    return;

  CIPoint2D p;

  transformPointToPixel(point, p);

  pixelRenderer_->drawString(p, text);
}

void
CRenderer2D::
setTextSize(double, double)
{
}

void
CRenderer2D::
getTextLimits(const std::string &, double *text_x, double *text_y)
{
  *text_x = 1.0;
  *text_y = 1.0;
}

//-------

bool
CRenderer2D::
isAntiAlias() const
{
  return antiAlias_;
}

void
CRenderer2D::
setAntiAlias(bool flag)
{
  antiAlias_ = flag;
}

//-------

void
CRenderer2D::
setTransform(bool flag)
{
  transformFlag_ = flag;
}

void
CRenderer2D::
getTransformedLineWidth(double *x_line_width, double *y_line_width) const
{
  CPoint2D p1, p2;

  transformPoint1(CPoint2D(0, 0), p1);
  transformPoint1(CPoint2D(1, 0), p2);

  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;

  double l = sqrt(dx*dx + dy*dy);

  *x_line_width = getLineWidth()*l;

  transformPoint1(CPoint2D(0, 1), p2);

  dx = p2.x - p1.x;
  dy = p2.y - p1.y;

  l = sqrt(dx*dx + dy*dy);

  *y_line_width = getLineWidth()*l;
}

void
CRenderer2D::
transformPointToPixel(const CPoint2D &w, CPoint2D &p) const
{
  CPoint2D tw;

  if (transformFlag_)
    transformPoint1(w, tw);
  else
    tw = w;

  windowToPixel(tw, p);
}

void
CRenderer2D::
transformPointToPixel(const CPoint2D &w, CIPoint2D &p) const
{
  CPoint2D tp;

  transformPointToPixel(w, tp);

  p.x = CMathRound::Round(tp.x);
  p.y = CMathRound::Round(tp.y);
}

void
CRenderer2D::
transformPixelToPoint(const CPoint2D &p, CPoint2D &w) const
{
  CPoint2D tp;

  pixelToWindow(p, tp);

  if (transformFlag_)
    untransformPoint1(tp, w);
  else
    w = tp;
}

void
CRenderer2D::
transformPixelToPoint(const CIPoint2D &p, CPoint2D &w) const
{
  CPoint2D tp;

  pixelToWindow(p, tp);

  if (transformFlag_)
    untransformPoint1(tp, w);
  else
    w = tp;
}

void
CRenderer2D::
transformPoint(const CPoint2D &p1, CPoint2D &p2) const
{
  if (transformFlag_)
    transformPoint1(p1, p2);
  else
    p2 = p1;
}

void
CRenderer2D::
transformPoint1(const CPoint2D &p1, CPoint2D &p2) const
{
  viewMatrix_.multiplyPoint(p1.x, p1.y, &p2.x, &p2.y);
}

void
CRenderer2D::
untransformPoint(const CPoint2D &p1, CPoint2D &p2) const
{
  if (transformFlag_)
    untransformPoint1(p1, p2);
  else
    p2 = p1;
}

void
CRenderer2D::
untransformPoint1(const CPoint2D &p1, CPoint2D &p2) const
{
  viewIMatrix_.multiplyPoint(p1.x, p1.y, &p2.x, &p2.y);
}

void
CRenderer2D::
pixelLengthToWindowLength(double pl, double *wl) const
{
  CPoint2D w1, w2;

  pixelToWindow(CPoint2D(0 , 0 ), w1);
  pixelToWindow(CPoint2D(pl, pl), w2);

  double wl1 = fabs(w2.x - w1.x);
  double wl2 = fabs(w2.y - w1.y);

  *wl = (wl1 > wl2 ? wl1 : wl2);
}

void
CRenderer2D::
pixelWidthToWindowWidth(double pw, double *ww) const
{
  CPoint2D w1, w2;

  pixelToWindow(CPoint2D(0 , 0 ), w1);
  pixelToWindow(CPoint2D(pw, pw), w2);

  *ww = fabs(w2.x - w1.x);
}

void
CRenderer2D::
pixelHeightToWindowHeight(double ph, double *wh) const
{
  CPoint2D w1, w2;

  pixelToWindow(CPoint2D(0 , 0 ), w1);
  pixelToWindow(CPoint2D(ph, ph), w2);

  *wh = fabs(w2.y - w1.y);
}

void
CRenderer2D::
windowLengthToPixelLength(double l, double *pl) const
{
  CPoint2D p1, p2;

  windowToPixel(CPoint2D(0.0, 0.0), p1);
  windowToPixel(CPoint2D(l  , l  ), p2);

  double pl1 = p2.x - p1.x;
  double pl2 = p1.y - p2.y;

  *pl = (pl1 > pl2 ? pl1 : pl2);
}

void
CRenderer2D::
windowLengthToPixelLength(double l, int *pl) const
{
  double pl1;

  windowLengthToPixelLength(l, &pl1);

  *pl = CMathRound::Round(pl1);
}

void
CRenderer2D::
windowToPixel(const CPoint2D &w, CIPoint2D &p, CMathRound::Rounding rounding) const
{
  CPoint2D p1;

  windowToPixel(w, p1);

  p.x = CMathRound::Round(p1.x, rounding);
  p.y = CMathRound::Round(p1.y, rounding);
}

void
CRenderer2D::
windowToPixel(const CPoint2D &w, CPoint2D &p) const
{
  displayRange_.windowToPixel(w.x, w.y, &p.x, &p.y);
}

void
CRenderer2D::
pixelToWindow(const CIPoint2D &p, CPoint2D &w) const
{
  displayRange_.pixelToWindow(p.x, p.y, &w.x, &w.y);
}

void
CRenderer2D::
pixelToWindow(const CPoint2D &p, CPoint2D &w) const
{
  displayRange_.pixelToWindow(p.x, p.y, &w.x, &w.y);
}

//-----------

void
CRenderer2D::
drawImage(const CPoint2D &point, CImagePtr image)
{
  CIPoint2D point1;

  transformPointToPixel(point, point1);

  pixelRenderer_->drawImage(point1, image);
}

void
CRenderer2D::
drawAlphaImage(const CPoint2D &point, CImagePtr image)
{
  CIPoint2D point1;

  transformPointToPixel(point, point1);

  pixelRenderer_->drawAlphaImage(point1, image);
}

void
CRenderer2D::
drawImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CIPoint2D p1, p2;

  transformPointToPixel(bbox.getMin(), p1);
  transformPointToPixel(bbox.getMax(), p2);

  pixelRenderer_->drawImageInBox(CIBBox2D(p1, p2), image);
}

void
CRenderer2D::
drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  CIPoint2D p1, p2;

  transformPointToPixel(bbox.getMin(), p1);
  transformPointToPixel(bbox.getMax(), p2);

  pixelRenderer_->drawAlphaImageInBox(CIBBox2D(p1, p2), image);
}

void
CRenderer2D::
drawRoundedRectangle(const CPoint2D &p1, const CPoint2D &p2, double xr, double yr)
{
  pathInit();

  pathRoundedRectangle(p1, p2, xr, yr);

  pathStroke();
}

void
CRenderer2D::
fillRoundedRectangle(const CPoint2D &p1, const CPoint2D &p2, double xr, double yr)
{
  pathInit();

  pathRoundedRectangle(p1, p2, xr, yr);

  pathFill();
}

void
CRenderer2D::
drawRectangle(const CBBox2D &bbox)
{
  CPoint2D p1 = bbox.getMin();
  CPoint2D p3 = bbox.getMax();

  CPoint2D p2(p3.x, p1.y);
  CPoint2D p4(p1.x, p3.y);

  drawLine (p1, p2);
  joinLines(p1, p2, p3);
  drawLine (p2, p3);
  joinLines(p2, p3, p4);
  drawLine (p3, p4);
  joinLines(p3, p4, p1);
  drawLine (p4, p1);
  joinLines(p4, p1, p2);
}

void
CRenderer2D::
fillRectangle(const CBBox2D &bbox)
{
  double x1 = bbox.getXMin();
  double y1 = bbox.getYMin();
  double x2 = bbox.getXMax();
  double y2 = bbox.getYMax();

  PointList points;

  points.resize(4);

  points[0].x = x1; points[0].y = y1;
  points[1].x = x2; points[1].y = y1;
  points[2].x = x2; points[2].y = y2;
  points[3].x = x1; points[3].y = y2;

  fillPolygon(points);
}

void
CRenderer2D::
gradientFillRectangle(const CBBox2D &bbox, CGenGradient *gradient)
{
  double x1 = bbox.getXMin();
  double y1 = bbox.getYMin();
  double x2 = bbox.getXMax();
  double y2 = bbox.getYMax();

  GenGradient2DFill filler(gradient, x1, y1, x2, y2);

  filler.fill(this);
}

void
CRenderer2D::
fillFilledRectangle(const CBBox2D &bbox, const CRenderer2DFiller &filler)
{
  double x1 = bbox.getXMin();
  double y1 = bbox.getYMin();
  double x2 = bbox.getXMax();
  double y2 = bbox.getYMax();

  PointList points;

  points.resize(4);

  points[0].x = x1; points[0].y = y1;
  points[1].x = x2; points[1].y = y1;
  points[2].x = x2; points[2].y = y2;
  points[3].x = x1; points[3].y = y2;

  fillFilledPolygon(points, filler);
}

void
CRenderer2D::
drawPoint(const CPoint2D &point)
{
  if (antiAlias_) {
    std::vector<CPoint2D> points;

    points.resize(4);

    transformPointToPixel(point, points[0]);

    points[2].x = points[0].x + 1;
    points[2].y = points[0].y + 1;
    points[1].x = points[2].x;
    points[1].y = points[0].y;
    points[3].x = points[0].x;
    points[3].y = points[2].y;

    pixelRenderer_->fillAAPolygon(points);
  }
  else {
    CIPoint2D point1;

    transformPointToPixel(point, point1);

    pixelRenderer_->drawPoint(point1);
  }
}

void
CRenderer2D::
drawSymbol(const CPoint2D &point, CSymbol2D::Type symbol)
{
  CIPoint2D point1;

  transformPointToPixel(point, point1);

  pixelRenderer_->drawSymbol(point1, symbol);
}

//----------

void
CRenderer2D::
setFont(CFontPtr font)
{
  if (pixelRenderer_)
    pixelRenderer_->setFont(font);
}

void
CRenderer2D::
getFont(CFontPtr &font) const
{
  pixelRenderer_->getFont(font);
}

double
CRenderer2D::
getCharWidth()
{
  int width = pixelRenderer_->getCharWidth();

  CPoint2D p;

  transformPixelToPoint(CPoint2D(width, 0), p);

  return p.x;
}

double
CRenderer2D::
getCharAscent()
{
  int ascent = pixelRenderer_->getCharAscent();

  CPoint2D p1, p2;

  transformPixelToPoint(CPoint2D(0, ascent), p1);
  transformPixelToPoint(CPoint2D(0, 0     ), p2);

  return fabs(p2.y - p1.y);
}

double
CRenderer2D::
getCharDescent()
{
  int descent = pixelRenderer_->getCharDescent();

  CPoint2D p1, p2;

  transformPixelToPoint(CPoint2D(0, descent), p1);
  transformPixelToPoint(CPoint2D(0, 0      ), p2);

  return fabs(p2.y - p1.y);
}

double
CRenderer2D::
getCharHeight()
{
  int height = pixelRenderer_->getCharHeight();

  CPoint2D p1, p2;

  transformPixelToPoint(CPoint2D(0, height), p1);
  transformPixelToPoint(CPoint2D(0, 0     ), p2);

  return fabs(p2.y - p1.y);
}

double
CRenderer2D::
getStringWidth(const std::string &str)
{
  int width = pixelRenderer_->getStringWidth(str);

  CPoint2D p;

  transformPixelToPoint(CPoint2D(width, 0), p);

  return p.x;
}

//----------

void
CRenderer2D::
initClip()
{
  if (! enabled_ || ! pixelRenderer_)
    return;

  pixelRenderer_->resetClip();
}

//----------

void
CRenderer2D::
pathDrawPolygon(const PointList &points)
{
  if (points.empty())
    return;

  pathInit();

  pathPolygon(points);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathFillPolygon(const PointList &points)
{
  if (points.empty())
    return;

  pathInit();

  pathPolygon(points);

  pathClose();

  pathFill();
}

void
CRenderer2D::
pathPolygon(const PointList &points)
{
  initPath();

  getPath()->polygon(points);
}

void
CRenderer2D::
pathDrawCircle(const CPoint2D &center, double r)
{
  pathDrawEllipse(center, r, r);
}

void
CRenderer2D::
pathFillCircle(const CPoint2D &center, double r)
{
  pathFillEllipse(center, r, r);
}

void
CRenderer2D::
pathCircle(const CPoint2D &center, double r)
{
  pathEllipse(center, r, r);
}

void
CRenderer2D::
pathDrawEllipse(const CPoint2D &center, double xr, double yr)
{
  pathInit();

  pathEllipse(center, xr, yr);

  pathStroke();
}

void
CRenderer2D::
pathFillEllipse(const CPoint2D &center, double xr, double yr)
{
  pathInit();

  pathEllipse(center, xr, yr);

  pathFill();
}

void
CRenderer2D::
pathEllipse(const CPoint2D &center, double xr, double yr)
{
  initPath();

  getPath()->ellipse(center, xr, yr);
}

void
CRenderer2D::
pathDrawArc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  pathInit();

  pathArc(center, xr, yr, angle1, angle2);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathFillArc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  pathInit();

  pathArc(center, xr, yr, angle1, angle2);

  pathClose();

  pathFill();
}

void
CRenderer2D::
pathArc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  initPath();

  getPath()->arc(center, xr, yr, angle1, angle2);
}

void
CRenderer2D::
pathDrawArcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  pathInit();

  pathArcN(center, xr, yr, angle1, angle2);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathFillArcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  pathInit();

  pathArcN(center, xr, yr, angle1, angle2);

  pathClose();

  pathFill();
}

void
CRenderer2D::
pathArcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  initPath();

  getPath()->arcN(center, xr, yr, angle1, angle2);
}

void
CRenderer2D::
pathDrawBezier(C3Bezier2D &bezier)
{
  pathInit();

  pathBezier(bezier);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathDrawBezier(C2Bezier2D &bezier)
{
  pathInit();

  pathBezier(bezier);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathBezier(C3Bezier2D &bezier)
{
  initPath();

  getPath()->bezier(bezier);
}

void
CRenderer2D::
pathBezier(C2Bezier2D &bezier)
{
  initPath();

  getPath()->bezier(bezier);
}

void
CRenderer2D::
pathDrawRectangle(const CBBox2D &bbox)
{
  pathInit();

  pathRectangle(bbox);

  pathClose();

  pathStroke();
}

void
CRenderer2D::
pathFillRectangle(const CBBox2D &bbox)
{
  pathInit();

  pathRectangle(bbox);

  pathClose();

  pathFill();
}

void
CRenderer2D::
pathRectangle(const CBBox2D &bbox)
{
  initPath();

  getPath()->rectangle(bbox.getMin(), bbox.getMax());
}

//-------------

void
CRenderer2D::
setRegion(CRendererRegion2D *region)
{
  region_ = RendererRegion2DP(region);
}

CRendererRegion2D *
CRenderer2D::
getRegion()
{
  if (! region_.get())
    region_ = RendererRegion2DP(new CRendererRegion2D(this));

  return region_.get();
}

CPath2D *
CRenderer2D::
createPath() const
{
  auto *th = const_cast<CRenderer2D *>(this);

  auto *path = new CPath2D;

  auto *renderer = th->createPathRenderer();

  if (renderer)
    path->setRenderer(renderer);
  else
    path->setRenderer(const_cast<CRenderer2D *>(this));

  return path;
}

CRenderer2D *
CRenderer2D::
createImageRenderer() const
{
  return new CImageRenderer2D;
}

CPath2DRenderer *
CRenderer2D::
createPathRenderer() const
{
  return nullptr;
}

void
CRenderer2D::
drawFunction(const CRenderer2DFunc &func)
{
  double x1 = func.getStart ();
  double x2 = func.getEnd   ();
  uint   n  = func.getNSteps();

  double dx = (x2 - x1)/(n - 1);

  double y1 = func.y(x1);

  drawSymbol(CPoint2D(x1, y1), getSymbol());

  for (uint i = 1; i < n; ++i) {
    x2 = x1 + dx;

    double y2 = func.y(x2);

    drawLine(CPoint2D(x1, y1), CPoint2D(x2, y2));

    drawSymbol(CPoint2D(x2, y2), getSymbol());

    x1 = x2;
    y1 = y2;
  }
}
