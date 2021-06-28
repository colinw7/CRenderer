#include <CGLRenderer3D.h>
#include <CGLApp.h>
#include <CFreeTypePixelRenderer.h>
#include <CMathGeom3D.h>
#include <CBezierToLine.h>
#include <CFreeType.h>
#include <CPath2D.h>
#include <CFreeTypePath2D.h>
#include <CGLTexture.h>
#include <GL/glu.h>
#include <GL/glut.h>

static void tessCombine
             (double c[3], void *d[4], float w[4], void **out);
static void tessError
             (int error);

class PathVistor : public CPath2DVisitor {
 private:
  CGLRenderer3D *renderer_;

 public:
  PathVistor(CGLRenderer3D *renderer) : renderer_(renderer) { }

 ~PathVistor() { }

  void moveTo(const CPoint2D &point);

  void lineTo(const CPoint2D &point);

  void bezier2To(const CPoint2D &point1, const CPoint2D &point2);

  void bezier3To(const CPoint2D &point1, const CPoint2D &point2, const CPoint2D &point3);

  void close();
};

CGLRenderer3D::
CGLRenderer3D(CGLWindow *window)
{
  renderer_ = new CGLPixelRenderer(window);
}

CGLRenderer3D::
~CGLRenderer3D()
{
  delete renderer_;
}

void
CGLRenderer3D::
updateSize()
{
  renderer_->updateSize(100, 100);
}

void
CGLRenderer3D::
clear(const CRGBA &bg)
{
  glClearColor(bg.getRed(), bg.getGreen(), bg.getBlue(), bg.getAlpha());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
CGLRenderer3D::
flush()
{
  renderer_->flush();
}

void
CGLRenderer3D::
setForeground(const CRGBA &fg)
{
  renderer_->setForeground(fg);
}

CFontPtr
CGLRenderer3D::
getFont() const
{
  CFontPtr font;

  renderer_->getFont(font);

  return font;
}

void
CGLRenderer3D::
setFont(CFontPtr font)
{
  renderer_->setFont(font);
}

void
CGLRenderer3D::
drawPoint(const CPoint3D &point)
{
  drawPoint(point.x, point.y, point.z);
}

void
CGLRenderer3D::
drawPoint(double x, double y, double z)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_POINTS);

  glVertex3d(x, y, z);

  glEnd();
}

void
CGLRenderer3D::
drawPoint(const CPoint3D &point, CGLTexture *texture)
{
  glEnable(GL_TEXTURE_2D);

  texture->bind();

  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnable(GL_POINT_SPRITE);

  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  //float sizes[2];

  //glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes);

  //if (sizes[1] > 100.0f)
  //  sizes[1] = 100.0f;

  //glPointSize(sizes[1]);
  glPointSize(8.0);

  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_POINTS);

  glVertex3d(point.x, point.y, point.z);

  glEnd();

  glDisable(GL_POINT_SPRITE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void
CGLRenderer3D::
drawLine(const CLine3D &line)
{
  const CPoint3D &p1 = line.start();
  const CPoint3D &p2 = line.end  ();

  drawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

void
CGLRenderer3D::
drawLine(double x1, double y1, double z1, double x2, double y2, double z2)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_LINE_STRIP);

  glVertex3d(x1, y1, z1);
  glVertex3d(x2, y2, z2);

  glEnd();
}

void
CGLRenderer3D::
fillXYRectangle(double x1, double y1, double x2, double y2, double z, CGLTexture *texture)
{
  if (texture) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture->getId());

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_QUADS);

  glNormal3d(0, 0, 1);

  if (texture) {
    glTexCoord2d(0.0, 0.0); glVertex3d(x1, y1, z);
    glTexCoord2d(1.0, 0.0); glVertex3d(x2, y1, z);
    glTexCoord2d(1.0, 1.0); glVertex3d(x2, y2, z);
    glTexCoord2d(0.0, 1.0); glVertex3d(x1, y2, z);
  }
  else {
    glVertex3d(x1, y1, z);
    glVertex3d(x2, y1, z);
    glVertex3d(x2, y2, z);
    glVertex3d(x1, y2, z);
  }

  glEnd();

  if (texture)
    glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
