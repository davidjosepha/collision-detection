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
  Cuboid s_cube = Cuboid(1.0, 1.0, 1.0, 10.0);
  Cuboid l_cube = Cuboid(2.0, 2.0, 2.0, 10.0);

  objects = std::vector<Object*>();
  objects.push_back(&s_cube); // i = 0
  objects.push_back(&l_cube); // i = 1

  //std::vector<CollisionEvent> events = std::vector<CollisionEvent>();
  dummyengine = DummyEngine(motionengine, objects);

  Viewer viewer = Viewer(dummyengine);
  viewer.initGlut(0, NULL);
}
