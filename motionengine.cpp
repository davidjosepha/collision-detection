#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include "collisionevent.h"
#include "motionengine.h"

MotionEngine::MotionEngine() { }

void MotionEngine::pose(CollisionEvent & event, float time, glm::mat4 & pmat) {
  float dtime = time - event.time();

  glm::mat4 irot = irotate(event.initial_angle(), event.initial_axis());
  glm::mat4 trot = trotate(dtime, event.initial_angle(), event.axis_of_rotation());
  glm::mat4 trans = translate(dtime, event.initial_coordinates(), event.velocity());

  pmat = trans * trot * irot;
}

glm::mat4 MotionEngine::irotate(float angle, glm::vec3 & axis) {
  return glm::rotate(angle, axis);
}

glm::mat4 MotionEngine::trotate(float dtime, float angle, glm::vec3 & axis) {
  return glm::rotate(angle, axis);
}

glm::mat4 MotionEngine::translate(float dtime, glm::vec3 & coordinates, glm::vec3 & velocity) {
  return glm::translate(coordinates + dtime * velocity);
}
