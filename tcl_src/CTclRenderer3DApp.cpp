#include "std_c++.h"
#include "CTclApp/CTclApp.h"
#include "CTclRenderer3D.h"

#include "CTclRenderer3D.k"

class CTclRenderer3DApp;

class CTclRenderer3DApp : public CTclApp {
 private:
  CTclRenderer3DCmd *cmd_;

 public:
  CTclRenderer3DApp(int argc, char **argv);
 ~CTclRenderer3DApp();

 protected:
  void addCommands();

  string getTclStr();
};

int
main(int argc, char **argv)
{
  CTclRenderer3DApp app(argc, argv);
}

CTclRenderer3DApp::
CTclRenderer3DApp(int argc, char **argv) :
 CTclApp(argc, argv), cmd_(NULL)
{
  init();
}

CTclRenderer3DApp::
~CTclRenderer3DApp()
{
  delete cmd_;
}

void
CTclRenderer3DApp::
addCommands()
{
  cmd_ = new CTclRenderer3DCmd(this);
}

string
CTclRenderer3DApp::
getTclStr()
{
  return CTclRenderer3D_TK;
}
