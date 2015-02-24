#ifndef CUBOID_H
#define CUBOID_H
#include "geom_3d.h"

class Cuboid {
    public:
        Cuboid(float x, float y, float z, Vec & center, float mass,
                Vec & axis, Vec & velocity, float angle, float rotation);

        void impact(float mass, const Vec & position, const Vec & velocity);

        void genVerticesAndIndices();
        Vec * center();
        Vec * axis();
        Vec * velocity();
        GLfloat angle();

        GLfloat * vertices();
        GLubyte * indices();
        GLfloat * colors();

        float x_, y_, z_, k_;

    private:
        void genColors();
        void genVertices();
        void genIndices();

        void clearVerticesAndIndices();

        GLfloat mass_;

        Vec * axis_;
        Vec * velocity_;
        GLfloat angle_;
        GLfloat rotation_;

        GLfloat vertices_[25*20+3];
        GLubyte indices_[96*20];
        GLfloat colors_[25*20];
        //GLfloat vertices_[8*3];
        //GLubyte indices_[12*3*3];
        //GLfloat colors_[12*3*3*8];

        Vec * center_;
};

#endif
