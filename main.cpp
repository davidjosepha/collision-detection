#include <cstdlib>
#include <iostream>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "object.h"
#include "cuboid.h"
#include "viewer.h"

int main(int argc, char * argv[]) {
  Cuboid cube = Cuboid(5.0, 5.0, 5.0, 10.0);
  Viewer viewer = Viewer(cube);
  viewer.initGlut(argc, argv);

  return 0;
}
