#include <cstdlib>
#include <cmath>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/gtx/transform.hpp>
#include "collisionevent.h"
#include "motionengine.h"

MotionEngine::MotionEngine() { }

void MotionEngine::pose(CollisionEvent const & event, float time, glm::mat4 & pmat) {
  float dtime = time - event.time();

  glm::mat4 irot = irotate(event.initial_angle(), *(event.initial_axis()));
  glm::mat4 trot = trotate(dtime, event.angular_velocity(), *(event.axis_of_rotation()));
  glm::mat4 trans = translate(dtime, *(event.initial_coordinates()), *(event.velocity()));

  pmat = trans * trot * irot;
}

glm::mat4 MotionEngine::irotate(float angle, glm::vec3 const & axis) const {
  return glm::rotate(angle, axis);
}

glm::mat4 MotionEngine::trotate(float dtime, float angle, glm::vec3 const & axis) const {
  return glm::rotate(dtime * angle, axis);
}

glm::mat4 MotionEngine::translate(float dtime,
                                         glm::vec3 const & coordinates,
                                         glm::vec3 const & velocity) const {
  return glm::translate(coordinates + dtime * velocity);
}
