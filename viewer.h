#ifndef VIEWER_H
#define VIEWER_H

//
class Viewer {
  public:
    Viewer();
    Viewer(std::vector<Object*> & objects);
    static void initGlut(int argc, char * argv[]);

  private:
    static void model();
    static void display();
    static void reshape(int w, int h);

  static std::vector<Object*> * models_;
};

#endif
