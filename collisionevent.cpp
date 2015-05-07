#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include "collisionevent.h"

CollisionEvent::CollisionEvent(int object,
                               double time,
                               glm::vec3 initial_coordinates,
                               glm::vec3 initial_axis,
                               float initial_angle,
                               glm::vec3 axis_of_rotation,
                               glm::vec3 velocity,
                               float angular_velocity) {
  object_ = object;
  time_ = time;
  initial_coordinates_ = initial_coordinates;
  initial_axis_ = initial_axis;
  initial_angle_ = initial_angle;

  axis_of_rotation_ = axis_of_rotation;
  velocity_ = velocity;
  angular_velocity_ = angular_velocity;
}

void CollisionEvent::setValues(int object,
                               double time,
                               glm::vec3 initial_coordinates,
                               glm::vec3 initial_axis,
                               float initial_angle,
                               glm::vec3 axis_of_rotation,
                               glm::vec3 velocity,
                               float angular_velocity) {
  object_ = object;
  time_ = time;
  initial_coordinates_ = initial_coordinates;
  initial_axis_ = initial_axis;
  initial_angle_ = initial_angle;

  axis_of_rotation_ = axis_of_rotation;
  velocity_ = velocity;
  angular_velocity_ = angular_velocity;
}

int CollisionEvent::object() const {
  return object_;
}

float CollisionEvent::time() const {
  return time_;
}

glm::vec3 const * CollisionEvent::initial_coordinates() const {
  return &initial_coordinates_;
}

glm::vec3 const * CollisionEvent::initial_axis() const {
  return &initial_axis_;
}

float CollisionEvent::initial_angle() const {
  return initial_angle_;
}

glm::vec3 const * CollisionEvent::axis_of_rotation() const {
  return &axis_of_rotation_;
}

glm::vec3 const * CollisionEvent::velocity() const {
  return &velocity_;
}

float CollisionEvent::angular_velocity() const {
  return angular_velocity_;
}
