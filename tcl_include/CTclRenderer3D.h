#ifndef CTCL_RENDERER_3D_H
#define CTCL_RENDERER_3D_H

#include <CTclAppCanvas.h>

class CTclRenderer3D;

class CTclRenderer3DCmd : public CTclAppCanvasCmd {
 public:
  CTclRenderer3DCmd(CTclApp *app);
 ~CTclRenderer3DCmd();

  std::string getCommandName () const { return "CTclRenderer3D"; }
  std::string getCommandClass() const { return "CTclRenderer3D"; }

  CTclAppCanvas *createInstance(CTclApp *app);

 private:
  CTclRenderer3D* renderer_ { nullptr };
};

#endif
