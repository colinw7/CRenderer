#ifndef CQT_PATH_2D_H
#define CQT_PATH_2D_H

#include <CPath2D.h>

class QPainter;
class QPainterPath;

class CQPath2D : public CPath2D {
 public:
  // path visitor
  class PathVisitor {
   public:
    PathVisitor() { }

   ~PathVisitor() { }

    virtual void init() { }
    virtual void term() { }

    virtual void moveTo (const CPoint2D &p) = 0;
    virtual void lineTo (const CPoint2D &p) = 0;
    virtual void quadTo (const CPoint2D &p1, const CPoint2D &p2) = 0;
    virtual void curveTo(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3) = 0;

   public:
    int      i { -1 };
    int      n { 0 };
    CPoint2D lastP;
    CPoint2D nextP;
  };

  //---

  CQPath2D();

  CQPath2D(const CQPath2D &path);

  CQPath2D &operator=(const CQPath2D &path);

 ~CQPath2D();

  CQPath2D *dup() const override;

  void init() override;

  void ellipse(const CPoint2D &center, double xr, double yr) override;

  void text(const std::string &text, CFontPtr font) override;

  void roundedRectangle(const CPoint2D &point1, const CPoint2D &point2,
                        double rx, double ry) override;

  bool moveTo (const CPoint2D &point) override;
  bool rmoveTo(const CPoint2D &point) override;

  bool lineTo (const CPoint2D &point) override;
  bool rlineTo(const CPoint2D &point) override;

  void arc (const CPoint2D &point, double xr, double yr, double angle1, double angle2) override;
  void arcN(const CPoint2D &point, double xr, double yr, double angle1, double angle2) override;

  bool arcTo(const CPoint2D &point1, const CPoint2D &point2, double xr, double yr) override;

  bool bezier2To(const CPoint2D &point1, const CPoint2D &point2) override;
  bool bezier3To(const CPoint2D &point1, const CPoint2D &point2, const CPoint2D &point3) override;

  void close() override;

  void stroke(QPainter *painter);

  void setFillType(CFillType fill_type) override;

  void fill(QPainter *painter);
  void fillImage(QPainter *painter, CImagePtr image);
  void fillGradient(QPainter *painter, CRefPtr<CGenGradient> gradient);

  bool getCurrentPoint(CPoint2D &point) override;

  void bbox(CBBox2D &bbox) const override;

  void transform(const CMatrix2D &m) override;

  void visit(PathVisitor &visitor) const;

 private:
  using CPath2D::stroke;
  using CPath2D::fill;
  using CPath2D::fillImage;
  using CPath2D::fillGradient;

 private:
  QPainterPath *path_         { nullptr };
  bool          currentPoint_ { false };
};

#endif
