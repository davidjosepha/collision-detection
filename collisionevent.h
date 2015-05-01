#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H
#include <glm/glm.hpp>

class CollisionEvent {
  public:
    CollisionEvent(int object,
                   double time,
                   glm::vec3 initial_coordinates_,
                   glm::vec3 initial_axis,
                   float initial_angle,
                   glm::vec3 axis_of_rotation_,
                   glm::vec3 velocity,
                   float angular_velocity);

    int object() const;
    float time() const;
    glm::vec3 const * initial_coordinates() const;
    glm::vec3 const * initial_axis() const;
    float initial_angle() const;

    glm::vec3 const * velocity() const;
    glm::vec3 const * axis_of_rotation() const;
    float angular_velocity() const;

  private:
    int object_;  // TODO: Use an entity id instead.
    double time_;
    glm::vec3 initial_coordinates_;
    glm::vec3 initial_axis_;
    float initial_angle_;

    glm::vec3 velocity_;
    glm::vec3 axis_of_rotation_;
    float angular_velocity_;
};

#endif
