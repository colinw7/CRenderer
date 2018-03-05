#include <CQRenderer2D.h>
#include <CQImage.h>
#include <CQFont.h>
#include <CQUtil.h>
#include <CQPath2D.h>
#include <CQImageRenderer2D.h>
#include <CLinearGradient.h>
#include <CRadialGradient.h>

#include <QPainter>

// note: this implementation does not disable this overload for array types
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

CQRenderer2D::
CQRenderer2D(QPainter *painter) :
 CRenderer2D(0), painter_(painter)
{
  if (! painter_) {
    painterP_ = make_unique<QPainter>();
    painter_  = painterP_.get();
  }

  pen_   = make_unique<QPen  >();
  brush_ = make_unique<QBrush>();

  brush_->setStyle(Qt::SolidPattern);

  CFontPtr font = CFontMgrInst->lookupFont("helvetica", CFONT_STYLE_NORMAL, 8);

  setFont(font);
}

CQRenderer2D::
~CQRenderer2D()
{
}

void
CQRenderer2D::
beginDraw()
{
  drawing_ = true;

  setQTransform();
}

void
CQRenderer2D::
endDraw()
{
  drawing_ = false;
}

void
CQRenderer2D::
startDoubleBuffer(bool do_clear)
{
  int width  = getWidth ();
  int height = getHeight();

  if (width != pixmapWidth_ || height != pixmapHeight_) {
    pixmapWidth_  = width;
    pixmapHeight_ = height;

    pixmap_ = make_unique<QPixmap>(pixmapWidth_, pixmapHeight_);

    pixmap_->fill(Qt::black);
  }

  getQPainter()->begin(pixmap_.get());

  getQPainter()->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (do_clear)
    clear();
}

void
CQRenderer2D::
endDoubleBuffer(bool copy)
{
  getQPainter()->end();

  if (copy)
    copyDoubleBuffer();
}

void
CQRenderer2D::
copyDoubleBuffer()
{
  if (pixmap_) {
    beginDraw();

    getQPainter()->drawPixmap(0, 0, *pixmap_.get());

    endDraw();
  }
}

void
CQRenderer2D::
setDataRange(double xmin, double ymin, double xmax, double ymax)
{
  CRenderer2D::setDataRange(xmin, ymin, xmax, ymax);

  setQTransform();
}

void
CQRenderer2D::
resetDataRange()
{
  CRenderer2D::resetDataRange();

  setQTransform();
}

void
CQRenderer2D::
setViewMatrix(const CMatrix2D &view_matrix)
{
  CRenderer2D::setViewMatrix(view_matrix);

  setQTransform();
}

void
CQRenderer2D::
unsetViewMatrix()
{
  CRenderer2D::unsetViewMatrix();

  unsetQTransform();
}

void
CQRenderer2D::
setQTransform()
{
  if (! drawing_) return;

  double xmin, ymin, xmax, ymax;

  getDataRange(&xmin, &ymin, &xmax, &ymax);

  double w = xmax - xmin;
  double h = ymax - ymin;

#if 0
  getQPainter()->setWindow(-1, -1, 2, 2);

  QTransform transform;

  transform.translate(-1, -1);
  transform.scale    (2/w, -2/h);
  transform.translate(-xmin, -ymax);

  const CMatrix2D *matrix = getViewMatrix();

  double a, b, c, d, tx, ty;

  matrix->getValues(&a, &b, &c, &d, &tx, &ty);

  QTransform transform1(a, b, c, d, tx, ty);

  transform = transform1*transform;

  getQPainter()->setWorldTransform(transform);
#endif

  double size = std::min(w, h);

  int pw = getWidth ();
  int ph = getHeight();

  int side = std::min(pw, ph);

  int dw = (pw - side)/2;
  int dh = (ph - side)/2;

  getQPainter()->setWorldMatrix(QMatrix());

  getQPainter()->translate(dw, side + dh);
  getQPainter()->scale(side/size, -side/size);
  getQPainter()->translate(-xmin, -ymin);

  getQPainter()->setViewTransformEnabled(true);

  hasTransform_ = true;
}

