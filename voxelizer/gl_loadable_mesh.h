#ifndef __GL_LOADABLE_MESH_H__
#define __GL_LOADABLE_MESH_H__

// An interface for triangle meshes that can load their own contents into GL.
class GlLoadableMesh {
 public:
  virtual ~GlLoadableMesh() {}
  virtual void LoadIntoGl() = 0;
};

#endif
