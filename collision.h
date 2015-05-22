#ifndef COLLISION_H
#define COLLISION_H
#include <glm/glm.hpp>
#include <vector>
#include "collisionevent.h"
#include "object.h"

class Collision {
  public:
    Collision();
    Collision(std::vector<Object*> const & objects);

    void generateCollisionEvents(float time,
                                 glm::vec3 const & point,
                                 int object_a,
                                 int object_b,
                                 CollisionEvent const & initial_collision_a,
                                 CollisionEvent const & initial_collision_b,
                                 CollisionEvent & final_collision_a,
                                 CollisionEvent & final_collision_b) const;

  private:
    Object const * object(int object_id) const;

    void linearVelocity(float impulse_parameter,
                        glm::vec3 const & normal,
                        float mass,
                        CollisionEvent const & initial_collision,
                        CollisionEvent & final_collision) const;

    void angularVelocity(float impulse_parameter,
                         glm::vec3 const & normal,
                         glm::vec3 const & radius,
                         float moment_of_inertia,
                         CollisionEvent const & initial_collision,
                         CollisionEvent & final_collision) const;

    void normalToEdge(glm::vec3 & normal) const;

    void coordinatesAtTime(float time, CollisionEvent const & collision, glm::vec3 & coordinates) const;

    void radiusAtPoint(float time, glm::vec3 const & point, CollisionEvent const & collision, glm::vec3 & radius) const;

    float momentOfInertia(int object_id, glm::vec3 const & axis) const;

    void velocityAtPoint(float time, glm::vec3 const & point, CollisionEvent const & collision, glm::vec3 & velocity) const;

    float impulseParameter(float elasticity,
                           glm::vec3 const & impact_velocity,
                           glm::vec3 const & normal,
                           float mass_a,
                           float mass_b,
                           glm::vec3 const & radius_a,
                           glm::vec3 const & radius_b,
                           float moment_of_inertia_a,
                           float moment_of_inertia_b) const;

    std::vector<Object*> const * objects_;
};

#endif
