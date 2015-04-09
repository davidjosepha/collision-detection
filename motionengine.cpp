#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include "geom_3d.h"
#include "cuboid.h"
#include "collisionevent.h"
#include "motionengine.h"

MotionEngine::MotionEngine() { }

void MotionEngine::position(CollisionEvent & event, float time, Cuboid & object, GLfloat (& vertices)[ARRAY_SIZE]) {
  float dtime = time - event.time();

  //rotate(dtime, event.angle(), event.axis(), vertices);
  //translate(dtime, event.coordinates(), event.velocity(), vertices);
}

void MotionEngine::rotate(float dtime, float angle, glm::vec3 & axis, GLfloat (& vertices)[ARRAY_SIZE]) {
  // to do, use glm for data in other areas to avoid stupid conversions like this one
  glm::fquat qrot = glm::fquat(cosf(angle / 2.0), axis * sinf(angle / 2.0));
  normalize(qrot);

   // cos(theta)I + sin(theta)[u]_x + (1 - cos(theta))(u tensorprod u)
  for (int i = 0; i < ARRAY_SIZE; i += 3) {
    glm::vec4 ptrot = qrot * glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);

    vertices[i] = ptrot.x;
    vertices[i + 1] = ptrot.y;
    vertices[i + 2] = ptrot.z;
  }
}

void MotionEngine::translate(float dtime, glm::vec3 & coordinates, glm::vec3 & velocity, GLfloat (& vertices)[ARRAY_SIZE]) {
  glm::vec3 dist = coordinates + dtime * velocity;

  for (int i = 0; i < ARRAY_SIZE; i += 3) {
    vertices[i] += dist.x;
    vertices[i + 1] += dist.y;
    vertices[i + 2] += dist.z;
  }
}
