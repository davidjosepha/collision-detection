#ifndef CUBOID_H
#define CUBOID_H

class Cuboid {
    public:
        Cuboid(float x, float y, float z, glm::vec3 & center, float mass,
                glm::vec3 & axis, glm::vec3 & velocity, float angle, float rotation);

        void impact(const glm::vec3 & collision_point, float impact_mass,
                    const glm::vec3 & impact_center, float impact_moment_of_inertia,
                    const glm::vec3 & impact_point_velocity, const glm::vec3 & impact_normal_unit);
        glm::vec3 velocityAtPoint(const glm::vec3 & point);

        void genVerticesAndIndices();
        glm::vec3 * center();
        glm::vec3 * axis();
        glm::vec3 * velocity();
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
        GLfloat moment_of_inertia_;

        // the unit vector around which the Cuboid rotates
        glm::vec3 * axis_;
        // the velocity of the center of mass of the Cuboid
        glm::vec3 * velocity_;
        // the current rotation of the Cuboid against the axis (radians)
        GLfloat angle_;
        // the current rotational frequency of the Cuboid against the axis (radians / t)
        GLfloat rotation_;

        GLfloat vertices_[25*20+3];
        GLubyte indices_[96*20];
        GLfloat colors_[25*20];
        //GLfloat vertices_[8*3];
        //GLubyte indices_[12*3*3];
        //GLfloat colors_[12*3*3*8];

        glm::vec3 * center_;
};

#endif
