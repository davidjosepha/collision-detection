#include <cstdlib>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include "object.h"
#include "viewer.h"

int main(int argc, char * argv[]) {
  Viewer viewer = Viewer();
  viewer.initGlut(argc, argv);
  return 0;
}
