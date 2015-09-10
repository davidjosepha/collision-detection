#ifndef DUMMYENGINE_H
#define DUMMYENGINE_H
#include "collisionevent.h"
#include <queue>
#include <vector>
#include "motionengine.h"
#include "object.h"
#include "state.h"

class DummyEngine {
  public:
    DummyEngine();
    DummyEngine(MotionEngine & motionengine,
                std::vector<Object*> const & objects);
    void randomEvent(int object_id);
    void getState(int object_id, float time, State & state);
    int numObjects() const;
    void pushEvent(CollisionEvent const & col);
  private:
    std::vector<CollisionEvent> last_events_; // make not a pointer
    std::vector<Object*> const * objects_; // make reference not pointer
    std::queue<CollisionEvent> event_queue_;

    MotionEngine * motionengine_;
};

#endif
