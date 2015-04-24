#ifndef MOTIONENGINE_H
#define MOTIONENGINE_H
#include "collisionevent.h"

class MotionEngine {
  public:
    MotionEngine();
    void pose(CollisionEvent const & event, float time, glm::mat4 & pmat);

  private:
    glm::mat4 irotate(float angle, glm::vec3 const & axis) const;
    glm::mat4 trotate(float dtime, float angle, glm::vec3 const & axis) const;
    glm::mat4 translate(float dtime, glm::vec3 const & coordinates, glm::vec3 const & velocity) const;
};

#endif
