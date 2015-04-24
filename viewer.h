#ifndef VIEWER_H
#define VIEWER_H
#include "dummyengine.h"

class Viewer {
  public:
    Viewer();
    Viewer(DummyEngine & dummyengine, int numobjects);
    static void initGlut(int argc, char * argv[]);

  private:
    static void model();
    static void display();
    static void reshape(int w, int h);

    static DummyEngine * dummyengine_;
    static int numobjects_;
};

#endif
