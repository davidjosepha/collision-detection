#ifndef OBJECT_H
#define OBJECT_H

class Object {
    public:
        double radius();
        glm::mat4 vertices();
    private:
        double radius_;
        glm::mat4 vertices_;
};

#endif
