#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H
#include <glm/glm.hpp>

class CollisionEvent {
  public:
    CollisionEvent();
    CollisionEvent(int object,
                   float time,
                   glm::vec3 initial_coordinates,
                   glm::vec3 initial_axis,
                   float initial_angle,
                   glm::vec3 axis_of_rotation,
                   glm::vec3 velocity,
                   float angular_velocity);

    void setObjectId(int object) { object_ = object; };
    void setTime(float time) { time_ = time; };
    void setInitialCoordinates(glm::vec3 initial_coordinates) { initial_coordinates_ = initial_coordinates; };
    void setInitialAxis(glm::vec3 initial_axis) { initial_axis_ = initial_axis; };
    void setInitialAngle(int initial_angle) { initial_angle_ = initial_angle; };
    void setAxisOfRotation(glm::vec3 axis_of_rotation) { axis_of_rotation_ = axis_of_rotation; };
    void setVelocity(glm::vec3 velocity) { velocity_ = velocity; };
    void setAngularVelocity(int angular_velocity) { angular_velocity_ = angular_velocity; };

    void setValues(int object,
                   float time,
                   glm::vec3 initial_coordinates,
                   glm::vec3 initial_axis,
                   float initial_angle,
                   glm::vec3 axis_of_rotation,
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
    float time_;
    glm::vec3 initial_coordinates_;
    glm::vec3 initial_axis_;
    float initial_angle_;

    glm::vec3 velocity_;
    glm::vec3 axis_of_rotation_;
    float angular_velocity_;
};

#endif
