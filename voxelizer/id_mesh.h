#ifndef __ID_MESH_H__
#define __ID_MESH_H__

#include <vector>
#include <GL/gl.h>
#include "geom_3d.h"
#include "gl_loadable_mesh.h"

struct IDVertex {
  Vec position;
  unsigned int id;
  IDVertex(float _x, float _y, float _z, unsigned int _id)
    : position(_x, _y, _z), id(_id) {}
  IDVertex(Vec _pos, unsigned int _id) : position(_pos), id(_id) {}
};

// A mesh whose vertices have a position and uint ID.
struct IDMesh : GlLoadableMesh {
  std::vector<IDVertex> verts;
  std::vector<unsigned int> tris;
  GLuint vao;
  GLuint vbuf;
  GLuint ibuf;
  
  IDMesh();
  ~IDMesh();
  void Clear();
  virtual void LoadIntoGl();
};

#endif
