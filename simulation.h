#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>
#include "object.h"
#include "viewer.h"
#include "motionengine.h"
#include "dummyengine.h"

class Simulation {
  public:
    Simulation();
  private:
    std::vector<Object> objects;
    Viewer viewer;
    MotionEngine motionengine;
    DummyEngine dummyengine;
    // need to add dummy physics engine
};


#endif
