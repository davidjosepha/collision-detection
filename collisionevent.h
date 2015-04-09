#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

class CollisionEvent {
  public:
    CollisionEvent(Cuboid &object, float time,
                    glm::vec3 coordinates, glm::vec3 axis, float angle,
                    glm::vec3 velocity, float angular_velocity);

    Cuboid* object();
    float time();
    glm::vec3 coordinates();
    glm::vec3 axis();
    float angle();
    glm::vec3 velocity();
    float angular_velocity();

  private:
    Cuboid* object_;
    float time_;
    glm::vec3 coordinates_;
    glm::vec3 axis_;
    float angle_;
    glm::vec3 velocity_;
    float angular_velocity_;
};

#endif
