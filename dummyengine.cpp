#include "dummyengine.h"
#include <cstdio>
#include <vector>
#include <glm/glm.hpp>
#include "collision.h"
#include "motionengine.h"
#include "object.h"
#include "state.h"

using namespace std;

DummyEngine::DummyEngine() { }

DummyEngine::DummyEngine(MotionEngine & motionengine,
                         const vector<Object*> & objects) {
  motionengine_ = &motionengine;
  objects_ = &objects;
  
  for (int i = 0; i < objects.size(); i++) {
    last_events_.push_back(CollisionEvent(i,                       // object id
                                     0.0,                          // time
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // initial_coordinates
                                     glm::vec3(0.0f, 0.0f, 1.0f),  // initial_axis
                                     0.0f,                         // initial_angle
                                     glm::vec3(1.0f, 0.0f, 0.0f),  // axis_of_rotation
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // velocity
                                     0.0f));                       // angular_velocity
  }

  for (int i = 0; i < objects.size(); i++) {
    event_queue_.push(CollisionEvent(i,                       // object id
                                     0.0,                          // time
                                     glm::vec3((i - 0.5) * 4.0f, 0.0f, 0.0f),  // initial_coordinates
                                     glm::vec3(0.0f, 0.0f, 1.0f),  // initial_axis
                                     0.0f,                         // initial_angle
                                     glm::vec3(1.0f, 0.0f, 0.0f),  // axis_of_rotation
                                     glm::vec3(-4.0f * (i - 0.5), 0.0f, 0.0f),  // velocity
                                     0.0f));                       // angular_velocity
  }
}

void DummyEngine::randomEvent(int object_id) {
  int i = object_id;
  Collision collision = Collision(*objects_);
  CollisionEvent newcol_a, newcol_b;
  collision.generateCollisionEvents(last_events_[i].time() + .7,
                                    glm::vec3(0.0f, 0.0f, 0.0f),
                                    0,
                                    1,
                                    last_events_[0],
                                    last_events_[1],
                                    newcol_a,
                                    newcol_b);
  event_queue_.push(newcol_a);
  event_queue_.push(newcol_b);
}

void DummyEngine::getState(int object_id, float time, State & state) {
  while (event_queue_.size() > 0 && time > event_queue_.front().time()) {
    CollisionEvent & col = event_queue_.front();
    event_queue_.pop();
    last_events_[col.object()] = col;

    randomEvent(col.object());
  }

  Object * object = (*objects_)[object_id];
  state.setVerts(*(object->verts()));
  state.setTris(*(object->tris()));

  motionengine_->pose(last_events_[object_id], time, *state.pose());
}

int DummyEngine::numObjects() const {
  return objects_->size();
}

void DummyEngine::pushEvent(CollisionEvent const & col) {
  event_queue_.push(col); 
}
