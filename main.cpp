#include <cstdlib>
#include <vector>
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
  Cuboid cube = Cuboid(1.0, 1.0, 1.0, 10.0);
  std::vector<Object*> objects = std::vector<Object*>();
  objects.push_back(&cube);

  Viewer viewer = Viewer(objects);
  viewer.initGlut(argc, argv);

  return 0;
}
