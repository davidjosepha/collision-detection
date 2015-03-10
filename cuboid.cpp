#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "geom_3d.h"
#include "cuboid.h"

Cuboid::Cuboid(float x, float y, float z, Vec & center, float mass,
        Vec & axis, Vec & velocity, float angle, float rotation) {

    x_ = x;
    y_ = y;
    z_ = z;
    k_ = 3*3*12;

    mass_ = mass;
    moment_of_inertia_ = mass_ * (pow(x,2) * pow(y,2) + pow(x,2) * pow(z,2) + pow(y,2) * pow(z,2))
                         / (6 * (pow(x,2) + pow(y,2) + pow(z,2)));
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

//                                            m_b                      
//                                                     I_b
//                              v_bp1                              n^
void Cuboid::impact(const Vec & collision_point, float impact_mass,
                    const Vec & impact_center, float impact_moment_of_inertia,
                    const Vec & impact_point_velocity, const Vec & impact_normal_unit) {
    // r_ap : vector from center of mass to point of impact
    Vec point_vector = collision_point - *center_;
    // r_bp : vector from center of mass of colliding object to point of impact
    Vec impact_point_vector = collision_point - impact_center;
    // w_a1 : initial pre-collision angular velocity
    Vec angular_velocity = *axis_ * rotation_;
    //
    // v_ap1 : initial velocity of impact point
    Vec point_velocity = velocityAtPoint(collision_point);
    // v_ab1 : initial velocity between two objects at point
    Vec total_point_velocity = point_velocity - impact_point_velocity;

    float elasticity = 1;
    // j : myphysicslab.com/collision.html
    float impulse_param = (-(1 + elasticity) * total_point_velocity.Dot(impact_normal_unit))
            / (
                1 / mass_ + 1 / impact_mass
                + point_vector.Cross(impact_normal_unit).Norm2() / moment_of_inertia_
                + impact_point_vector.Cross(impact_normal_unit).Norm2() / impact_moment_of_inertia
              );
    
    // v_a2 = v_a1 + j n^ / m_a
    *velocity_ = (impact_normal_unit * impulse_param) / mass_;
    // w_a2 = w_a1 + (r_ap x jn^) / I_a
    Vec final_angular_velocity = angular_velocity + point_vector.Cross(impact_normal_unit * impulse_param) / moment_of_inertia_;
    *axis_ = final_angular_velocity.Normalized();
    rotation_ = final_angular_velocity.Norm();


    std::cout << velocity_->x << ", ";
    std::cout << velocity_->y << ", ";
    std::cout << velocity_->z;
}

Vec Cuboid::velocityAtPoint(const Vec & point) {
    /*
     * velocity at point s = v + (r_hat x s) ω
     *     s := a point on the surface of object
     * r_hat := the unit vector of the rotational axis of the object
     *     v := velocity of the center of mass of the object
     *     ω := rotational frequency of the object
     **/
    Vec point_vector = point - *center_;

    Vec local_velocity = axis_->Cross(point_vector);
    local_velocity *= rotation_;
    local_velocity += *velocity_;

    return local_velocity;
}

void Cuboid::genVerticesAndIndices() {
    genVertices();
    genIndices();
}

Vec * Cuboid::center() {
    *center_ += *velocity_;
    return center_;
}

Vec * Cuboid::axis() {
    return axis_;
}

Vec * Cuboid::velocity() {
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
