#include "simulation.h"
#include <cstdio>
#include <vector>
#include "viewer.h"
#include "object.h"
#include "cuboid.h"
#include "motionengine.h"
#include "collisionevent.h"

using namespace std;

Simulation::Simulation() {
  Cuboid scube = Cuboid(1.0, 1.0, 1.0, 10.0);
  Cuboid lcube = Cuboid(2.0, 2.0, 2.0, 10.0);

  std::vector<Object*> objects = std::vector<Object*>();
  objects.push_back(&scube);
  objects.push_back(&lcube);

  Viewer viewer = Viewer(objects);
  viewer.initGlut(0, NULL);

  printf("hey");
}
