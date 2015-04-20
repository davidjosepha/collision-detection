#ifndef CUBOID_H
#define CUBOID_H
#include "object.h"
#include <glm/glm.hpp>

class Cuboid : public Object {
  public:
    Cuboid(float x, float y, float z, float mass);

    virtual const glm::vec3 * verts() const { return verts_; }
    virtual const glm::highp_ivec3 * tris() const { return tris_; }
    virtual int numverts() const { return NUM_VERTS; }
    virtual int numtris() const { return NUM_TRIS; }

  private:
    const static int NUM_VERTS = 8;
    const static int NUM_TRIS = 12;
    glm::vec3 verts_[NUM_VERTS];
    glm::highp_ivec3 tris_[NUM_TRIS] = {
      { 0, 3, 1 },  { 0, 1, 5 },  { 0, 5, 3 },
      { 2, 1, 3 },  { 2, 7, 1 },  { 2, 3, 7 },
      { 4, 5, 1 },  { 4, 1, 7 },  { 4, 7, 5 },
      { 6, 3, 5 },  { 6, 7, 3 },  { 6, 5, 7 }
    };

    void genverts(const float & x, const float & y, const float & z);
};

#endif
