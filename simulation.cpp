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

  objects = std::vector<Object*>();
  objects.push_back(&scube); // i = 0
  objects.push_back(&lcube); // i = 1

  std::vector<CollisionEvent> states = std::vector<CollisionEvent>();
  CollisionEvent sevent = CollisionEvent(0, 0.0, glm::vec3(1.0f, 1.0f, 1.0f),
                                         glm::vec3(0.0f, 0.0f, 1.0f), 1.0f,
                                         glm::vec3(0.0f, 1.0f, 0.0f),
                                         glm::vec3(0.05f, 0.0f, 0.0f), 0.0f);
  states.push_back(sevent);

  motionengine = MotionEngine();

  dummyengine = DummyEngine(motionengine, states, objects);

  Viewer viewer = Viewer(dummyengine, objects.size());
  viewer.initGlut(0, NULL);
}
