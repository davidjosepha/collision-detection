#ifndef CONTROLLER_H
#define CONTROLLER_H

// A controller program that recieves keyboard input and passes it to the model
class Controller {
 public:
  Controller();
  Controller(Cuboid & model);
  void initViewer(double & rotate_x, double & rotate_y,
                  double & xpos, double & ypos, double & zpos,
                  double & xrot, double & yrot);
  static void initControls();

 private:
  static void specialKeys(int key, int x, int y);
  static void keyboard(unsigned char key, int x, int y);

  static Cuboid * model_;

  static std::string move_mode_;

  static double * rotate_x_, * rotate_y_;
  static double * xpos_, * ypos_, * zpos_;
  static double * xrot_, * yrot_;

  static int draw_mode_;
  static int face_to_change_;
};

#endif
