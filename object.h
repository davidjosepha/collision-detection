#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>

class Object {
  public:
    virtual const glm::vec4 * verts() const = 0;
    virtual const glm::highp_uvec3 * tris() const = 0;
    virtual int numverts() const = 0;
    virtual int numtris() const = 0;
    virtual int mass() const = 0;
    virtual float inertia(glm::vec3 const & axis) const = 0;
    virtual void normalToEdge(glm::vec3 const & point, glm::vec3 & normal) const = 0;
};

#endif