fillXZRectangle(double x1, double z1, double x2, double z2, double y, CGLTexture *texture)
{
  if (texture) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture->getId());

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
  else {
    CRGBA fg;

    renderer_->getForeground(fg);

    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());
  }

  glBegin(GL_QUADS);

  if (texture) {
    glTexCoord2d(0.0, 0.0); glVertex3d(x1, y, z1);
    glTexCoord2d(1.0, 0.0); glVertex3d(x2, y, z1);
    glTexCoord2d(1.0, 1.0); glVertex3d(x2, y, z2);
    glTexCoord2d(0.0, 1.0); glVertex3d(x1, y, z2);
  }
  else {
    glVertex3d(x1, y, z1);
    glVertex3d(x2, y, z1);
    glVertex3d(x2, y, z2);
    glVertex3d(x1, y, z2);
  }

  glEnd();

  if (texture)
    glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
fillYZRectangle(double y1, double z1, double y2, double z2, double x, CGLTexture *texture)
{
  if (texture) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture->getId());

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
  else {
    CRGBA fg;

    renderer_->getForeground(fg);

    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());
  }

  glBegin(GL_QUADS);

  if (texture) {
    glTexCoord2d(0.0, 0.0); glVertex3d(x, y1, z1);
    glTexCoord2d(1.0, 0.0); glVertex3d(x, y2, z1);
    glTexCoord2d(1.0, 1.0); glVertex3d(x, y2, z2);
    glTexCoord2d(0.0, 1.0); glVertex3d(x, y1, z2);
  }
  else {
    glVertex3d(x, y1, z1);
    glVertex3d(x, y2, z1);
    glVertex3d(x, y2, z2);
    glVertex3d(x, y1, z2);
  }

  glEnd();

  if (texture)
    glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
fillZYRectangle(double z1, double y1, double z2, double y2, double x, CGLTexture *texture)
{
  if (texture) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture->getId());

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
  else {
    CRGBA fg;

    renderer_->getForeground(fg);

    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());
  }

  glBegin(GL_QUADS);

  if (texture) {
    glTexCoord2d(0.0, 0.0); glVertex3d(x, y1, z1);
    glTexCoord2d(1.0, 0.0); glVertex3d(x, y1, z2);
    glTexCoord2d(1.0, 1.0); glVertex3d(x, y2, z2);
    glTexCoord2d(0.0, 1.0); glVertex3d(x, y2, z1);
  }
  else {
    glVertex3d(x, y1, z1);
    glVertex3d(x, y1, z2);
    glVertex3d(x, y2, z2);
    glVertex3d(x, y2, z1);
  }

  glEnd();

  if (texture)
    glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
fillTriangle(const CPoint3D &p1, const CPoint3D &p2, const CPoint3D &p3)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_TRIANGLES);

  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);

  glEnd();
}

void
CGLRenderer3D::
fillTriangleFan(const PointList &points)
{
  glBegin(GL_TRIANGLE_FAN);

  PointList::const_iterator p1, p2;

  for (p1 = points.begin(), p2 = points.end(); p1 != p2; ++p1) {
    const CGLPoint3D &point = *p1;

    const CPoint3D &vertex = point.vertex;

    glVertex3d(vertex.x, vertex.y, vertex.z);

    if (point.color.isValid()) {
      const CRGBA &color = point.color.getValue();

      glColor4f(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
    }

    if (point.texture.isValid()) {
      const CPoint2D &texture = point.texture.getValue();

      glTexCoord2d(texture.x, texture.y);
    }
  }

  glEnd();
}

void
CGLRenderer3D::
fillRectangle(const CPoint3D &p1, const CPoint2D &t1,
              const CPoint3D &p2, const CPoint2D &t2,
              const CPoint3D &p3, const CPoint2D &t3,
              const CPoint3D &p4, const CPoint2D &t4,
              CGLTexture *texture)
{
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture->getId());

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glBegin(GL_QUADS);

  glTexCoord2d(t1.x, t1.y); glVertex3d(p1.x, p1.y, p1.z);
  glTexCoord2d(t2.x, t2.y); glVertex3d(p2.x, p2.y, p2.z);
  glTexCoord2d(t3.x, t3.y); glVertex3d(p3.x, p3.y, p3.z);
  glTexCoord2d(t4.x, t4.y); glVertex3d(p4.x, p4.y, p4.z);

  glEnd();

  glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
fillPolygon(const std::vector<CPoint3D> &points)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_POLYGON);

  std::vector<CPoint3D>::const_iterator p1 = points.begin();
  std::vector<CPoint3D>::const_iterator p2 = points.end  ();

  for ( ; p1 != p2; ++p1) {
    const CPoint3D &point = *p1;

    glVertex3d(point.x, point.y, point.z);
  }

  glEnd();
}

