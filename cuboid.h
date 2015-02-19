#ifndef CUBOID_H
#define CUBOID_H
#include <gsl/gsl_vector.h>

class Cuboid {
    public:
        Cuboid(float x, float y, float z, gsl_vector & center, float mass,
                gsl_vector & axis, gsl_vector & velocity, float angle, float rotation);

        void impact(float mass, const gsl_vector & position, const gsl_vector & velocity);

        void genVerticesAndIndices();
        gsl_vector * center();
        gsl_vector * axis();
        gsl_vector * velocity();
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
        void crossProduct(const gsl_vector & u, const gsl_vector & v, gsl_vector & product);

        GLfloat mass_;

        gsl_vector * axis_;
        gsl_vector * velocity_;
        GLfloat angle_;
        GLfloat rotation_;

        GLfloat vertices_[25*20+3];
        GLubyte indices_[96*20];
        GLfloat colors_[25*20];
        //GLfloat vertices_[8*3];
        //GLubyte indices_[12*3*3];
        //GLfloat colors_[12*3*3*8];

        gsl_vector * center_;
};

#endif
