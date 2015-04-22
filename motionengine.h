#ifndef MOTIONENGINE_H
#define MOTIONENGINE_H
#include "collisionevent.h"

class MotionEngine {
  public:
    MotionEngine();
    void pose(CollisionEvent & event, float time, glm::mat4 & pmat);

  private:
    glm::mat4 irotate(float angle, glm::vec3 & axis);
    glm::mat4 trotate(float dtime, float angle, glm::vec3 & axis);
    glm::mat4 translate(float dtime, glm::vec3 & coordinates, glm::vec3 & velocity);
};

#endif
