#ifndef DUMMYENGINE_H
#define DUMMYENGINE_H
#include "collisionevent.h"
#include <vector>
#include "motionengine.h"
#include "object.h"
#include "state.h"

class DummyEngine {
  public:
    DummyEngine();
    DummyEngine(MotionEngine & motionengine,
                const std::vector<CollisionEvent> & states,
                const std::vector<Object*> & objects);
    void getState(int object_id, float time, State & state);
    int numObjects() const;
  private:
    const std::vector<CollisionEvent> * events_;
    const std::vector<Object*> * objects_;

    MotionEngine * motionengine_;
};

#endif
