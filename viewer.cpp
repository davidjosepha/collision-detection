#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include "object.h"
#include "viewer.h"

Object * Viewer::model_;

Viewer::Viewer(Object & model) {
  model_ = &model;
}

void Viewer::model() {
    /*
  // Access each model from the list and use the vertex and index arrays from each
//  model_->genVerticesAndIndices();

  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, model_->vertices());
  //glColorPointer(3, GL_FLOAT, 0, model_->colors());

  glPushMatrix();
  glDrawElements(GL_TRIANGLES, 4*3*(model_->k_*8), GL_UNSIGNED_BYTE, model_->indices());
  glPopMatrix();

  glDisableClientState(GL_VERTEX_ARRAY);
//  glDisableClientState(GL_COLOR_ARRAY);
//  */
}

void Viewer::display() {
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // Reset transformations

  glEnable(GL_DEPTH_TEST); //enable the depth testing
  
  model();

  glFlush();
  glutSwapBuffers();
}

void Viewer::reshape(int w, int h) {
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
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
  glutCreateWindow("Awesome Cube");

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