void
CGLRenderer3D::
drawSphere(const CPoint3D &p, double r, CGLTexture *texture)
{
  GLUquadric *qobj = gluNewQuadric();

  gluQuadricTexture(qobj, GL_TRUE);

  if (texture) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture->getId());

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glPushMatrix();

  glTranslated(p.x, p.y, p.z);

  //glRotatef(270, 1, 0, 0);

  //glutSolidSphere(r, 50, 50);

  gluSphere(qobj, r, 50, 50);

  glPopMatrix();

  gluDeleteQuadric(qobj);

  if (texture)
    glDisable(GL_TEXTURE_2D);
}

void
CGLRenderer3D::
drawPlane(const CPlane3D &plane)
{
  GLdouble z[2];

  glGetDoublev(GL_DEPTH_RANGE, z);

  CPlane3D near(CPoint3D(0,0,z[0]), CVector3D(0,0,1));
  CPlane3D far (CPoint3D(0,0,z[1]), CVector3D(0,0,1));

  CLine3D nline, fline;

  plane.intersect(near, nline);
  plane.intersect(far , fline);

  drawLine(nline);
  drawLine(fline);
}

void
CGLRenderer3D::
drawText(double *x, double *y, const char *text)
{
  CFreeTypeMgrInst->setFont(getFont());

  CFreeTypePixelRenderer ftr(renderer_);

  if (! CFreeTypeMgrInst->drawString(&ftr, text, x, y))
    return;
}

