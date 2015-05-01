#ifndef COLLISION_H
#define COLLISION_H
#include <glm/glm.hpp>
#include <vector>
#include "object.h"

class Collision {
  public:
    Collision();
    Collision(float time,
              glm::vec3 const & point,
              std::vector<Object*> const & objects,
              int obj_a,
              int obj_b,
              CollisionEvent const & icol_a,
              CollisionEvent const & icol_b);

    void generateCollisionEvents(CollisionEvent & fcol_a, CollisionEvent & fcol_b) const;
  private:
    void relativePoint(CollisionEvent const & col, glm::vec3 & rpoint) const;
    void velocityAtPoint(CollisionEvent const & col, glm::vec3 & velocity) const;
    float momentOfInertia(glm::vec3 const & axis, int obj) const;
    void impulse();
    float impulseParam();

    float time_;
    glm::vec3 * point_;
    std::vector<Object*> * objects_;
};

#endif