void
CQRenderer2D::
unsetQTransform()
{
  if (! drawing_) return;

#if 0
  int pxmin, pymin, pxmax, pymax;

  getPixelRange(&pxmin, &pymin, &pxmax, &pymax);

  getQPainter()->setWindow(pxmin, pymin, pxmax - pxmin, pymax - pymin);

  QTransform transform;

  getQPainter()->setWorldTransform(transform);
#endif

  getQPainter()->setViewTransformEnabled(false);

  hasTransform_ = false;
}

void
CQRenderer2D::
setForeground(const CRGBA &fg)
{
  qfg_ = CQUtil::rgbaToColor(fg);

  pen_  ->setColor(qfg_);
  brush_->setColor(qfg_);
}

void
CQRenderer2D::
setBackground(const CRGBA &bg)
{
  qbg_ = CQUtil::rgbaToColor(bg);
}

void
CQRenderer2D::
setLineWidth(double width)
{
  if (width <= 0.0) width = 1.0;

  QVector<qreal> dashes = pen_->dashPattern();

  double oldWidth = pen_->widthF();

  pen_->setWidthF(width);

  for (int i = 0; i < dashes.size(); ++i)
    dashes[i] *= oldWidth/width;

  CRenderer2D::setLineWidth(width);
}

void
CQRenderer2D::
setLineDash(const CLineDash &line_dash)
{
  double width = pen_->widthF();

  if (width <= 0.0) width = 1.0;

  int num = line_dash.getNumLengths();

  if (num > 0) {
    pen_->setStyle(Qt::CustomDashLine);

    pen_->setDashOffset(line_dash.getOffset());

    QVector<qreal> dashes;

    for (int i = 0; i < num; ++i)
      dashes << line_dash.getLength(i)/width;

    pen_->setDashPattern(dashes);
  }
  else
    pen_->setStyle(Qt::SolidLine);
}

void
CQRenderer2D::
setLineJoin(CLineJoinType join)
{
  switch (join) {
    case LINE_JOIN_TYPE_MITRE:
      pen_->setJoinStyle(Qt::MiterJoin);
      break;
    case LINE_JOIN_TYPE_ROUND:
      pen_->setJoinStyle(Qt::RoundJoin);
      break;
    case LINE_JOIN_TYPE_BEVEL:
      pen_->setJoinStyle(Qt::BevelJoin);
      break;
    default:
      break;
  }
}

void
CQRenderer2D::
setLineCap(CLineCapType cap)
{
  switch (cap) {
    case LINE_CAP_TYPE_BUTT:
      pen_->setCapStyle(Qt::FlatCap);
      break;
    case LINE_CAP_TYPE_ROUND:
      pen_->setCapStyle(Qt::RoundCap);
      break;
    case LINE_CAP_TYPE_SQUARE:
      pen_->setCapStyle(Qt::SquareCap);
      break;
    default:
      break;
  }
}

void
CQRenderer2D::
setMitreLimit(double limit)
{
  pen_->setMiterLimit(limit);
}

double
CQRenderer2D::
getMitreLimit() const
{
  return pen_->miterLimit();
}

void
CQRenderer2D::
setFillType(CFillType fill_type)
{
  CQPath2D *path = getQPath();

  path->setFillType(fill_type);
}

CFillType
CQRenderer2D::
getFillType() const
{
  CQPath2D *path = getQPath();

  return path->getFillType();
}

CQPath2D *
CQRenderer2D::
getQPath() const
{
  initPath();

  CQPath2D *path = dynamic_cast<CQPath2D *>(getPath());

  return path;
}

void
CQRenderer2D::
clear()
{
  QBrush brush;

  brush.setColor(qbg_);
  brush.setStyle(Qt::SolidPattern);

  int w = getWidth ();
  int h = getHeight();

  getQPainter()->setViewTransformEnabled(false);

  getQPainter()->fillRect(QRect(0, 0, w, h), brush);

  getQPainter()->setViewTransformEnabled(true);

#if 0
  QRect      window    = getQPainter()->window();
  QTransform transform = getQPainter()->worldTransform();

  getQPainter()->setWindow        (0, 0, 1, 1);
  getQPainter()->setWorldTransform(QTransform());

  getQPainter()->fillRect(0, 0, 1, 1, brush);

  getQPainter()->setWindow        (window);
  getQPainter()->setWorldTransform(transform);
#endif
}