void
CGLRenderer3D::
drawPathText(double *x, double *y, const char *text)
{
  CFreeTypeMgrInst->setFont(getFont());

  CPath2D path;

  CFreeTypePath2D ftr(&path);

  if (! CFreeTypeMgrInst->pathString(&ftr, text, x, y))
    return;

  tess_ = gluNewTess();

  gluTessCallback(tess_, GLU_TESS_BEGIN  , (void (*)()) glBegin    );
  gluTessCallback(tess_, GLU_TESS_END    , (void (*)()) glEnd      );
  gluTessCallback(tess_, GLU_TESS_VERTEX , (void (*)()) glVertex3dv);
  gluTessCallback(tess_, GLU_TESS_ERROR  , (void (*)()) tessError  );
  gluTessCallback(tess_, GLU_TESS_COMBINE, (void (*)()) tessCombine);

  gluTessProperty(tess_, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

  drawPath(path);

  gluDeleteTess(tess_);

  tess_ = NULL;
}

void
CGLRenderer3D::
drawPath(const CPath2D &path)
{
  PathVistor visitor(this);

  path.process(visitor);
}

void
CGLRenderer3D::
setOverlayPixelRange(int xmin, int ymin, int xmax, int ymax)
{
  overlay_range_.setPixelRange(xmin, ymin, xmax, ymax);
}

void
CGLRenderer3D::
setOverlayDataRange(double xmin, double ymin, double xmax, double ymax)
{
  overlay_range_.setWindowRange(xmin, ymin, xmax, ymax);
}

void
CGLRenderer3D::
setOverlayLayer(uint layer)
{
  overlay_layer_ = layer;
}

void
CGLRenderer3D::
setOverlayZ()
{
  renderer_->setZ(0.5 + 0.01*overlay_layer_);
}

void
CGLRenderer3D::
drawOverlayRectangle(double x1, double y1, double x2, double y2)
{
  int px1, py1, px2, py2;

  overlay_range_.windowToPixel(x1, y1, &px1, &py1);
  overlay_range_.windowToPixel(x2, y2, &px2, &py2);

  setOverlayZ();

  renderer_->drawClippedRectangle(CIBBox2D(px1, py1, px2, py2));
}

void
CGLRenderer3D::
fillOverlayRectangle(double x1, double y1, double x2, double y2)
{
  int px1, py1, px2, py2;

  overlay_range_.windowToPixel(x1, y1, &px1, &py1);
  overlay_range_.windowToPixel(x2, y2, &px2, &py2);

  setOverlayZ();

  renderer_->fillClippedRectangle(CIBBox2D(px1, py1, px2, py2));
}

void
CGLRenderer3D::
fillOverlayTriangle(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3)
{
  int px1, py1, px2, py2, px3, py3;

  overlay_range_.windowToPixel(p1.x, p1.y, &px1, &py1);
  overlay_range_.windowToPixel(p2.x, p2.y, &px2, &py2);
  overlay_range_.windowToPixel(p3.x, p3.y, &px3, &py3);

  setOverlayZ();

  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  double z = renderer_->getZ();

  glBegin(GL_TRIANGLES);

  glVertex3d(px1, py1, z);
  glVertex3d(px2, py2, z);
  glVertex3d(px3, py3, z);

  glEnd();
}

void
CGLRenderer3D::
fillOverlayTriangle(const CGLPoint2D &p1, const CGLPoint2D &p2, const CGLPoint2D &p3)
{
  int px1, py1, px2, py2, px3, py3;

  overlay_range_.windowToPixel(p1.vertex.x, p1.vertex.y, &px1, &py1);
  overlay_range_.windowToPixel(p2.vertex.x, p2.vertex.y, &px2, &py2);
  overlay_range_.windowToPixel(p3.vertex.x, p3.vertex.y, &px3, &py3);

  setOverlayZ();

  double z = renderer_->getZ();

  CRGBA fg;

  renderer_->getForeground(fg);

  glBegin(GL_TRIANGLES);

  if (p1.color.isValid()) {
    const CRGBA &fg1 = p1.color.getValue();

    glColor4f(fg1.getRed(), fg1.getGreen(), fg1.getBlue(), fg1.getAlpha());
  }
  else
    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  if (p1.texture.isValid()) {
    const CPoint2D &texture = p1.texture.getValue();

    glTexCoord2d(texture.x, texture.y);
  }

  glVertex3d(px1, py1, z);

  if (p2.color.isValid()) {
    const CRGBA &fg1 = p2.color.getValue();

    glColor4f(fg1.getRed(), fg1.getGreen(), fg1.getBlue(), fg1.getAlpha());
  }
  else
    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glVertex3d(px2, py2, z);

  if (p2.texture.isValid()) {
    const CPoint2D &texture = p2.texture.getValue();

    glTexCoord2d(texture.x, texture.y);
  }

  if (p3.color.isValid()) {
    const CRGBA &fg1 = p3.color.getValue();

    glColor4f(fg1.getRed(), fg1.getGreen(), fg1.getBlue(), fg1.getAlpha());
  }
  else
    glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  if (p3.texture.isValid()) {
    const CPoint2D &texture = p3.texture.getValue();

    glTexCoord2d(texture.x, texture.y);
  }

  glVertex3d(px3, py3, z);

  glEnd();
}

void
CGLRenderer3D::
drawOverlayLine(double x1, double y1, double x2, double y2)
{
  int px1, py1, px2, py2;

  overlay_range_.windowToPixel(x1, y1, &px1, &py1);
  overlay_range_.windowToPixel(x2, y2, &px2, &py2);

  setOverlayZ();

  renderer_->drawClippedLine(CIPoint2D(px1, py1), CIPoint2D(px2, py2));
}

void
CGLRenderer3D::
drawOverlayPoint(double x, double y)
{
  int px, py;

  overlay_range_.windowToPixel(x, y, &px, &py);

  setOverlayZ();

  renderer_->drawClippedPoint(CIPoint2D(px, py));
}

void
CGLRenderer3D::
drawOverlayText(double x, double y, const std::string &str)
{
#if 0
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  setOverlayZ();

  int px, py;

  overlay_range_.windowToPixel(x, y, &px, &py);

  glRasterPos3d(px, py, 1);

  uint len = str.size();

  for (uint i = 0; i < len; ++i)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);

  CGLAppInst->displayErrors();
#endif

  CImagePtr image = imageString(str);

  drawOverlayImage(x, y, image);
}

