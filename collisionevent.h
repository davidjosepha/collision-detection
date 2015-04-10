#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

class CollisionEvent {
  public:
    CollisionEvent(Cuboid &object, float time,
                    glm::vec3 initial_coordinates_, glm::vec3 initial_axis,
                    float initial_angle,
                    glm::vec3 axis_of_rotation_, glm::vec3 velocity,
                    float angular_velocity);

    Cuboid & object();
    float time();
    glm::vec3 & initial_coordinates();
    glm::vec3 & initial_axis();
    float initial_angle();

    glm::vec3 & velocity();
    glm::vec3 & axis_of_rotation_;
    float angular_velocity();

  private:
    Cuboid* object_;  // TODO: Use an entity id instead.
    double time_;
    glm::vec3 initial_coordinates_;
    glm::vec3 initial_axis_;
    float initial_angle_;

    glm::vec3 velocity_;
    glm::vec3 axis_of_rotation_;
    float angular_velocity_;
};

#endif