void
CQRenderer2D::
fill()
{
  QBrush brush;

  brush.setColor(qfg_);
  brush.setStyle(Qt::SolidPattern);

  int w = getWidth ();
  int h = getHeight();

  getQPainter()->setViewTransformEnabled(false);

  getQPainter()->fillRect(QRect(0, 0, w, h), brush);

  getQPainter()->setViewTransformEnabled(true);

#if 0
  QRect      window    = getQPainter()->window();
  QTransform transform = getQPainter()->worldTransform();

  getQPainter()->setWindow        (0, 0, 1, 1);
  getQPainter()->setWorldTransform(QTransform());

  getQPainter()->fillRect(0, 0, 1, 1, brush);

  getQPainter()->setWindow        (window);
  getQPainter()->setWorldTransform(transform);
#endif
}

void
CQRenderer2D::
drawPoint(const CPoint2D &point)
{
  getQPainter()->setPen(*getQPen());

  getQPainter()->drawPoint(QPointF(point.x, point.y));
}

void
CQRenderer2D::
drawLine(const CPoint2D &point1, const CPoint2D &point2)
{
  getQPainter()->setPen(*getQPen());

  getQPainter()->drawLine(QPointF(point1.x, point1.y),
                          QPointF(point2.x, point2.y));
}

void
CQRenderer2D::
drawSymbol(const CPoint2D &point, CSymbol2D::Type symbol)
{
  getQPainter()->setPen(*getQPen());

  if (CSymbol2DMgr::isSymbol(symbol)) {
    const CSymbol2D &psymbol = CSymbol2DMgr::getSymbol(symbol);

    uint size = getPixelRenderer()->getSymbolSize();

    double wsize;

    pixelLengthToWindowLength(int(size), &wsize);

    for (uint i = 0; i < psymbol.lines.size(); ++i) {
      CPoint2D start(psymbol.lines[i].x1, psymbol.lines[i].y1);
      CPoint2D end  (psymbol.lines[i].x2, psymbol.lines[i].y2);

      drawLine(point + start*wsize, point + end*wsize);
    }
  }
  else
    drawPoint(point);
}

void
CQRenderer2D::
drawRectangle(const CBBox2D &rect)
{
  getQPainter()->setPen(*getQPen());

  getQPainter()->drawRect(QRectF(rect.getXMin(), rect.getYMin(),
                                 rect.getWidth(), rect.getHeight()));
}

void
CQRenderer2D::
fillRectangle(const CBBox2D &rect)
{
  brush_->setStyle(Qt::SolidPattern);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  getQPainter()->fillRect(QRectF(rect.getXMin(), rect.getYMin(),
                                 rect.getWidth(), rect.getHeight()),
                          *brush_);
}

void
CQRenderer2D::
gradientFillRectangle(const CBBox2D &rect, CGenGradient *gradient)
{
  CLinearGradient *lgradient = dynamic_cast<CLinearGradient *>(gradient);

  if (lgradient) {
    QLinearGradient qgradient = CQUtil::toQGradient(lgradient);

    getQPainter()->fillRect(QRectF(rect.getXMin(), rect.getYMin(),
                                   rect.getWidth(), rect.getHeight()),
                            QBrush(qgradient));

    return;
  }

  CRadialGradient *rgradient = dynamic_cast<CRadialGradient *>(gradient);

  if (rgradient) {
    QRadialGradient qgradient = CQUtil::toQGradient(rgradient);

    getQPainter()->fillRect(QRectF(rect.getXMin(), rect.getYMin(),
                                   rect.getWidth(), rect.getHeight()),
                            QBrush(qgradient));

    return;
  }
}

void
CQRenderer2D::
drawPolygon(const PointList &points)
{
  getQPainter()->setPen(*pen_);

  QPainterPath path;

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->strokePath(path, *pen_);
}

void
CQRenderer2D::
fillPolygon(const PointList &points)
{
  brush_->setStyle(Qt::SolidPattern);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?
                     Qt::WindingFill : Qt::OddEvenFill);

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);
}

void
CQRenderer2D::
fillImagePolygon(const PointList &points, CImagePtr image)
{
  if (! image.isValid()) return;

  QImage &qimage = image.cast<CQImage>()->getQImage();

  brush_->setTextureImage(qimage);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?
                     Qt::WindingFill : Qt::OddEvenFill);

  uint num_points = points.size();

  if (num_points < 1) return;

  path.moveTo(points[0].x, points[0].y);

  for (uint i = 1; i < num_points; ++i)
    path.lineTo(points[i].x, points[i].y);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);

  brush_->setStyle(Qt::SolidPattern);
}

