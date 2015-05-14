#include "cuboid.h"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>

Cuboid::Cuboid(float x, float y, float z, float mass) {
  genverts(x, y, z);
  genchunks(x, y, z);
  gensphere();
  mass_ = mass;
}

float Cuboid::inertia(glm::vec3 const & axis) const {

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
                z * (k + .5) / CUBES_PER_SIDE,
                0.0 };
      }
    }
  }
}

void Cuboid::gensphere() {
}

void Cuboid::inertia_at_axis(glm::vec3 const & axis) {
  glm::vec3 orient_axis = glm::cross(axis, glm::vec3(0.0, 1.0, 0.0));
  float orient_angle = glm::angle(axis, glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 orient_mat = glm::rotate(orient_angle, orient_axis);
}
