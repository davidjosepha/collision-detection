#include <cstdlib>
#include <cstdio>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "glm/gtx/transform.hpp"
#include "geom_3d.h"
#include "cuboid.h"
#include "collisionevent.h"

void position(CollisionEvent & event, float time, Cuboid & object, GLfloat & vertices) {
    float dtime = time - event.time();
    // http://glm.g-truc.net/0.9.4/api/a00206.html
    
    // cos(theta)I + sin(theta)[u]_x + (1 - cos(theta))(u tensorprod u)
    for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += 3) {

    }
}