void
CQRenderer2D::
fillGradientPolygon(const PointList &points, CRefPtr<CGenGradient> gradient)
{
  CLinearGradient *lg;
  CRadialGradient *rg;

  if      ((lg = dynamic_cast<CLinearGradient*>(gradient.getPtr())) != 0) {
    QLinearGradient qlg = CQUtil::toQGradient(lg);

    getQPainter()->setBrush(QBrush(qlg));
  }
  else if ((rg = dynamic_cast<CRadialGradient*>(gradient.getPtr())) != 0) {
    QRadialGradient qrg = CQUtil::toQGradient(rg);

    getQPainter()->setBrush(QBrush(qrg));
  }

  QTransform transform;

  bool flip_y = display_range_.getFlipY();

  if (! flip_y) {
    transform = getQPainter()->worldTransform();

    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);
  }

  QPainterPath path;

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?  Qt::WindingFill : Qt::OddEvenFill);

  uint num_points = points.size();

  if (num_points < 1) return;

  if (! flip_y) {
    path.moveTo(points[0].x, -points[0].y);

    for (uint i = 1; i < num_points; ++i)
      path.lineTo(points[i].x, -points[i].y);
  }
  else {
    path.moveTo(points[0].x, points[0].y);

    for (uint i = 1; i < num_points; ++i)
      path.lineTo(points[i].x, points[i].y);
  }

  path.closeSubpath();

  getQPainter()->fillPath(path, getQPainter()->brush());

  if (! flip_y)
    getQPainter()->setWorldTransform(transform);
}

void
CQRenderer2D::
drawText(const CPoint2D &point, const std::string &text)
{
  bool flip_y = display_range_.getFlipY();

  flip_y = true;

  CQFont *qfont = getQFont();

  double xscale = 1;
  double yscale = (! flip_y ? -1 : 1);

  QTransform transform = getQPainter()->worldTransform();

  QTransform transform1 = transform;

  transform1.scale(xscale, yscale);

  getQPainter()->setWorldTransform(transform1, true);

  double x1 = point.x/xscale;
  double y1 = point.y/yscale;

  QPainterPath path;

  path.addText(QPointF(x1, y1), qfont->getQFont(), text.c_str());

  getQPainter()->strokePath(path, *pen_);

  getQPainter()->setWorldTransform(transform);
}

void
CQRenderer2D::
drawTextInRect(const CBBox2D &rect, const std::string &text)
{
  bool flip_y = display_range_.getFlipY();

  CQFont *qfont = getQFont();

  int size = qfont->getSize();

  double wsize;

  pixelLengthToWindowLength(size, &wsize);

  QTransform old_trans = getQPainter()->worldTransform();

  QTransform trans;

  double scale = wsize/size;

  if (! flip_y)
    trans.scale(scale, -scale);
  else
    trans.scale(scale, scale);

  getQPainter()->setWorldTransform(trans, true);

  double x1 = rect.getXMin()/scale;
  double x2 = rect.getXMax()/scale;

  double y1 = -(rect.getYMin()/scale - wsize);
  double y2 = -(rect.getYMax()/scale - wsize);

  if (! flip_y) {
    y1 = -y1;
    y2 = -y2;
  }

  QString qstr(text.c_str());

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_  );

  QRectF qrect(std::min(x1, x2), std::min(y1, y2), fabs(x2 - x1), fabs(y2 - y1));

  int flags = Qt::AlignCenter;

  getQPainter()->drawText(qrect, flags, qstr);

  getQPainter()->setWorldTransform(old_trans);
}

void
CQRenderer2D::
fillArc(const CPoint2D &center, double xr, double yr,
        double angle1, double angle2)
{
  brush_->setStyle(Qt::SolidPattern);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  double dangle1 = CMathGen::RadToDeg(angle1);
  double dangle2 = CMathGen::RadToDeg(angle2);

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?
                     Qt::WindingFill : Qt::OddEvenFill);

  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  path.arcMoveTo(rect, dangle1);

  path.arcTo(rect, dangle1, dangle2 - dangle1);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);
}

