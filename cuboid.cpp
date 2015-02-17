#include <cstdlib>
#include <cstdio>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "cuboid.h"

Cuboid::Cuboid() {
}

Cuboid::Cuboid(float x, float y, float z, float center[3], float color[3], float mass,
        float position[3], float axis[3],
        float velocity[3], float rotation) {

    x_ = x;
    y_ = y;
    z_ = z;
    //k_ = 3*3*8*8; // what the hell is this? we need to figure that out
    k_ = 3*3*12; // what the hell is this? we need to figure that out

    mass_ = mass;
    rotation_ = rotation;

    for (int i = 0; i < 3; ++i) {
        position_[i] = position[i];
        axis_[i] = axis[i];
        velocity_[i] = velocity[i];
        center_[i] = center[i];
        color_[i] = color[i];
    }

    clearVerticesAndIndices();
    genVertices();
    genIndices();
    genColors();
}

void Cuboid::impact(float mass, float position[3], float velocity[3]) {
    // vector from center of mass to point of impact
    float difference[3];

    for (int i = 0; i < 3; ++i) {
        difference[i] = position[i] - position_[i];
    }
    
    // vector normal to both the impact vector and difference vector
    float normal[3];
}

void Cuboid::genVerticesAndIndices() {
    genVertices();
    genIndices();
}

GLfloat * Cuboid::position() {
    return position_;
}

GLfloat * Cuboid::axis() {
    return axis_;
}

GLfloat * Cuboid::velocity() {
    return velocity_;
}

GLfloat Cuboid::rotation() {
    return rotation_;
}

GLfloat * Cuboid::vertices() {
    return vertices_;
}

GLubyte * Cuboid::indices() {
    return indices_;
}

GLfloat * Cuboid::colors() {
    return colors_;
}

void Cuboid::genColors() {
    /*
    int c_size = 0;

    for (float i = 1; i <= 2*25*20 - 1; i++) {
        colors_[c_size++] = (1/i)/2 + .5;
    }
    */
}

void Cuboid::genVertices() {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                vertices_[3*(4*i + 2*j + k)] = 2*i*x_ - 1;
                vertices_[3*(4*i + 2*j + k) + 1] = 2*j*y_ - 1;
                vertices_[3*(4*i + 2*j + k) + 2] = 2*k*z_ - 1;
            }
        }
    }

    //0  (0,0,0);
    //1  (0,0,z);
    //2  (0,y,0);
    //3  (0,y,z);
    //4  (x,0,0);
    //5  (x,0,z);
    //6  (x,y,0);
    //7  (x,y,z);
}

void Cuboid::genIndices() {
    // 6*2*3
    indices_[0] = 0;
    indices_[1] = 1;
    indices_[2] = 3;

    indices_[3] = 0;
    indices_[4] = 2;
    indices_[5] = 3;

    indices_[6] = 0;
    indices_[7] = 4;
    indices_[8] = 5;

    indices_[9] = 0;
    indices_[10] = 1;
    indices_[11] = 5;

    indices_[12] = 0;
    indices_[13] = 2;
    indices_[14] = 6;

    indices_[15] = 0;
    indices_[16] = 4;
    indices_[17] = 6;

    indices_[0+18] = 0+4;
    indices_[1+18] = 1+4;
    indices_[2+18] = 3+4;

    indices_[3+18] = 0+4;
    indices_[4+18] = 2+4;
    indices_[5+18] = 3+4;

    indices_[6+18] = 0+2;
    indices_[7+18] = 4+2;
    indices_[8+18] = 5+2;

    indices_[9+18] = 0+2;
    indices_[10+18] = 1+2;
    indices_[11+18] = 5+2;

    indices_[12+18] = 0+1;
    indices_[13+18] = 2+1;
    indices_[14+18] = 6+1;

    indices_[15+18] = 0+1;
    indices_[16+18] = 4+1;
    indices_[17+18] = 6+1;
    /*for (int i = 0; i < 8; ++i) {
        std::cout << indices_[3*i]; 
        std::cout << indices_[3*i + 1]; 
        std::cout << indices_[3*i + 2]; 
        std::cout << std::endl;
    }*/
}

void Cuboid::clearVerticesAndIndices() {
    //for (int i = 0; i < 1000; ++i) {
    for (int i = 0; i < 2*25*20+3; ++i) {
        vertices_[i] = 0;
    }

    for (int i = 0; i < 2*96*20; ++i) {
        indices_[i] = 0;
    }
}
