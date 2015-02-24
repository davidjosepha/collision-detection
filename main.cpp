#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "geom_3d.h"
#include "cuboid.h"
#include "controller.h"
#include "viewer.h"

int main(int argc, char * argv[]) {
  //float color[3] = {0.0, 1.0, 0.0};
  Vec center = Vec(0.0, 0.0, 0.0);
  Vec velocity = Vec(0.0, 0.0, 0.0);
  Vec axis = Vec(0.0, 1.0, 0.0);

  float angle = 0.0;
  float rotation = 0.0;
  Cuboid cuboid = Cuboid(1.0,1.0,1.0, center, 1.0, axis, velocity, angle, rotation);

  //Vec impact_position = Vec(1.0, 1.0, 0.0);
  //Vec impact_velocity = Vec(0.003, -0.003, 0.0);
  Vec impact_position = Vec(0.750, 0.333, 1.0);
  Vec impact_velocity = Vec(0.703, -0.403, -0.3);

  cuboid.impact(0.0, impact_position, impact_velocity); 

  Controller controller = Controller(cuboid);
  Viewer viewer = Viewer(cuboid, controller);
  viewer.initGlut(argc, argv);

  return 0;
}
