#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <iostream>
#include <string>
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

#define _USE_MATH_DEFINES
#define M_PI 3.1415926535

// SuperEllipse Controller::model_;

int Controller::draw_mode_;
int Controller::face_to_change_;
Cuboid * Controller::model_;

std::string Controller::move_mode_;

double * Controller::rotate_x_, * Controller::rotate_y_;
double * Controller::xpos_, * Controller::ypos_, * Controller::zpos_;
double * Controller::xrot_, * Controller::yrot_;

Controller::Controller() {
}

Controller::Controller(Cuboid & model) {
  model_ = &model;
  move_mode_ = "move";
  draw_mode_ = 0;
  face_to_change_ = 1;
}

void Controller::initViewer(double & rotate_x, double & rotate_y,
                            double & xpos, double & ypos, double & zpos,
                            double & xrot, double & yrot) {
  rotate_x_ = &rotate_x;
  rotate_y_ = &rotate_y;
  xpos_ = &xpos;
  ypos_ = &ypos;
  zpos_ = &zpos;
  xrot_ = &xrot;
  yrot_ = &yrot;
}

void Controller::specialKeys( int key, int x, int y ) {
  if(move_mode_ == "rotate") {

    if(key == GLUT_KEY_UP) {
      *rotate_x_ -= 2;

    } else if(key == GLUT_KEY_DOWN) {
      *rotate_x_ += 2;

    } else if(key == GLUT_KEY_RIGHT) {
      *rotate_y_ += 2;

    } else if(key == GLUT_KEY_LEFT) {
      *rotate_y_ -= 2;
    }

  } else if(move_mode_ == "move") {
    if(key == GLUT_KEY_UP) {
      float xrotrad, yrotrad;
      yrotrad = (*yrot_ / 180 * 3.141592654f);
      xrotrad = (*xrot_ / 180 * 3.141592654f);
      *xpos_ += float(sin(yrotrad)) / 4;
      *zpos_ -= float(cos(yrotrad)) / 4;
      *ypos_ -= float(sin(xrotrad)) / 4;

    } else if(key == GLUT_KEY_DOWN) {
      float xrotrad, yrotrad;
      yrotrad = (*yrot_ / 180 * 3.141592654f);
      xrotrad = (*xrot_ / 180 * 3.141592654f);
      *xpos_ -= float(sin(yrotrad)) / 4;
      *zpos_ += float(cos(yrotrad)) / 4;
      *ypos_ += float(sin(xrotrad)) / 4;

    } else if(key == GLUT_KEY_RIGHT) {
      *yrot_ += 1;
      if(*yrot_ >360) {
        *yrot_ -= 360;
      }
    } else if(key == GLUT_KEY_LEFT) {
      *yrot_ -= 1;
      if(*yrot_ < -360){
        *yrot_ += 360;
      }
    }

  } else if(move_mode_ == "faces") {

  } else if(move_mode_ == "size") {

  } else if(move_mode_ == "vertices") {

  }

  //  Request display update
  glutPostRedisplay();
}

void Controller::keyboard (unsigned char key, int x, int y) {
  if(key == 27) {
    exit(0);
  }

  if(key == 'r') {
    move_mode_ = "rotate";
  } else if(key == 'm') {
    move_mode_ = "move";
  } else if(key == 'f') {
    move_mode_ = "faces";
  } else if(key == 'v') {
    move_mode_ = "vertices";
  } else if(key == 's') {
    move_mode_ = "size";

  } else if(key == 'q') {
    *xrot_ += 1;
    if(*xrot_ >360) {
      *xrot_ -= 360;
    }
  } else if(key == 'z') {
    *xrot_ -= 1;
    if(*xrot_ < -360) {
      *xrot_ += 360;
    }

  } else if(key == '1') {

  } else if(key == '2') {

  } else if(key == 32) {
    draw_mode_ = (draw_mode_ + 1) % 3;
    if(draw_mode_ == 0) {       // fill mode
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
    }
    else if(draw_mode_ == 1) { // wireframe mode
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);
    }
    else {                   // point mode
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);
    }
  }
}

void Controller::initControls() {
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeys);
}
