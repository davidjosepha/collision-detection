#include "collision.h"
#include <cstdio>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "collisionevent.h"
#include "object.h"
#define ELASTICITY 1.0

Collision::Collision() { }

Collision::Collision(std::vector<Object*> const & objects) {
  objects_ = &objects;

  //int obj_a_ = obj_a;
  //int obj_b_ = obj_b;
  //CollisionEvent const * icol_a_ = &icol_a;
  //CollisionEvent const * icol_b_ = &icol_b;
}

void Collision::generateCollisionEvents(float time,
                                        glm::vec3 const & point,
                                        int object_a,
                                        int object_b,
                                        CollisionEvent const & initial_collision_a,
                                        CollisionEvent const & initial_collision_b,
                                        CollisionEvent & final_collision_a,
                                        CollisionEvent & final_collision_b) const {
  glm::vec3 normal;
  normalToEdge(normal);

  glm::vec3 impact_velocity; // TODO

  float mass_a = object(object_a)->mass();
  glm::vec3 radius_a;
  radiusAtPoint(time, point, initial_collision_a, radius_a);
  float moment_of_inertia_a = momentOfInertia(object_a, *initial_collision_a.axis_of_rotation());

  float mass_b = object(object_b)->mass();
  glm::vec3 radius_b;
  radiusAtPoint(time, point, initial_collision_b, radius_b);
  float moment_of_inertia_b = momentOfInertia(object_b, *initial_collision_b.axis_of_rotation());

  float impulse_parameter
        = impulseParameter(ELASTICITY,
                           impact_velocity,
                           normal,
                           mass_a,
                           mass_b,
                           radius_a,
                           radius_b,
                           moment_of_inertia_a,
                           moment_of_inertia_b);

  linearVelocity(impulse_parameter,
                 normal,
                 mass_a,
                 initial_collision_a,
                 final_collision_a);
  angularVelocity(impulse_parameter,
                  normal,
                  radius_a,
                  moment_of_inertia_a,
                  initial_collision_a,
                  final_collision_a);

  linearVelocity(impulse_parameter,
                 normal,
                 mass_b,
                 initial_collision_b,
                 final_collision_b);
  angularVelocity(impulse_parameter,
                  normal,
                  radius_b,
                  moment_of_inertia_b,
                  initial_collision_b,
                  final_collision_b);
}

Object const * Collision::object(int object_id) const {
  return (*objects_)[object_id];
}

void Collision::linearVelocity(float impulse_parameter,
                               glm::vec3 const & normal,
                               float mass,
                               CollisionEvent const & initial_collision,
                               CollisionEvent & final_collision) const {
  //final_velocity = initial_velocity + impulse_param * normal / mass;
}

void Collision::angularVelocity(float impulse_parameter,
                                glm::vec3 const & normal,
                                glm::vec3 const & radius,
                                float moment_of_inertia,
                                CollisionEvent const & initial_collision,
                                CollisionEvent & final_collision) const {
//  final_rotational_velocity = initial_rotational_velocity
//                              + glm::cross(rpoint, impulse_param * normal) / inertia;
}

//void Collision::generateCollisionEvents(int obj_a,
//                                        int obj_b,
//                                        CollisionEvent const & icol_a,
//                                        CollisionEvent const & icol_b,
//                                        CollisionEvent & fcol_a,
//                                        CollisionEvent & fcol_b) const {
//  /////////////
//  fcol_a.setValues(obj_a,
//                   time_,
//                   (time_ - icol_a.time()) * *icol_a.velocity()
//                      + *icol_a.initial_coordinates(),
//                   *icol_a.initial_axis(),
//                   icol_a.initial_angle(),
//                   *icol_a.axis_of_rotation(),
//                   (float)(((int)time_ % 2 == 1) * 2 - 1) * *icol_a.velocity(),
//                   icol_a.angular_velocity());
//  return;
//  /////////////
//  
//  float inertia_a = momentOfInertia(*icol_a.axis_of_rotation(), obj_a);
//  float inertia_b = momentOfInertia(*icol_b.axis_of_rotation(), obj_b);
//
//  glm::vec3 velocity_a;
//  velocityAtPoint(icol_a, velocity_a);
//  glm::vec3 velocity_b;
//  velocityAtPoint(icol_b, velocity_b);
//
//  glm::vec3 rpoint_a;
//  relativePoint(icol_a, rpoint_a);
//  glm::vec3 rpoint_b;
//  relativePoint(icol_b, rpoint_b);
//
//  glm::vec3 normal;
//  normalToEdge(normal);
//
//  glm::vec3 impact_velocity = velocity_a - velocity_b;
//  float impulse_param = impulseParam(ELASTICITY,
//                                     impact_velocity,
//                                     normal,
//                                     (*objects_)[obj_a]->mass(),
//                                     (*objects_)[obj_b]->mass(),
//                                     rpoint_a,
//                                     rpoint_b,
//                                     inertia_a,
//                                     inertia_b);
//}

void Collision::normalToEdge(glm::vec3 & normal) const {
  // TODO stuff
}
  
void Collision::velocityAtPoint(float time, glm::vec3 const & point, CollisionEvent const & collision, glm::vec3 & velocity) const {
  glm::vec3 radius;
  radiusAtPoint(time, point, collision, radius);

  velocity = *collision.velocity() + collision.angular_velocity() * glm::cross(*collision.axis_of_rotation(), radius);
}

void Collision::radiusAtPoint(float time, glm::vec3 const & point, CollisionEvent const & collision, glm::vec3 & radius) const {
  float dtime = time - collision.time();
  radius = point - (*collision.initial_coordinates() + dtime * *collision.velocity());
}

float Collision::momentOfInertia(int object_id, glm::vec3 const & axis) const {
  return (*objects_)[object_id]->inertia(axis);
}

float Collision::impulseParameter(float elasticity,
                                  glm::vec3 const & impact_velocity,
                                  glm::vec3 const & normal,
                                  float mass_a,
                                  float mass_b,
                                  glm::vec3 const & radius_a,
                                  glm::vec3 const & radius_b,
                                  float moment_of_inertia_a,
                                  float moment_of_inertia_b) const {
  float numerator = -(1 + elasticity) * glm::dot(impact_velocity, normal);

  glm::vec3 rap_norm = glm::cross(radius_a, normal);
  glm::vec3 rbp_norm = glm::cross(radius_b, normal);
  float denominator = (1 / mass_a) + (1 / mass_b)
                      + (glm::dot(rap_norm, rap_norm) / moment_of_inertia_a)
                      + (glm::dot(rbp_norm, rbp_norm) / moment_of_inertia_b);

  return numerator / denominator;
}
