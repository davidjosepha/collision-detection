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
#include "cuboid.h"
#include "controller.h"
#include "viewer.h"

//#define M_PI 3.1415926535

Cuboid * Viewer::model_;
Controller * Viewer::controller_;

double Viewer::rotate_x_, Viewer::rotate_y_;
double Viewer::xpos_, Viewer::ypos_, Viewer::zpos_;
double Viewer::xrot_, Viewer::yrot_;

int face_to_change;

Viewer::Viewer(Cuboid & model, Controller & controller) {
  rotate_x_ = 0; rotate_y_ = 0;
  xpos_ = 0; ypos_ = 0; zpos_ = 0;
  xrot_ = 0; yrot_ = 0;

  model_ = &model;
  controller_ = &controller;
  controller_->initViewer(rotate_x_, rotate_y_,
                         xpos_, ypos_, zpos_, xrot_, yrot_);
}

void Viewer::model() {
  model_->genVerticesAndIndices();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, model_->vertices());
  glColorPointer(3, GL_FLOAT, 0, model_->colors());

  glPushMatrix();
  //glRotatef(model_->phi()*M_PI, cos(theta), 0.0f, sin(theta));
  gsl_vector * axis = model_->axis();
  glRotatef(model_->angle()*M_PI, gsl_vector_get(axis, 0), gsl_vector_get(axis, 1), gsl_vector_get(axis, 2));

  gsl_vector * center = model_->center();
  glTranslatef(gsl_vector_get(center, 0), gsl_vector_get(center, 1), gsl_vector_get(center, 2));
  glDrawElements(GL_TRIANGLES, model_->k_, GL_UNSIGNED_BYTE, model_->indices());
  glPopMatrix();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

void Viewer::camera() {
  glRotated(xrot_,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
  glRotated(yrot_,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
  glTranslated(-xpos_,-ypos_,-zpos_); //translate the screen to the position of our camera
}

void Viewer::display() {
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // Reset transformations

  camera();
  glEnable(GL_DEPTH_TEST); //enable the depth testing
  // glEnable(GL_LIGHTING); //enable the lighting
  // glEnable(GL_LIGHT0); //enable LIGHT0, our Diffuse Light
  // glShadeModel (GL_SMOOTH); //set the shader to smooth shader

  // Rotate when user changes rotate_x and rotate_y
  glRotatef(rotate_x_, 1.0, 0.0, 0.0 );
  glRotatef(rotate_y_, 0.0, 1.0, 0.0 );

  model();

  glFlush();
  glutSwapBuffers();
}

void Viewer::reshape(int w, int h) {
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 0.1, 200.0);
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
  // glutKeyboardFunc(controller_->keyboard);
  // glutSpecialFunc(controller_->specialKeys);
  controller_->initControls();

  //  Pass control to GLUT for events
  glutMainLoop();
}