CImagePtr
CGLRenderer3D::
imageString(const std::string &str)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  CImagePtr image = CImageMgrInst->createImage(CImageNoSrc());

  CFreeTypeMgrInst->setFont(getFont());

  CFreeTypeMgrInst->imageString(image, str.c_str(), CRGBA(fg));

  return image;
}

void
CGLRenderer3D::
drawOverlayImage(double x, double y, CImagePtr image)
{
  if (image->hasColormap())
    return;

  setOverlayZ();

  int px, py;

  overlay_range_.windowToPixel(x, y, &px, &py);

  drawImage(CPoint3D(px, py, renderer_->getZ()), image);
}

void
CGLRenderer3D::
drawImage(const CPoint3D &point, CImagePtr image)
{
  glPixelZoom(1, -1);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glRasterPos3d(point.x, point.y, point.z);

  glDrawPixels(image->getWidth(), image->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->getData());
}

void
CGLRenderer3D::
initOverlay()
{
  glMatrixMode(GL_MODELVIEW ); glPushMatrix();
  glMatrixMode(GL_PROJECTION); glPushMatrix();

  //----

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  uint w = renderer_->getWidth ();
  uint h = renderer_->getHeight();

  glOrtho(0.0, w, h, 0.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
}

void
CGLRenderer3D::
termOverlay()
{
  glMatrixMode(GL_PROJECTION); glPopMatrix();
  glMatrixMode(GL_MODELVIEW ); glPopMatrix();
}

void
CGLRenderer3D::
drawCube(double xc, double yc, double zc, double s)
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glPushMatrix();

  glTranslatef(xc, yc, zc);

  glutSolidCube(s);

  glPopMatrix();
}

void
CGLRenderer3D::
pathMoveTo(const CPoint3D &point)
{
  pathPoints_.clear();

  pathPoints_.push_back(point);

  current_ = point;
}

void
CGLRenderer3D::
pathLineTo(const CPoint3D &point)
{
  pathPoints_.push_back(point);

  current_ = point;
}

void
CGLRenderer3D::
pathBezierTo(const CPoint3D &point1, const CPoint3D &point2)
{
  C2Bezier2D bezier(CPoint2D(current_.x, current_.y),
                    CPoint2D(point1  .x, point1  .y),
                    CPoint2D(point2  .x, point2  .y));

  CBezierToLine b_to_l;

  b_to_l.calc(bezier);

  uint num_points = b_to_l.getNumPoints();

  for (uint i = 0; i < num_points; ++i) {
    const CPoint2D &p = b_to_l.getPoint(i);

    CPoint3D p1(p.x, p.y, point1.z);

    pathPoints_.push_back(p1);
  }

  //glEnable(GL_MAP1_VERTEX_3);

  //double p[9];

  //p[0] = current_.x; p[1] = current_.y; p[2] = current_.z;
  //p[3] = point1  .x; p[4] = point1  .y; p[5] = point1  .z;
  //p[6] = point2  .x; p[7] = point2  .y; p[8] = point2  .z;

  //glMap1d(GL_MAP1_VERTEX_3, 0.0, 100.0, 3, 3, &p[0]);

  //for (int i = 0; i <= 100; ++i)
  //  glEvalCoord1d((double) i);

  //glDisable(GL_MAP1_VERTEX_3);

  current_ = point2;
}