void
CQRenderer2D::
fillImageArc(const CPoint2D &center, double xr, double yr,
             double angle1, double angle2, CImagePtr image)
{
  if (! image.isValid()) return;

  QImage &qimage = image.cast<CQImage>()->getQImage();

  brush_->setTextureImage(qimage);

  getQPainter()->setBrush(*brush_);
  getQPainter()->setPen  (*pen_);

  QPainterPath path;

  double dangle1 = CMathGen::RadToDeg(angle1);
  double dangle2 = CMathGen::RadToDeg(angle2);

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?
                     Qt::WindingFill : Qt::OddEvenFill);

  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  path.arcMoveTo(rect, dangle1);

  path.arcTo(rect, dangle1, dangle2 - dangle1);

  path.closeSubpath();

  getQPainter()->fillPath(path, *brush_);
}

void
CQRenderer2D::
fillGradientArc(const CPoint2D &center, double xr, double yr,
                double angle1, double angle2, CRefPtr<CGenGradient> gradient)
{
  CLinearGradient *lg;
  CRadialGradient *rg;

  if      ((lg = dynamic_cast<CLinearGradient*>(gradient.getPtr())) != 0) {
    QLinearGradient qlg = CQUtil::toQGradient(lg);

    getQPainter()->setBrush(QBrush(qlg));
  }
  else if ((rg = dynamic_cast<CRadialGradient*>(gradient.getPtr())) != 0) {
    QRadialGradient qrg = CQUtil::toQGradient(rg);

    getQPainter()->setBrush(QBrush(qrg));
  }

  QPainterPath path;

  double dangle1 = CMathGen::RadToDeg(angle1);
  double dangle2 = CMathGen::RadToDeg(angle2);

  path.setFillRule(getFillType() == FILL_TYPE_WINDING ?
                     Qt::WindingFill : Qt::OddEvenFill);

  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  path.arcMoveTo(rect, dangle1);

  path.arcTo(rect, dangle1, dangle2 - dangle1);

  path.closeSubpath();

  getQPainter()->fillPath(path, getQPainter()->brush());
}

void
CQRenderer2D::
fillText(const CPoint2D &point, const std::string &text)
{
  CQFont *qfont = getQFont();

  int size = qfont->getSize();

  double wsize;

  pixelLengthToWindowLength(size, &wsize);

  double scale = wsize/size;

  double x1 = point.x/scale;
  double y1 = -(point.y/scale - size);

  QTransform old_trans = getQPainter()->worldTransform();

  QTransform trans1, trans2, trans3, trans4;

  trans1.translate(point.x, point.y - wsize);
  trans2.rotate   (qfont->getAngle());
  trans3.translate(-point.x, -(point.y - wsize));
  trans4.scale    (scale, -scale);

  getQPainter()->setWorldTransform(trans1, true);
  getQPainter()->setWorldTransform(trans2, true);
  getQPainter()->setWorldTransform(trans3, true);
  getQPainter()->setWorldTransform(trans4, true);

  CQPath2D *path = getQPath();

  path->init();

  path->moveTo(CPoint2D(x1, y1));

  CFontPtr font;

  getFont(font);

  path->text(text, font);

  path->fill(getQPainter());

  getQPainter()->setWorldTransform(old_trans);

#if 0
  getQPainter()->setPen(*getQPen());

  QTransform transform = getQPainter()->worldTransform();

  if (hasTransform_) {
    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);

    point.y = -point.y;
  }

  getQPainter()->drawText(QPointF(point.x, point.y), text.c_str());

  if (hasTransform_)
    getQPainter()->setWorldTransform(transform);
#endif
}

void
CQRenderer2D::
setImage(CImagePtr image)
{
#if 0
  double xmin, ymin, xmax, ymax;

  getDataRange(&xmin, &ymin, &xmax, &ymax);

  CMatrix2D view_matrix = *getViewMatrix();

  resetDataRange();

  unsetViewMatrix();

  drawImage(CPoint2D(0, 0), image);

  setViewMatrix(view_matrix);

  setDataRange(xmin, ymin, xmax, ymax);
#endif

  getQPainter()->setViewTransformEnabled(false);

  QImage &qimage = image.cast<CQImage>()->getQImage();

  getQPainter()->drawImage(0, 0, qimage);

  getQPainter()->setViewTransformEnabled(true);
}

