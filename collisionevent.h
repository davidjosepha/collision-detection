#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

class CollisionEvent {
    public:
        CollisionEvent(Cuboid &object, float time,
                        Vec coordinates, Vec axis, float angle,
                        Vec velocity, float angular_velocity);

        Cuboid* object();
        float time();
        Vec coordinates();
        Vec axis();
        float angle();
        Vec velocity();
        float angular_velocity();

    private:
        Cuboid* object_;
        float time_;
        Vec coordinates_;
        Vec axis_;
        float angle_;
        Vec velocity_;
        float angular_velocity_;
};

#endif
