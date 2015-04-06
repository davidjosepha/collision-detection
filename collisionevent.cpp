#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "geom_3d.h"
#include "cuboid.h"
#include "collisionevent.h"

CollisionEvent::CollisionEvent(Cuboid &object, float time,
                                Vec coordinates, Vec axis, float angle,
                                Vec velocity, float angular_velocity) {
    object_ = &object;
    time_ = time;
    coordinates_ = coordinates;
    axis_ = axis;
    angle_ = angle;
    velocity_ = velocity;
    angular_velocity_ = angular_velocity;
}

Cuboid* CollisionEvent::object() {
    return object_;
}

float CollisionEvent::time() {
    return time_;
}

Vec CollisionEvent::coordinates() {
    return coordinates_;
}

Vec CollisionEvent::axis() {
    return axis_;
}

float CollisionEvent::angle() {
    return angle_;
}

Vec CollisionEvent::velocity() {
    return velocity_;
}

float CollisionEvent::angular_velocity() {
    return angular_velocity_;
}
