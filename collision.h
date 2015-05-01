#ifndef COLLISION_H
#define COLLISION_H
#include <glm/glm.hpp>
#include <vector>
#include "collisionevent.h"
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

    void generateCollisionEvents(int obj_a,
                                 int obj_b,
                                 CollisionEvent const & icol_a,
                                 CollisionEvent const & icol_b,
                                 CollisionEvent & fcol_a,
                                 CollisionEvent & fcol_b) const;
  private:
    void relativePoint(CollisionEvent const & col, glm::vec3 & rpoint) const;
    void velocityAtPoint(CollisionEvent const & col, glm::vec3 & velocity) const;
    void normalToEdge(glm::vec3 & normal) const;
    float momentOfInertia(glm::vec3 const & axis, int obj) const;
    void finalVelocity(glm::vec3 const & initial_velocity,
                       float impulse_param,
                       glm::vec3 const & normal,
                       float mass,
                       glm::vec3 & final_velocity) const;
    void finalAngularVelocity(glm::vec3 const & initial_rotational_velocity,
                              glm::vec3 const & rpoint,
                              float impulse_param,
                              glm::vec3 const & normal,
                              float inertia,
                              glm::vec3 & final_rotational_velocity) const;
    float impulseParam(float elasticity,
                       glm::vec3 const & impact_velocity,
                       glm::vec3 const & normal,
                       float mass_a,
                       float mass_b,
                       glm::vec3 const & rpoint_a,
                       glm::vec3 const & rpoint_b,
                       float inertia_a,
                       float inertia_b) const;

    float time_;
    glm::vec3 const * point_;
    std::vector<Object*> const * objects_;
};

#endif
