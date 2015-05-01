#include "cuboid.h"

Cuboid::Cuboid(float x, float y, float z, float mass) {
  genverts(x, y, z);      
  mass_ = mass;
}

void Cuboid::genverts(const float & x, const float & y, const float & z) {
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
