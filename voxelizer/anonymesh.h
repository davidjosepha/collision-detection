#ifndef __ANONYMESH_H__
#define __ANONYMESH_H__

#include <vector>
#include <GL/gl.h>
#include "geom_3d.h"
#include "gl_loadable_mesh.h"

struct Anonymesh : GlLoadableMesh {
  std::vector<Vec> verts;
  std::vector<unsigned int> tris;
  GLuint vao;
  GLuint vbuf;
  GLuint ibuf;
  
  Anonymesh();
  ~Anonymesh();
  void Clear();
  virtual void LoadIntoGl();
};

#endif
