#ifndef CQT_PATH_2D_H
#define CQT_PATH_2D_H

#include <CPath2D.h>

class QPainter;
class QPainterPath;

class CQPath2D : public CPath2D {
 private:
  QPainterPath *path_;
  bool          current_point_;

 public:
  CQPath2D();
 ~CQPath2D();

  void init();

  void ellipse(const CPoint2D &center, double xr, double yr);

  void text(const std::string &text, CFontPtr font);

  void roundedRectangle(const CPoint2D &point1, const CPoint2D &point2, double rx, double ry);

  bool moveTo (const CPoint2D &point);
  bool rmoveTo(const CPoint2D &point);

  bool lineTo (const CPoint2D &point);
  bool rlineTo(const CPoint2D &point);

  void arc (const CPoint2D &point, double xr, double yr, double angle1, double angle2);
  void arcN(const CPoint2D &point, double xr, double yr, double angle1, double angle2);

  bool arcTo(const CPoint2D &point1, const CPoint2D &point2, double xr, double yr);

  bool bezier2To(const CPoint2D &point1, const CPoint2D &point2);
  bool bezier3To(const CPoint2D &point1, const CPoint2D &point2, const CPoint2D &point3);

  void close();

  void stroke(QPainter *painter);

  void setFillType(CFillType fill_type);

  void fill(QPainter *painter);
  void fillImage(QPainter *painter, CImagePtr image);
  void fillGradient(QPainter *painter, CRefPtr<CGenGradient> gradient);

  bool getCurrentPoint(CPoint2D &point);

  void bbox(CBBox2D &bbox);
};

#endif
