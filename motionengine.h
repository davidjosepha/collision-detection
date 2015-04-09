#ifndef MOTIONENGINE_H
#define MOTIONENGINE_H
#define ARRAY_SIZE 1000

class MotionEngine {
  public:
    MotionEngine();
    void position(CollisionEvent & event, float time, Cuboid & object, GLfloat (& vertices)[ARRAY_SIZE]);

  private:
    void rotate(float dtime, float angle, glm::vec3 & axis, GLfloat (& vertices)[ARRAY_SIZE]);
    void translate(float dtime, glm::vec3 & coordinates, glm::vec3 & velocity, GLfloat (& vertices)[ARRAY_SIZE]);
};

#endif
