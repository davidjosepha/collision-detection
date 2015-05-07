#include "dummyengine.h"
#include <vector>
#include <glm/glm.hpp>
#include "motionengine.h"
#include "object.h"
#include "state.h"

using namespace std;

DummyEngine::DummyEngine() { }

DummyEngine::DummyEngine(MotionEngine & motionengine,
                         const vector<Object*> & objects) {
  motionengine_ = &motionengine;
  objects_ = &objects;
  
  for(int i = 0; i < objects.size(); i++) {
    last_events_.push_back(CollisionEvent(i,                       // object id
                                     0.0,                          // time
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // initial_coordinates
                                     glm::vec3(0.0f, 0.0f, 1.0f),  // initial_axis
                                     0.0f,                         // initial_angle
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // axis_of_rotation
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // velocity
                                     0.0f));                       // angular_velocity
  }
}

void DummyEngine::getState(int object_id, float time, State & state) {
  while (event_queue_.size() > 0 && time > event_queue_.front().time()) {
    CollisionEvent & col = event_queue_.front();
    event_queue_.pop();
    last_events_[col.object()] = col;
  }

  Object * object = (*objects_)[object_id];
  state.setVerts(*(object->verts()));
  state.setTris(*(object->tris()));

  motionengine_->pose(last_events_[object_id], time, *state.pose());
}

int DummyEngine::numObjects() const {
  return objects_->size();
}
