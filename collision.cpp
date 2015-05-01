#include "collision.h"
#include <vector>
#include <glm/glm.hpp>
#include "collisionevent.h"
#include "object.h"
#define ELASTICITY 1.0

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
  CollisionEvent const * icol_a_ = &icol_a;
  CollisionEvent const * icol_b_ = &icol_b;
}

void Collision::generateCollisionEvents(int obj_a,
                                        int obj_b,
                                        CollisionEvent const & icol_a,
                                        CollisionEvent const & icol_b,
                                        CollisionEvent & fcol_a,
                                        CollisionEvent & fcol_b) const {
  
  float inertia_a;// = momentOfInertia(NULL, obj_a);
  float inertia_b;// = momentOfInertia(NULL, obj_b);

  glm::vec3 velocity_a;
  velocityAtPoint(icol_a, velocity_a);
  glm::vec3 velocity_b;
  velocityAtPoint(icol_b, velocity_b);

  glm::vec3 rpoint_a;
  relativePoint(icol_a, rpoint_a);
  glm::vec3 rpoint_b;
  relativePoint(icol_b, rpoint_b);

  glm::vec3 normal;
  normalToEdge(normal);

  glm::vec3 impact_velocity = velocity_a - velocity_b;
  float impulse_param = impulseParam(ELASTICITY,
                                     impact_velocity,
                                     normal,
                                     (*objects_)[obj_a]->mass(),
                                     (*objects_)[obj_b]->mass(),
                                     rpoint_a,
                                     rpoint_b,
                                     inertia_a,
                                     inertia_b);
}

void Collision::relativePoint(CollisionEvent const & col, glm::vec3 & rpoint) const {
  float dtime = time_ - col.time();
  rpoint = *point_ - (*col.initial_coordinates() + dtime * *col.velocity());
}

void Collision::velocityAtPoint(CollisionEvent const & col, glm::vec3 & velocity) const {
  glm::vec3 rpoint;
  relativePoint(col, rpoint);

  velocity = *col.velocity() + col.angular_velocity() * glm::cross(*col.axis_of_rotation(), rpoint);
}

void Collision::normalToEdge(glm::vec3 & normal) const {
  // do some...stuff
}

float Collision::momentOfInertia(glm::vec3 const & axis, int obj) const {
  // DO SHIT
}

void Collision::finalVelocity(glm::vec3 const & initial_velocity,
                   float impulse_param,
                   glm::vec3 const & normal,
                   float mass,
                   glm::vec3 & final_velocity) const {
  final_velocity = initial_velocity + impulse_param * normal / mass;
}

void Collision::finalAngularVelocity(glm::vec3 const & initial_rotational_velocity,
                          glm::vec3 const & rpoint,
                          float impulse_param,
                          glm::vec3 const & normal,
                          float inertia,
                          glm::vec3 & final_rotational_velocity) const {
  final_rotational_velocity = initial_rotational_velocity
                              + glm::cross(rpoint, impulse_param * normal) / inertia;
}

float Collision::impulseParam(float elasticity,
                              glm::vec3 const & impact_velocity,
                              glm::vec3 const & normal,
                              float mass_a,
                              float mass_b,
                              glm::vec3 const & rpoint_a,
                              glm::vec3 const & rpoint_b,
                              float inertia_a,
                              float inertia_b) const {
  float numerator = -(1 + elasticity) * glm::dot(impact_velocity, normal);

  glm::vec3 rap_norm = glm::cross(rpoint_a, normal);
  glm::vec3 rbp_norm = glm::cross(rpoint_b, normal);
  float denominator = (1 / mass_a) + (1 / mass_b)
                      + (glm::dot(rap_norm, rap_norm) / inertia_a)
                      + (glm::dot(rbp_norm, rbp_norm) / inertia_b);

  return numerator / denominator;
}
