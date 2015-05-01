#include "collision.h"
#include <vector>
#include <glm/glm.hpp>
#include "object.h"

Collision::Collision() { }

Collision::Collision(float time,
                     glm::vec3 const & point,
                     std::vector<Object*> const & objects,
                     int obj_a,
                     int obj_b,
                     CollisionEvent const & icol_a,
                     CollisionEvent const & icol_b) {
  time_ = time;
  point_ = &point;
  objects_ = &objects;

  int obj_a_ = obj_a;
  int obj_b_ = obj_b;
  CollisionEvent * icol_a_ = &icol_a;
  CollisionEvent * icol_b_ = &icol_b;
}

void Collision::generateCollisionEvents(CollisionEvent & fcol_a,
                                        CollisionEvent & fcol_b) const {
  
  float inert_a = momentOfInertia(NULL, obj_a_);
  float inert_b = momentOfInertia(NULL, obj_b_);
}

void Collision::relativePoint(CollisionEvent const & col, glm::vec3 & rpoint) const {
  float dtime = time_ - col->time();
  rpoint = point_ - (col->initial_coordinates() + dtime * col->velocity());
}

void Collision::velocityAtPoint(CollisionEvent const & col, glm::vec3 & velocity) const {
  glm::vec3 rpoint;
  relativePoint(col, rpoint);

  velocity = col->velocity() + col->angular_velocity() * glm::cross(col->axis_of_rotation(), rpoint);
}

float Collision::momentOfInertia(glm::vec3 const & axis, int obj) const {
  // do shit
}

void Collision::impulse() const {
  // do shit
}

float Collision::impulseParam() const {
  // calculate impulse parameter (j)
}
