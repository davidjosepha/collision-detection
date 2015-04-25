#ifndef CUBOID_H
#define CUBOID_H
#include "object.h"
#include <glm/glm.hpp>

class Cuboid : public Object {
  public:
    Cuboid(float x, float y, float z, float mass);

    virtual const glm::vec4 * verts() const { return verts_; }
    virtual const glm::highp_uvec3 * tris() const { return tris_; }
    virtual int numverts() const { return NUM_VERTS; }
    virtual int numtris() const { return NUM_TRIS; }

  private:
    const static int NUM_VERTS = 8;
    const static int NUM_TRIS = 12;
    glm::vec4 verts_[NUM_VERTS];
    glm::highp_uvec3 tris_[NUM_TRIS] = {
      { 0, 3, 1 },  { 0, 2, 3 },  { 4, 2, 0 },  { 4, 6, 2 },
      { 5, 6, 4 },  { 5, 7, 6 },  { 1, 7, 5 },  { 1, 3, 7 },
      { 0, 5, 4 },  { 0, 1, 5 },  { 2, 3, 7 },  { 2, 6, 7 }
    };

    void genverts(const float & x, const float & y, const float & z);
};

#endif
