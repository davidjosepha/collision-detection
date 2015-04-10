#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include "geom_3d.h"
#include "cuboid.h"
#include "collisionevent.h"

CollisionEvent::CollisionEvent(Cuboid &object, float time,
                        glm::vec3 initial_coordinates, glm::vec3 initial_axis,
                        float initial_angle,
                        glm::vec3 axis_of_rotation, glm::vec3 velocity,
                        float angular_velocity) {
  object_ = &object;
  time_ = time;
  initial_coordinates_ = initial_coordinates;
  initial_axis_ = initial_axis;
  initial_angle_ = initial_angle;

  axis_of_rotation_ = axis_of_rotation;
  velocity_ = velocity;
  angular_velocity_ = angular_velocity;
}

Cuboid & CollisionEvent::object() {
  return *object_;
}

float CollisionEvent::time() {
  return time_;
}

glm::vec3 & CollisionEvent::initial_coordinates() {
  return initial_coordinates_;
}

glm::vec3 & CollisionEvent::initial_axis() {
  return initial_axis_;
}

float CollisionEvent::initial_angle() {
  return initial_angle_;
}

glm::vec3 & CollisionEvent::axis_of_rotation() {
  return axis_of_rotation_;
}

glm::vec3 & CollisionEvent::velocity() {
  return velocity_;
}

float CollisionEvent::angular_velocity() {
  return angular_velocity_;
}
