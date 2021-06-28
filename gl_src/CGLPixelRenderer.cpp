#include <CGLPixelRenderer.h>
#include <CGLApp.h>
#include <CGLWindow.h>
#include <CGLUtil.h>
#include <CImageLib.h>
#include <CFileUtil.h>
#include <GL/glut.h>

CGLPixelRenderer::
CGLPixelRenderer(CGLWindow *window) :
 CPixelRenderer(), window_(window)
{
}

CGLPixelRenderer::
~CGLPixelRenderer()
{
}

CGLPixelRenderer *
CGLPixelRenderer::
dup() const
{
  return new CGLPixelRenderer(window_);
}

uint
CGLPixelRenderer::
getWidth() const
{
  GLint values[4];

  glGetIntegerv(GL_VIEWPORT, values);

  return values[2];
}

uint
CGLPixelRenderer::
getHeight() const
{
  GLint values[4];

  glGetIntegerv(GL_VIEWPORT, values);

  return values[3];
}

void
CGLPixelRenderer::
updateSize(int,int)
{
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  int w = getWidth ();
  int h = getHeight();

  if (! keep_aspect_)
    glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
  //glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
  else {
    int dx = 0;
    int dy = 0;

    if (w <= h)
      dy = (h - w)/2;
    else
      dx = (w - h)/2;

    glOrtho(-dx, w - dx, h - dy, -dy, -1.0, 1.0);
    //glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
  }

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  //GLfloat lightPos[] = { -50.0, 50.0, 100.0, 1.0 };

  //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void
CGLPixelRenderer::
startDoubleBuffer(bool /*clear*/)
{
}

void
CGLPixelRenderer::
endDoubleBuffer(bool /*copy*/)
{
  glutSwapBuffers();
}

void
CGLPixelRenderer::
copyDoubleBuffer()
{
}

void
CGLPixelRenderer::
setLineWidth(double line_width)
{
  glLineWidth(line_width);

  CPixelRenderer::setLineWidth(int(line_width));
}

double
CGLPixelRenderer::
getLineWidth()
{
  GLfloat width;

  glGetFloatv(GL_LINE_WIDTH, &width);

  return width;
}

void
CGLPixelRenderer::
getLineWidthDetails(double *min_width, double *max_width, double *dwidth)
{
  GLfloat widths[2];
  GLfloat step;

  glGetFloatv(GL_LINE_WIDTH_RANGE      , widths);
  glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &step);

  *min_width = widths[0];
  *max_width = widths[1];

  *dwidth = step;
}

void
CGLPixelRenderer::
setLineDash(const CILineDash &line_dash)
{
  int factor;

  ushort pattern = line_dash.getShortPattern(factor);

  glEnable(GL_LINE_STIPPLE);

  glLineStipple(factor, pattern);

  CPixelRenderer::setLineDash(line_dash);
}

void
CGLPixelRenderer::
setPointSize(double size)
{
  glEnable(GL_POINT_SMOOTH);

  glPointSize(size);
}

double
CGLPixelRenderer::
getPointSize()
{
  GLfloat size;

  glGetFloatv(GL_POINT_SIZE, &size);

  return size;
}

void
CGLPixelRenderer::
getPointSizeDetails(double *min_size, double *max_size, double *dsize)
{
  GLfloat sizes[2];
  GLfloat step;

  glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE      , sizes);
  glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &step);

  *min_size = sizes[0];
  *max_size = sizes[1];

  *dsize = step;
}

void
CGLPixelRenderer::
clear()
{
  glClearColor(bg_.getRed(), bg_.getGreen(), bg_.getBlue(), bg_.getAlpha());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
CGLPixelRenderer::
fill()
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_QUADS);

  int w = getWidth ();
  int h = getHeight();

  glVertex3d(0    , 0    , z_);
  glVertex3d(0    , h - 1, z_);
  glVertex3d(w - 1, h - 1, z_);
  glVertex3d(w - 1, 0    , z_);

  glEnd();
}

void
CGLPixelRenderer::
drawClippedPoint(const CIPoint2D &point)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_POINTS);

  glVertex3d(point.x, point.y, z_);

  glEnd();
}

void
CGLPixelRenderer::
drawClippedChar(const CIPoint2D &point, char c)
{
  std::string str;

  str += c;

  drawClippedString(point, str);
}

void
CGLPixelRenderer::
drawClippedString(const CIPoint2D &point, const std::string &str)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  int h = getCharAscent();

  glRasterPos3d(point.x, point.y + h, z_);

  uint len = str.size();

  CFontPtr font;

  getFont(font);

  void *fn = CGLUtil::fontToBitmap(font);

  for (uint i = 0; i < len; ++i)
    glutBitmapCharacter(fn, str[i]);
}

void
CGLPixelRenderer::
drawClippedLine(const CIPoint2D &point1, const CIPoint2D &point2)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_LINES);

  glVertex3d(point1.x, point1.y, z_);
  glVertex3d(point2.x, point2.y, z_);

  glEnd();
}

void
CGLPixelRenderer::
drawClippedRectangle(const CIBBox2D &bbox)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_LINE_LOOP);

  const CIPoint2D &ll = bbox.getLL();
  const CIPoint2D &ur = bbox.getUR();

  glVertex3d(ll.x, ll.y, z_);
  glVertex3d(ur.x, ll.y, z_);
  glVertex3d(ur.x, ur.y, z_);
  glVertex3d(ll.x, ur.y, z_);

  glEnd();
}

