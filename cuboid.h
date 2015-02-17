#ifndef CUBOID_H
#define CUBOID_H

class Cuboid {
    public:
        Cuboid();
        Cuboid(float x, float y, float z, float center[3], float color[3], float mass,
                float position[3], float axis[3],
                float velocity[3], float rotation);

        void impact(float mass, float position[3], float velocity[3]);

        void genVerticesAndIndices();
        GLfloat * position();
        GLfloat * axis();
        GLfloat * velocity();
        GLfloat rotation();

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

        GLfloat position_[3];
        GLfloat axis_[3];

        GLfloat velocity_[3];
        GLfloat rotation_;

        //GLfloat vertices_[8*3];
        GLfloat vertices_[2*25*20+3];
        GLubyte indices_[2*96*20];
        //GLubyte indices_[12*3*3];
        GLfloat colors_[2*25*20];
        //GLfloat colors_[12*3*3*8];

        float center_[3], color_[3];

        float theta_axis_[3], phi_axis_[3];
};

#endif
