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
#include "simulation.h"

int main(int argc, char * argv[]) {
  Simulation sim = Simulation();

  return 0;
}
