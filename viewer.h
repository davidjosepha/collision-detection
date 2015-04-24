#ifndef VIEWER_H
#define VIEWER_H
#include "dummyengine.h"

class Viewer {
  public:
    Viewer();
    Viewer(DummyEngine & dummyengine);
    static void initGlut(int argc, char * argv[]);

  private:
    static void populateGlBuffers(float time);
    static void display();
    static void reshape(int w, int h);

    static DummyEngine * dummyengine_;
    static float time_;
};

#endif
