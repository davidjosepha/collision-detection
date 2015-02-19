#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include "cuboid.h"

Cuboid::Cuboid(float x, float y, float z, gsl_vector & center, float mass,
        gsl_vector & axis, gsl_vector & velocity, float angle, float rotation) {

    x_ = x;
    y_ = y;
    z_ = z;
    k_ = 3*3*12;

    mass_ = mass;
    angle_ = angle;
    rotation_ = rotation;

    center_ = &center;
    axis_ = &axis;
    velocity_ = &velocity;

    clearVerticesAndIndices();
    genVertices();
    genIndices();
    genColors();
}

void Cuboid::impact(float mass, const gsl_vector & position, const gsl_vector & velocity) {
    // vector from center of mass to point of impact
    gsl_vector * center_to_impact = gsl_vector_alloc(3);
    gsl_vector_memcpy(center_to_impact, &position);
    gsl_vector_sub(center_to_impact, center_);

    gsl_vector * rotational_axis = gsl_vector_alloc(3);
    crossProduct(*center_to_impact, velocity, *rotational_axis);
    
    // get length of center_to_impact
    float unit_scale = pow(pow(gsl_vector_get(center_to_impact, 0), 2)
                         + pow(gsl_vector_get(center_to_impact, 1), 2)
                         + pow(gsl_vector_get(center_to_impact, 2), 2), 0.5);
    // create the unit vector of center_to_impact
    gsl_vector * unit_cti = gsl_vector_alloc(3);
    gsl_vector_memcpy(unit_cti, center_to_impact);
    gsl_vector_scale(unit_cti, 1 / unit_scale);

    // get the magnitude in the direction of the center of mass from the point of impact
    double movement_magnitude;
    gsl_blas_ddot(unit_cti, &velocity, &movement_magnitude);
    gsl_vector_memcpy(velocity_, unit_cti);

    // scale the vector so its length is the correct magnitude
    gsl_vector_scale(velocity_, movement_magnitude);

    // vector normal to both the impact vector and difference vector
}

// taken from https://gist.github.com/jmbr/668083
void Cuboid::crossProduct(const gsl_vector & u, const gsl_vector & v, gsl_vector & product) {
    double p1 = gsl_vector_get(&u, 1)*gsl_vector_get(&v, 2)
        - gsl_vector_get(&u, 2)*gsl_vector_get(&v, 1);
 
    double p2 = gsl_vector_get(&u, 2)*gsl_vector_get(&v, 0)
        - gsl_vector_get(&u, 0)*gsl_vector_get(&v, 2);
 
    double p3 = gsl_vector_get(&u, 0)*gsl_vector_get(&v, 1)
        - gsl_vector_get(&u, 1)*gsl_vector_get(&v, 0);
 
    gsl_vector_set(&product, 0, p1);
    gsl_vector_set(&product, 1, p2);
    gsl_vector_set(&product, 2, p3);
}

void Cuboid::genVerticesAndIndices() {
    genVertices();
    genIndices();
}

gsl_vector * Cuboid::center() {
    gsl_vector_add(center_, velocity_);
    return center_;
}

gsl_vector * Cuboid::axis() {
    return axis_;
}

gsl_vector * Cuboid::velocity() {
    return velocity_;
}

GLfloat Cuboid::angle() {
    angle_ += rotation_;
    return angle_;
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
    int c_size = 0;
    for (float i = 1; i < 25*20; ++i) {
        colors_[c_size++] = (1/i)/2 + .5;
    }

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
}

void Cuboid::clearVerticesAndIndices() {
    //for (int i = 0; i < 1000; ++i) {
    for (int i = 0; i < 25*20+3; ++i) {
        vertices_[i] = 0;
    }

    for (int i = 0; i < 96*20; ++i) {
        indices_[i] = 0;
    }
}
