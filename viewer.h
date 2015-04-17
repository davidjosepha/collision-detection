#ifndef VIEWER_H
#define VIEWER_H

//
class Viewer {
  public:
    Viewer(Object & model);
    static void initGlut(int argc, char * argv[]);

  private:
    static void model();
    static void display();
    static void reshape(int w, int h);

  static Object * model_;
};

#endif
