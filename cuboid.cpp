#include "cuboid.h"
#include <math.h>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>

#include <cstdio>

Cuboid::Cuboid(float x, float y, float z, float mass) {
  genverts(x, y, z);
  genchunks(x, y, z);
  gensphere();
  mass_ = mass;
}

float Cuboid::inertia(glm::vec3 const & axis) const {
  glm::vec3 uaxis = glm::normalize(axis);
  float min = 10;
  int closest;
  for (int i = 0; i < sizeof(sphere_verts_) / sizeof(glm::vec3); i++) {
    float dist = pow(sphere_verts_[i].x, 2)
                 + pow(sphere_verts_[i].y, 2)
                 + pow(sphere_verts_[i].z, 2);
    if (dist < min) {
      min = dist;
      closest = i;
    }
  }

  return sphere_[closest];
}

void Cuboid::genverts(float x, float y, float z) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        verts_[4*i + 2*j + k] = { x * (2.0*i - 1.0) / 2.0,
                                  y * (2.0*j - 1.0) / 2.0,
                                  z * (2.0*k - 1.0) / 2.0,
                                  1.0f };
      }
    }
  }
}

void Cuboid::genchunks(float x, float y, float z) {
  for (int i = 0; i < CUBES_PER_SIDE; i++) {
    for (int j = 0; j < CUBES_PER_SIDE; j++) {
      for (int k = 0; k < CUBES_PER_SIDE; k++) {
        chunks_[CUBES_PER_SIDE * CUBES_PER_SIDE * i + CUBES_PER_SIDE * j + k]
            = { x * (i + .5) / CUBES_PER_SIDE,
                y * (j + .5) / CUBES_PER_SIDE,
                z * (k + .5) / CUBES_PER_SIDE };
      }
    }
  }
}

void Cuboid::gensphere() {
  for (int i = 0; i < sizeof(sphere_verts_) / sizeof(glm::vec3); i++) {
    sphere_[i] = inertia_at_axis(sphere_verts_[i]);
  }
}

float Cuboid::inertia_at_axis(glm::vec3 const & axis) {
  glm::vec3 orient_axis = glm::cross(axis, glm::vec3(0.0, 0.0, 1.0));
  float orient_angle = glm::angle(axis, glm::vec3(0.0, 0.0, 1.0));
  glm::mat4 orient_mat = glm::rotate(orient_angle, orient_axis);

  int numpoints = sizeof(chunks_) / sizeof(glm::vec3);
  float distance_integral = 0;
  for (int i = 0; i < numpoints; i++) {
    glm::vec4 point_mass = glm::vec4(chunks_[i], 0.0f);
    point_mass = orient_mat * point_mass;
    distance_integral += pow(point_mass.x, 2) + pow(point_mass.y, 2);
  }

  return distance_integral * (mass_ / numpoints);
  
  // sum over (x^2 + y^2) * m
  // v = axis
  // x = point
  // || (x - (v_hat dot x) * v_hat ||^2
}
