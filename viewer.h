#ifndef VIEWER_H
#define VIEWER_H

//
class Viewer {
public:
  Viewer(SuperEllipse & model, Controller & controller);
  static void initGlut(int argc, char * argv[]);

private:
  static void model();
  static void camera();
  static void display();
  static void reshape(int w, int h);

  static SuperEllipse * model_;
  static Controller * controller_;

  static double rotate_x_, rotate_y_;
  static double xpos_, ypos_, zpos_, xrot_, yrot_;

  static int face_to_change;
};

#endif
