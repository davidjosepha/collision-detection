#include "dummyengine.h"
#include <vector>
#include <glm/glm.hpp>
#include "motionengine.h"
#include "object.h"
#include "state.h"

DummyEngine::DummyEngine() { }

DummyEngine::DummyEngine(MotionEngine & motionengine,
                         const std::vector<CollisionEvent> & events,
                         const std::vector<Object*> & objects) {
  motionengine_ = &motionengine;
  events_ = &events;
  objects_ = &objects;
}

void DummyEngine::getState(int object_id, float time, State & state) {
  //events_[object_id];
  Object * object = (*objects_)[object_id];
  state.setVerts(*(object->verts()));
  state.setTris(*(object->tris()));

  motionengine_->pose((*events_)[object_id], time, *state.pose()); 
  //state.setPose(pose);
}

int DummyEngine::numObjects() {
  return objects_->size();
}
