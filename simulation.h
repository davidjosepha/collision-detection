#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>
#include "object.h"
#include "viewer.h"
#include "motionengine.h"

class Simulation {
  public:
    Simulation();
  private:
    std::vector<Object> objects;
    Viewer viewer;
    MotionEngine motionengine;
    // need to add dummy physics engine
};


#endif
