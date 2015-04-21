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
  Cuboid scube = Cuboid(1.0, 1.0, 1.0, 10.0);
  Cuboid lcube = Cuboid(2.0, 2.0, 2.0, 10.0);

  std::vector<Object*> objects = std::vector<Object*>();
  objects.push_back(&scube);
  objects.push_back(&lcube);

  Viewer viewer = Viewer(objects);
  viewer.initGlut(argc, argv);

  return 0;
}
