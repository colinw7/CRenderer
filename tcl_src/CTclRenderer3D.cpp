#include <CTclRenderer3D.h>
#include <CTclCanvas.h>

#include <CRenderer3DObject.h>
#include <CXLibRenderer3D.h>
#include <CMathMacros.h>
#include <std_Xt.h>

class CTclRenderer3D : public CTclCanvas {
 public:
  CTclRenderer3D(CTclApp *app);
 ~CTclRenderer3D();

  bool instanceCommand(int argc, const char **argv);
  bool instanceConfigure(int argc, const char **argv);

  void draw();

  void mousePress();
  void mouseMotion();
  void mouseRelease();
  void keyPress() { }
  void keyRelease() { }

 private:
  void initRenderer();
  void process();

 private:
  CXLibRenderer3D*   renderer_ { nullptr };
  CRenderer3DObject* object_   { nullptr };
  double             x_angle_  { 0.0 };
  double             y_angle_  { 0.0 };
  double             z_angle_  { 0.0 };
  CMatrix3D          matrix_;
};

//---

CTclRenderer3DCmd::
CTclRenderer3DCmd(CTclApp *app) :
 CTclAppCanvasCmd(app)
{
  init();
}

CTclRenderer3DCmd::
~CTclRenderer3DCmd()
{
  delete renderer_;
}

CTclAppCanvas *
CTclRenderer3DCmd::
createInstance(CTclApp *app)
{
  renderer_ = new CTclRenderer3D(app);

  return (CTclAppCanvas *) renderer_->getCanvasWidget();
}

CTclRenderer3D::
CTclRenderer3D(CTclApp *app) :
 CTclCanvas(app), renderer_(NULL), object_(NULL)
{
  x_angle_ = y_angle_ = z_angle_ = 0.0;
}

CTclRenderer3D::
~CTclRenderer3D()
{
  delete renderer_;
}

bool
CTclRenderer3D::
instanceCommand(int argc, const char **argv)
{
  if      (argc > 0 && strcmp(argv[0], "zoom") == 0) {
    CRenderer3DCamera &camera = renderer_->getCamera();

    const CRenderer3DCameraData &camera_data = camera.getData();

    CPoint3D position = camera_data.position;

    if      (argc > 1 && strcmp(argv[1], "in") == 0)
      position.z += 4;
    else if (argc > 1 && strcmp(argv[1], "out") == 0)
      position.z -= 4;
    else
      return false;

    camera.setPosition(position);

    camera.build();
  }
  else if (argc > 0 && strcmp(argv[0], "setmode") == 0) {
    if      (argc > 1 && strcmp(argv[1], "wireframe") == 0)
      object_->setDrawType(CDRAW_TYPE_3D_WIREFRAME);
    else if (argc > 1 && strcmp(argv[1], "solid") == 0)
      object_->setDrawType(CDRAW_TYPE_3D_SOLID);
    else
      return false;
  }
  else if (argc > 0 && strcmp(argv[0], "rotate") == 0) {
    if      (argc > 1 && strcmp(argv[1], "x") == 0)
      ++x_angle_;
    else if (argc > 1 && strcmp(argv[1], "y") == 0)
      ++y_angle_;
    else if (argc > 1 && strcmp(argv[1], "z") == 0)
      ++z_angle_;
    else
      return false;
  }
  else if (argc > 0 && strcmp(argv[0], "step") == 0) {
    if      (argc > 1 && strcmp(argv[1], "add") == 0) {
      if      (argc > 2 && strcmp(argv[2], "cull") == 0)
        renderer_->addStep(CRenderer3D::CULL_STEP);
      else if (argc > 2 && strcmp(argv[2], "backframe_removal") == 0)
        renderer_->addStep(CRenderer3D::BACKFACE_REMOVAL_STEP);
      else if (argc > 2 && strcmp(argv[2], "light") == 0)
        renderer_->addStep(CRenderer3D::LIGHT_STEP);
      else if (argc > 2 && strcmp(argv[2], "sort") == 0)
        renderer_->addStep(CRenderer3D::SORT_STEP);
      else if (argc > 2 && strcmp(argv[2], "clip") == 0)
        renderer_->addStep(CRenderer3D::CLIP_STEP);
      else
        return false;
    }
    else if (argc > 1 && strcmp(argv[1], "remove") == 0) {
      if      (argc > 2 && strcmp(argv[2], "cull") == 0)
        renderer_->removeStep(CRenderer3D::CULL_STEP);
      else if (argc > 2 && strcmp(argv[2], "backframe_removal") == 0)
        renderer_->removeStep(CRenderer3D::BACKFACE_REMOVAL_STEP);
      else if (argc > 2 && strcmp(argv[2], "light") == 0)
        renderer_->removeStep(CRenderer3D::LIGHT_STEP);
      else if (argc > 2 && strcmp(argv[2], "sort") == 0)
        renderer_->removeStep(CRenderer3D::SORT_STEP);
      else if (argc > 2 && strcmp(argv[2], "clip") == 0)
        renderer_->removeStep(CRenderer3D::CLIP_STEP);
      else
        return false;
    }
    else
      return false;
  }
  else if (argc > 0 && strcmp(argv[0], "update") == 0) {
    draw();
  }
  else
    return false;

  return true;
}

bool
CTclRenderer3D::
instanceConfigure(int, const char **)
{
  return false;
}

void
CTclRenderer3D::
draw()
{
  CXGraphics *graphics = getCXGraphics();

  if (graphics == NULL)
    return;

  if (renderer_ == NULL)
    initRenderer();

  process();

  renderer_->redraw();
}

void
CTclRenderer3D::
initRenderer()
{
  renderer_ = new CXLibRenderer3D(getCXGraphics());

  object_ = renderer_->loadCOB("cube.cob");

  object_->setDrawType(CDRAW_TYPE_3D_WIREFRAME);

  object_->setOrigin(CPoint3D(0, 0, 100));

  CPoint3D  cam_pos(0, 0, 0);
  CVector3D cam_dir(0, 0, 0);

  renderer_->setEulerCamera(cam_pos, cam_dir, 50, 500, 90,
                            getWidth(), getHeight());
}

void
CTclRenderer3D::
process()
{
  if (object_ != NULL) {
    matrix_.setXYZRotation(DEG_TO_RAD(x_angle_),
                           DEG_TO_RAD(y_angle_),
                           DEG_TO_RAD(z_angle_));

    object_->resetFlags();

    object_->modelToWorld(matrix_);

    renderer_->clearObjects();

    renderer_->addObject(object_);
  }
}

void
CTclRenderer3D::
mousePress()
{
}

void
CTclRenderer3D::
mouseMotion()
{
}

void
CTclRenderer3D::
mouseRelease()
{
}