void
CGLRenderer3D::
pathBezierTo(const CPoint3D &point1, const CPoint3D &point2, const CPoint3D &point3)
{
  C3Bezier2D bezier(CPoint2D(current_.x, current_.y),
                    CPoint2D(point1  .x, point1  .y),
                    CPoint2D(point2  .x, point2  .y),
                    CPoint2D(point3  .x, point3  .y));

  CBezierToLine b_to_l;

  b_to_l.calc(bezier);

  uint num_points = b_to_l.getNumPoints();

  for (uint i = 0; i < num_points; ++i) {
    const CPoint2D &p = b_to_l.getPoint(i);

    CPoint3D p1(p.x, p.y, point1.z);

    pathPoints_.push_back(p1);
  }

  //glEnable(GL_MAP1_VERTEX_3);

  //double p[4][3];

  //p[0][0] = current_.x; p[0][1] = current_.y; p[0][2] = current_.z;
  //p[1][0] = point1  .x; p[1][1] = point1  .y; p[1][2] = point1  .z;
  //p[2][0] = point2  .x; p[2][1] = point2  .y; p[2][2] = point2  .z;
  //p[3][0] = point3  .x; p[3][1] = point3  .y; p[3][2] = point3  .z;

  //glMap1d(GL_MAP1_VERTEX_3, 0.0, 100.0, 3, 4, &p[0][0]);

  //for (int i = 0; i <= 100; ++i)
  //  glEvalCoord1d((double) i);

  //glDisable(GL_MAP1_VERTEX_3);

  current_ = point3;
}

void
CGLRenderer3D::
pathClose()
{
  CRGBA fg;

  renderer_->getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  uint numPathPoints = pathPoints_.size();

  if (tess_) {
    gluTessBeginPolygon(tess_, NULL);

    gluTessBeginContour(tess_);

    for (uint i = 0; i < numPathPoints; ++i) {
      const CPoint3D &point = pathPoints_[i];

      gluTessVertex(tess_, (double *) &point.x, (void *) &point.x);
    }

    gluTessEndContour(tess_);

    gluTessEndPolygon(tess_);
  }
  else {
    glBegin(GL_POLYGON);

    for (uint i = 0; i < numPathPoints; ++i) {
      const CPoint3D &point = pathPoints_[i];

      glVertex3d(point.x, point.y, point.z);
    }

    glEnd();
  }
}

static void
tessCombine(double c[3], void *[4], float [4], void **out)
{
  double *vertex = (double *) malloc(3*sizeof(double));

  vertex[0] = c[0];
  vertex[1] = c[1];
  vertex[2] = c[2];

  *out = vertex;
}

static void
tessError(int error)
{
  std::cerr << (const char *) gluErrorString(error) << std::endl;
}

//-------

void
PathVistor::
moveTo(const CPoint2D &point)
{
  renderer_->pathMoveTo(CPoint3D(point.x, point.y, 0.0));
}

void
PathVistor::
lineTo(const CPoint2D &point)
{
  renderer_->pathLineTo(CPoint3D(point.x, point.y, 0.0));
}

void
PathVistor::
bezier2To(const CPoint2D &point1, const CPoint2D &point2)
{
  renderer_->pathBezierTo(CPoint3D(point1.x, point1.y, 0.0),
                          CPoint3D(point2.x, point2.y, 0.0));
}

void
PathVistor::
bezier3To(const CPoint2D &point1, const CPoint2D &point2, const CPoint2D &point3)
{
  renderer_->pathBezierTo(CPoint3D(point1.x, point1.y, 0.0),
                          CPoint3D(point2.x, point2.y, 0.0),
                          CPoint3D(point3.x, point3.y, 0.0));
}

void
PathVistor::
close()
{
  renderer_->pathClose();
}
