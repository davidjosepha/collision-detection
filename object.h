#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>

class Object {
  public:
    virtual const glm::vec3 * verts() const = 0;
    virtual const glm::highp_uvec3 * tris() const = 0;
    virtual int numverts() const = 0;
    virtual int numtris() const = 0;
};

#endif
