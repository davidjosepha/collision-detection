#include "collision.h"
#include <cstdio>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "collisionevent.h"
#include "object.h"
#define ELASTICITY 1.0

Collision::Collision() { }

Collision::Collision(std::vector<Object*> const & objects) {
  objects_ = &objects;
}

void Collision::generateCollisionEvents(float time,
                                        glm::vec3 const & point,
                                        int object_a,
                                        int object_b,
                                        CollisionEvent const & initial_collision_a,
                                        CollisionEvent const & initial_collision_b,
                                        CollisionEvent & final_collision_a,
                                        CollisionEvent & final_collision_b) const {
  float dtime_a = time - initial_collision_a.time();
  float mass_a = object(object_a)->mass();
  glm::vec3 radius_a;
  radiusAtPoint(dtime_a, point, initial_collision_a, radius_a);
  float moment_of_inertia_a = momentOfInertia(object_a, *initial_collision_a.axis_of_rotation());

  float dtime_b = time - initial_collision_a.time();
  float mass_b = object(object_b)->mass();
  glm::vec3 radius_b;
  radiusAtPoint(dtime_b, point, initial_collision_b, radius_b);
  float moment_of_inertia_b = momentOfInertia(object_b, *initial_collision_b.axis_of_rotation());

  glm::vec3 velocity_a;
  velocityAtPoint(dtime_a, point, initial_collision_a, velocity_a);
  glm::vec3 velocity_b;
  velocityAtPoint(dtime_b, point, initial_collision_b, velocity_b);
  glm::vec3 impact_velocity = velocity_a - velocity_b;

  final_collision_a.setObjectId(object_a);
  final_collision_a.setTime(time);
  glm::vec3 coordinates_a;
  coordinatesAtTime(dtime_a, initial_collision_a, coordinates_a);
  final_collision_a.setInitialCoordinates(coordinates_a);

  final_collision_b.setObjectId(object_b);
  final_collision_b.setTime(time);
  glm::vec3 coordinates_b;
  coordinatesAtTime(dtime_b, initial_collision_b, coordinates_b);
  final_collision_b.setInitialCoordinates(coordinates_b);

  axisAngle(dtime_a,
            initial_collision_a.initial_angle(),
            *initial_collision_a.initial_axis(),
            initial_collision_a.angular_velocity(),
            *initial_collision_a.axis_of_rotation(),
            final_collision_a);

  axisAngle(dtime_b,
            initial_collision_b.initial_angle(),
            *initial_collision_b.initial_axis(),
            initial_collision_b.angular_velocity(),
            *initial_collision_b.axis_of_rotation(),
            final_collision_b);

  glm::vec3 normal;
  normalToEdge(point - coordinates_b, dtime_b, object_b, initial_collision_b, normal);

  float impulse_parameter = impulseParameter(ELASTICITY,
                                             impact_velocity,
                                             normal,
                                             mass_a,
                                             mass_b,
                                             radius_a,
                                             radius_b,
                                             moment_of_inertia_a,
                                             moment_of_inertia_b);

  angularVelocity(impulse_parameter,
                  normal,
                  radius_a,
                  moment_of_inertia_a,
                  initial_collision_a,
                  final_collision_a);
  linearVelocity(impulse_parameter,
                 normal,
                 mass_a,
                 initial_collision_a,
                 final_collision_a);

  angularVelocity(impulse_parameter,
                  normal,
                  radius_b,
                  moment_of_inertia_b,
                  initial_collision_b,
                  final_collision_b);
  linearVelocity(impulse_parameter,
                 normal,
                 mass_b,
                 initial_collision_b,
                 final_collision_b);
}

Object const * Collision::object(int object_id) const {
  return (*objects_)[object_id];
}

void Collision::axisAngle(float dtime,
                          float initial_angle,
                          glm::vec3 const & initial_axis,
                          float angular_velocity,
                          glm::vec3 const & axis_of_rotation,
                          CollisionEvent & final_collision) const {
  glm::mat4 irot = glm::rotate(initial_angle, initial_axis);
  glm::mat4 trot = glm::rotate(dtime * angular_velocity, axis_of_rotation);
  glm::mat4 rot = trot * irot;

  float final_angle;
  glm::vec3 final_axis;
  glm::axisAngle(rot, final_axis, final_angle);

  final_collision.setInitialAxis(final_axis);
  final_collision.setInitialAngle(final_angle);
}

void Collision::linearVelocity(float impulse_parameter,
                               glm::vec3 const & normal,
                               float mass,
                               CollisionEvent const & initial_collision,
                               CollisionEvent & final_collision) const {
  glm::vec3 linear_velocity = *initial_collision.velocity() + impulse_parameter * normal / mass;
  final_collision.setVelocity(linear_velocity);
}

void Collision::angularVelocity(float impulse_parameter,
                                glm::vec3 const & normal,
                                glm::vec3 const & radius,
                                float moment_of_inertia,
                                CollisionEvent const & initial_collision,
                                CollisionEvent & final_collision) const {
  // axis is a unit vector!!
  glm::vec3 initial_omega = *initial_collision.axis_of_rotation() * initial_collision.angular_velocity();
  glm::vec3 final_omega = initial_omega
                          + glm::cross(radius, impulse_parameter * normal) / moment_of_inertia;

  final_collision.setAxisOfRotation(glm::normalize(final_omega));
  final_collision.setAngularVelocity(glm::length(final_omega));
}

void Collision::normalToEdge(glm::vec3 const & relative_point,
                             float dtime,
                             int object_id,
                             CollisionEvent const & collision,
                             glm::vec3 & normal) const {
  glm::vec3 canonical_point = glm::rotate(
                                glm::rotate(relative_point,
                                            collision.initial_angle(),
                                            *collision.initial_axis()
                                ),
                                dtime * collision.angular_velocity(),
                                *collision.axis_of_rotation()
                              );
  object(object_id)->normalToEdge(canonical_point, normal);
}

void Collision::coordinatesAtTime(float dtime,
                                  CollisionEvent const & collision,
                                  glm::vec3 & coordinates) const {
  coordinates = *collision.initial_coordinates() + dtime * *collision.velocity();
}
  
void Collision::velocityAtPoint(float dtime,
                                glm::vec3 const & point,
                                CollisionEvent const & collision,
                                glm::vec3 & velocity) const {
  glm::vec3 radius;
  radiusAtPoint(dtime, point, collision, radius);

  velocity = *collision.velocity() + collision.angular_velocity() * glm::cross(*collision.axis_of_rotation(), radius);
}

void Collision::radiusAtPoint(float dtime,
                              glm::vec3 const & point,
                              CollisionEvent const & collision,
                              glm::vec3 & radius) const {
  glm::vec3 coordinates;
  coordinatesAtTime(dtime, collision, coordinates);
  radius = point - coordinates;
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
