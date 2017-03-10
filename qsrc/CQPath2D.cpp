#include <CQPath2D.h>
#include <CQFont.h>
#include <CQUtil.h>
#include <CQImage.h>
#include <CLineList2D.h>
#include <CLinearGradient.h>
#include <CRadialGradient.h>
#include <QPainter>
#include <QPainterPath>

CQPath2D::
CQPath2D() :
 CPath2D(), current_point_(false)
{
  path_ = new QPainterPath;
}

CQPath2D::
~CQPath2D()
{
}

void
CQPath2D::
init()
{
  path_ = new QPainterPath;

  current_point_ = false;
}

bool
CQPath2D::
moveTo(const CPoint2D &point)
{
  path_->moveTo(point.x, point.y);

  current_point_ = true;

  return true;
}

bool
CQPath2D::
rmoveTo(const CPoint2D &point)
{
  CPoint2D current;

  if (getCurrentPoint(current))
    moveTo(point + current);
  else
    moveTo(point);

  return true;
}

bool
CQPath2D::
lineTo(const CPoint2D &point)
{
  if (! current_point_)
    path_->moveTo(point.x, point.y);
  else
    path_->lineTo(point.x, point.y);

  current_point_ = true;

  return true;
}

bool
CQPath2D::
rlineTo(const CPoint2D &point)
{
  if (! current_point_)
    return rmoveTo(point);

  CPoint2D current;

  if (! getCurrentPoint(current))
    return false;

  lineTo(point + current);

  return true;
}

void
CQPath2D::
ellipse(const CPoint2D &center, double xr, double yr)
{
  path_->addEllipse(QRectF(center.x - xr, center.y - yr, 2*xr, 2*yr));

  current_point_ = true;
}

void
CQPath2D::
text(const std::string &text, CFontPtr font)
{
  CQFont *qfont = font.cast<CQFont>();

  CPoint2D point;

  getCurrentPoint(point);

  path_->addText(QPointF(point.x, point.y), qfont->getQFont(), text.c_str());

  current_point_ = true;
}

void
CQPath2D::
roundedRectangle(const CPoint2D &point1, const CPoint2D &point2, double rx, double ry)
{
  path_->addRoundedRect(QRectF(point1.x, point1.y, point2.x - point1.x, point2.y - point1.y), rx, ry);

  current_point_ = true;
}

void
CQPath2D::
arc(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  if (! current_point_)
    path_->moveTo(0, 0);

  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  angle1 = CMathGen::RadToDeg(angle1);
  angle2 = CMathGen::RadToDeg(angle2);

  path_->arcTo(rect, angle1, angle2 - angle1);

  current_point_ = true;
}

void
CQPath2D::
arcN(const CPoint2D &center, double xr, double yr, double angle1, double angle2)
{
  if (! current_point_)
    path_->moveTo(0, 0);

  QRectF rect(center.x - xr, center.y - yr, 2*xr, 2*yr);

  angle1 = CMathGen::RadToDeg(angle1);
  angle2 = CMathGen::RadToDeg(angle2);

  path_->arcTo(rect, angle2, angle1 - angle2);

  current_point_ = true;
}

bool
CQPath2D::
arcTo(const CPoint2D &point1, const CPoint2D &point2, double xr, double yr)
{
  if (! current_point_)
    path_->moveTo(point1.x, point1.y);

  QPointF pos = path_->currentPosition();

  double xc, yc, xt1, yt1, xt2, yt2;

  double x[3] = { pos.x(), point1.x, point2.x };
  double y[3] = { pos.y(), point1.y, point2.y };

  CLineList2D path(x, y, 3);

  if (! path.arcThrough(xr, yr, &xc, &yc, &xt1, &yt1, &xt2, &yt2))
    return false;

  lineTo(CPoint2D(xt1, yt1));

  double theta, delta;

  CMathGeom2D::ConvertFromSVGArc(xt1, yt1, xt2, yt2, 0.0, xr, yr, 0, 0, &xc, &yc, &theta, &delta);

  QRectF rect(xc - xr, yc - yr, 2*xr, 2*yr);

  path_->arcTo(rect, theta, delta);

  current_point_ = true;

  return true;
}

bool
CQPath2D::
bezier2To(const CPoint2D &point2, const CPoint2D &point3)
{
  if (! current_point_)
    path_->moveTo(0, 0);

  path_->quadTo(QPointF(point2.x, point2.y), QPointF(point3.x, point3.y));

  current_point_ = true;

  return true;
}

bool
CQPath2D::
bezier3To(const CPoint2D &point2, const CPoint2D &point3, const CPoint2D &point4)
{
  if (! current_point_)
    path_->moveTo(0, 0);

  path_->cubicTo(QPointF(point2.x, point2.y), QPointF(point3.x, point3.y), QPointF(point4.x, point4.y));

  current_point_ = true;

  return true;
}

void
CQPath2D::
close()
{
  path_->closeSubpath();

  current_point_ = false;
}

void
CQPath2D::
stroke(QPainter *painter)
{
  painter->strokePath(*path_, painter->pen());
}

void
CQPath2D::
setFillType(CFillType fill_type)
{
  CPath2D::setFillType(fill_type);

  if (fill_type_ == FILL_TYPE_EVEN_ODD)
    path_->setFillRule(Qt::OddEvenFill);
  else
    path_->setFillRule(Qt::WindingFill);
}

void
CQPath2D::
fill(QPainter *painter)
{
  painter->fillPath(*path_, painter->brush());
}

void
CQPath2D::
fillImage(QPainter *painter, CImagePtr image)
{
  QBrush brush;

  QImage &qimage = image.cast<CQImage>()->getQImage();

  brush.setTextureImage(qimage);

  painter->fillPath(*path_, brush);
}

void
CQPath2D::
fillGradient(QPainter *painter, CRefPtr<CGenGradient> gradient)
{
  QBrush brush;

  CLinearGradient *lg;
  CRadialGradient *rg;

  if      ((lg = dynamic_cast<CLinearGradient*>(gradient.getPtr())) != NULL) {
    QLinearGradient qlg = CQUtil::toQGradient(lg);

    brush = QBrush(qlg);
  }
  else if ((rg = dynamic_cast<CRadialGradient*>(gradient.getPtr())) != NULL) {
    QRadialGradient qrg = CQUtil::toQGradient(rg);

    brush = QBrush(qrg);
  }

  painter->fillPath(*path_, brush);
}

bool
CQPath2D::
getCurrentPoint(CPoint2D &point)
{
  if (! current_point_) {
    point = CPoint2D(0, 0);

    return false;
  }

  QPointF p = path_->currentPosition();

  point = CPoint2D(p.x(), p.y());

  return true;
}

void
CQPath2D::
bbox(CBBox2D &bbox)
{
  QRectF rect = path_->boundingRect();

  bbox = CBBox2D(rect.bottomLeft().x(), rect.bottomLeft().y(),
                 rect.topRight  ().x(), rect.topRight  ().y());
}
