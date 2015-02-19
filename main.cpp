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
#include <gsl/gsl_vector.h>
#include "cuboid.h"
#include "controller.h"
#include "viewer.h"

int main(int argc, char * argv[]) {
  //float color[3] = {0.0, 1.0, 0.0};
  gsl_vector * center = gsl_vector_alloc(3);
  gsl_vector * velocity = gsl_vector_alloc(3);
  gsl_vector * axis = gsl_vector_alloc(3);

  gsl_vector_set(center, 0, 0.0);
  gsl_vector_set(center, 1, 0.0);
  gsl_vector_set(center, 2, 0.0);

  gsl_vector_set(velocity, 0, 0.0);
  gsl_vector_set(velocity, 1, 0.0);
  gsl_vector_set(velocity, 2, 0.0);

  gsl_vector_set(axis, 0, 0.0);
  gsl_vector_set(axis, 1, 1.0);
  gsl_vector_set(axis, 2, 0.0);
  float angle = 0.0;
  float rotation = 0.0;
  Cuboid cuboid = Cuboid(1.0,1.0,1.0, *center, 1.0, *axis, *velocity, angle, rotation);

  gsl_vector * impact = gsl_vector_alloc(3);
  gsl_vector * impact_velocity = gsl_vector_alloc(3);

  gsl_vector_set(impact, 0, 1.0);
  gsl_vector_set(impact, 1, 1.0);
  gsl_vector_set(impact, 2, 0.0);

  gsl_vector_set(impact_velocity, 0, -0.001);
  gsl_vector_set(impact_velocity, 1, -0.001);
  gsl_vector_set(impact_velocity, 2, 0.0);

  cuboid.impact(0.0, *impact, *impact_velocity); 
  Controller controller = Controller(cuboid);
  Viewer viewer = Viewer(cuboid, controller);
  viewer.initGlut(argc, argv);

  gsl_vector_free(center);
  gsl_vector_free(velocity);
  gsl_vector_free(axis);
  gsl_vector_free(impact);
  gsl_vector_free(impact_velocity);
  return 0;
}