void
CGLPixelRenderer::
fillClippedRectangle(const CIBBox2D &bbox)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_QUADS);

  const CIPoint2D &ll = bbox.getLL();
  const CIPoint2D &ur = bbox.getUR();

  glVertex3d(ll.x, ll.y, z_);
  glVertex3d(ll.x, ur.y, z_);
  glVertex3d(ur.x, ur.y, z_);
  glVertex3d(ur.x, ll.y, z_);

  glEnd();
}

void
CGLPixelRenderer::
fillClippedTriangle(const CIPoint2D &point1, const CIPoint2D &point2, const CIPoint2D &point3)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_TRIANGLES);

  glVertex3d(point1.x, point1.y, z_);
  glVertex3d(point2.x, point2.y, z_);
  glVertex3d(point3.x, point3.y, z_);

  glEnd();
}

void
CGLPixelRenderer::
drawClippedPolygon(const IPointList &points)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_LINES);

  uint num_points = points.size();

  for (uint i = 0; i < num_points; ++i)
    glVertex3d(points[i].x, points[i].y, z_);

  glEnd();
}

void
CGLPixelRenderer::
fillClippedPolygon(const IPointList &points)
{
  CRGBA fg;

  getForeground(fg);

  glColor4f(fg.getRed(), fg.getGreen(), fg.getBlue(), fg.getAlpha());

  glBegin(GL_POLYGON);

  uint num_points = points.size();

  for (uint i = 0; i < num_points; ++i)
    glVertex3d(points[i].x, points[i].y, z_);

  glEnd();
}

void
CGLPixelRenderer::
drawClippedArc(const CIPoint2D &, int, int, double, double)
{
  assert(false);
}

void
CGLPixelRenderer::
fillClippedArc(const CIPoint2D &, int, int, double, double)
{
  assert(false);
}

void
CGLPixelRenderer::
drawClippedImage(const CIPoint2D &point, CImagePtr image)
{
  if (image->hasColormap())
    return;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glPixelZoom(img_scale_, -img_scale_);

  //glRasterPos3d(x, y + int(img_scale_*image->getHeight()), z_);
  glRasterPos3d(point.x, point.y, z_);

  glDrawPixels(image->getWidth(), image->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->getData());
}

void
CGLPixelRenderer::
drawClippedScaledImage(const CIPoint2D &point, CImagePtr image, double sx, double sy)
{
  if (image->hasColormap())
    return;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glPixelZoom(sx, -sy);

  //glRasterPos3d(x, y + int(scale*image->getHeight()), z_);
  glRasterPos3d(point.x, point.y, z_);

  glDrawPixels(image->getWidth(), image->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, image->getData());
}

void
CGLPixelRenderer::
drawClippedAlphaImage(const CIPoint2D &point, CImagePtr image)
{
  drawClippedImage(point, image);
}

int
CGLPixelRenderer::
getCharWidth()
{
  CFontPtr font;

  getFont(font);

  int w = CGLUtil::fontWidth(font);

  return w;
}

int
CGLPixelRenderer::
getCharAscent()
{
  CFontPtr font;

  getFont(font);

  int h = CGLUtil::fontAscent(font);

  return h;
}

int
CGLPixelRenderer::
getCharDescent()
{
  CFontPtr font;

  getFont(font);

  int h = CGLUtil::fontDescent(font);

  return h;
}

int
CGLPixelRenderer::
getCharHeight()
{
  CFontPtr font;

  getFont(font);

  int h = CGLUtil::fontHeight(font);

  return h;
}

int
CGLPixelRenderer::
getStringWidth(const std::string &str)
{
  return getCharWidth()*str.size();
}

void
CGLPixelRenderer::
getClip(int *, int *, int *, int *)
{
  assert(false);
}

void
CGLPixelRenderer::
flush()
{
  window_->redisplay();
}

void
CGLPixelRenderer::
saveImage(const std::string &fileName)
{
  GLint lastBuffer;

  glGetIntegerv(GL_READ_BUFFER, &lastBuffer);

  int w = getWidth ();
  int h = getHeight();

  uchar *data = new uchar [w*h*4];

  glPixelStorei(GL_PACK_ALIGNMENT  , 1);
  glPixelStorei(GL_PACK_ROW_LENGTH , 0);
  glPixelStorei(GL_PACK_SKIP_ROWS  , 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

  glReadBuffer(GL_FRONT);

  glReadPixels(0, 0, w, h, GL_BGRA, GL_UNSIGNED_BYTE, data);

  glReadBuffer(lastBuffer);

  CImagePtr image = CImageMgrInst->createImage(CImageNoSrc());

  if (! image.isValid())
    return;

  image->setDataSize(w, h);

  uint pos = 0;

  for (int y = h - 1; y >= 0; --y) {
    for (int x = 0; x < w; ++x) {
      double b = data[pos++]/255.0;
      double g = data[pos++]/255.0;
      double r = data[pos++]/255.0;
      double a = data[pos++]/255.0;

      CRGBA rgba(r, g, b, a);

      image->setRGBAPixel(x, y, rgba);
    }
  }

  CFileType type = CFileUtil::getImageTypeFromName(fileName);

  image->write(fileName, type);
}

void
CGLPixelRenderer::
errorPrint()
{
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "Error: %s\n", (char *) gluErrorString(error));
  }
}