void
CQRenderer2D::
setAlphaImage(CImagePtr image)
{
#if 0
  double xmin, ymin, xmax, ymax;

  getDataRange(&xmin, &ymin, &xmax, &ymax);

  CMatrix2D view_matrix = *getViewMatrix();

  resetDataRange();

  unsetViewMatrix();

  drawAlphaImage(CPoint2D(0, 0), image);

  setViewMatrix(view_matrix);

  setDataRange(xmin, ymin, xmax, ymax);
#endif

  setImage(image);
}

void
CQRenderer2D::
drawImage(const CPoint2D &point, CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  QTransform transform = getQPainter()->worldTransform();

  if (hasTransform_) {
    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);

    getQPainter()->drawImage(QPointF(point.x, -point.y),
                             qimage.convertToFormat(QImage::Format_RGB32));

    getQPainter()->setWorldTransform(transform);
  }
  else
    getQPainter()->drawImage(QPointF(point.x, point.y),
                             qimage.convertToFormat(QImage::Format_RGB32));
}

void
CQRenderer2D::
drawAlphaImage(const CPoint2D &point, CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  QTransform transform = getQPainter()->worldTransform();

  if (hasTransform_) {
    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);

    getQPainter()->drawImage(QPointF(point.x, -point.y), qimage);

    getQPainter()->setWorldTransform(transform);
  }
  else
    getQPainter()->drawImage(QPointF(point.x, point.y), qimage);
}

void
CQRenderer2D::
drawImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  bool flip_y = display_range_.getFlipY();

  if (! flip_y) {
    QTransform transform = getQPainter()->worldTransform();

    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);

    getQPainter()->drawImage(QRectF(bbox.getXMin(), -bbox.getYMax(),
                                    bbox.getWidth(), bbox.getHeight()),
                             qimage.convertToFormat(QImage::Format_RGB32));

    getQPainter()->setWorldTransform(transform);
  }
  else
    getQPainter()->drawImage(QRectF(bbox.getXMin(), bbox.getYMin(),
                                    bbox.getWidth(), bbox.getHeight()),
                             qimage.convertToFormat(QImage::Format_RGB32));
}

void
CQRenderer2D::
drawAlphaImageInBox(const CBBox2D &bbox, CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  bool flip_y = display_range_.getFlipY();

  if (! flip_y) {
    QTransform transform = getQPainter()->worldTransform();

    QTransform transform1 = transform;

    transform1.scale(1, -1);

    getQPainter()->setWorldTransform(transform1);

    getQPainter()->drawImage(QRectF(bbox.getXMin(), -bbox.getYMax(),
                                    bbox.getWidth(), bbox.getHeight()),
                             qimage);

    getQPainter()->setWorldTransform(transform);
  }
  else
    getQPainter()->drawImage(QRectF(bbox.getXMin(), bbox.getYMin(),
                                    bbox.getWidth(), bbox.getHeight()),
                             qimage);
}

void
CQRenderer2D::
drawBezier(C3Bezier2D &bezier)
{
  getQPainter()->setPen(*pen_);

  QPainterPath path;

  const CPoint2D &p1 = bezier.getFirstPoint   ();
  const CPoint2D &p2 = bezier.getControlPoint1();
  const CPoint2D &p3 = bezier.getControlPoint2();
  const CPoint2D &p4 = bezier.getLastPoint    ();

  path.moveTo(QPointF(p1.x, p1.y));

  path.cubicTo(QPointF(p2.x, p2.y),
               QPointF(p3.x, p3.y),
               QPointF(p4.x, p4.y));

  getQPainter()->strokePath(path, getQPainter()->pen());
}

void
CQRenderer2D::
strokePath(QPainterPath *path)
{
  QPainterPathStroker stroker;

  stroker.setWidth      (getQPen()->widthF());
  stroker.setCapStyle   (getQPen()->capStyle());
  stroker.setJoinStyle  (getQPen()->joinStyle());
  stroker.setDashPattern(getQPen()->dashPattern());

  getQPainter()->fillPath(stroker.createStroke(*path), *brush_);
}

CPath2D *
CQRenderer2D::
createPath() const
{
  return new CQPath2D;
}

CRenderer2D *
CQRenderer2D::
createImageRenderer() const
{
  return new CQImageRenderer2D;
}

CQFont *
CQRenderer2D::
getQFont() const
{
  CFontPtr pfont;

  getFont(pfont);

  return pfont.cast<CQFont>();
}
