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
#include "cuboid.h"
#include "controller.h"
#include "viewer.h"

#define _USE_MATH_DEFINES
#define M_PI 3.1415926535

int main(int argc, char * argv[]) {
  float center[3] = {0.0, 0.0, 0.0};
  float color[3] = {0.0, 1.0, 0.0};
  float position[3] = {0.0, 0.0, 0.0};
  float velocity[3] = {0.0, 0.0, 0.0};
  float axis[3] = {0.0, 1.0, 0.0};
  float rotation = 0.0;
  //Cuboid cuboid = Cuboid(1.0,1.0,1.0, center, color, 1.0, position, 0.0, 0.0, velocity, 0.0078125, 0.0078125);
  Cuboid cuboid = Cuboid(1.0,1.0,1.0, center, color, 1.0, position, axis, velocity, rotation);
  Controller controller = Controller(cuboid);
  Viewer viewer = Viewer(cuboid, controller);
  viewer.initGlut(argc, argv);

  return 0;
}
