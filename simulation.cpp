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

  std::vector<CollisionEvent> events = std::vector<CollisionEvent>();
  CollisionEvent s_event = CollisionEvent(0,                            // object id
                                          0.0,                          // time
                                          glm::vec3(1.0f, 1.0f, 1.0f),  // initial_coordinates
                                          glm::vec3(0.0f, 0.0f, 1.0f),  // initial_axis
                                          1.0f,                         // initial_angle
                                          glm::vec3(0.0f, 1.0f, 0.0f),  // axis_of_rotation
                                          glm::vec3(0.05f, 0.0f, 0.0f), // velocity
                                          1.0f);                        // angular_velocity
  events.push_back(s_event);

  motionengine = MotionEngine();

  dummyengine = DummyEngine(motionengine, events, objects);

  Viewer viewer = Viewer(dummyengine);
  viewer.initGlut(0, NULL);
}
