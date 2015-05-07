#include <cstdio>
#include <vector>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include "object.h"
#include "viewer.h"
#include "state.h"
#include "dummyengine.h"

DummyEngine * Viewer::dummyengine_;
float Viewer::time_;

Viewer::Viewer() {}

Viewer::Viewer(DummyEngine & dummyengine) {
  dummyengine_ = &dummyengine;
  time_ = 0.0;
}

void Viewer::populateGlBuffers(float time) {
  State state = State();
  glm::mat4 * pose;
  int numobjects_ = dummyengine_->numObjects();
  for (int i = 0; i < numobjects_; i++) {
    dummyengine_->getState(i, time, state);
    pose = state.pose();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(4, GL_FLOAT, 0, state.verts());
    glColorPointer(4, GL_FLOAT, 0, state.verts());

    glPushMatrix();
    glMultMatrixf((float*)state.pose());
    glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, state.tris());
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }
}

void Viewer::display() {
  glClearColor(255.0,255.0,255.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // Reset transformations

  // default camera position
  glTranslatef(0., 0., -4.);

  // default to showing the borders of triangles
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_DEPTH_TEST); //enable the depth testing
  
  populateGlBuffers(time_);
  time_ += 0.01;
  
  if (time_ > 100.0) {
    exit(0);
  }

  glFlush();
  glutSwapBuffers();
}

void Viewer::reshape(int w, int h) {
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0, (GLfloat) w/(GLfloat) h, 0.1, 200.0);
  glMatrixMode(GL_MODELVIEW);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Viewer::initGlut(int argc, char * argv[]) {
  //  Initialize GLUT and process user parameters
  glutInit(&argc, argv);

  //  Request double buffered true color window with Z-buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Create window
  glutInitWindowSize(1000,800);
  glutCreateWindow("Collision Detection Demo");

  //  Enable Z-buffer depth test
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  // Callback functions
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(reshape);

  //  Pass control to GLUT for events
  glutMainLoop();
}
