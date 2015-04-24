#ifndef __MULTI_COMPONENT_SLICEMAPPER_H__
#define __MULTI_COMPONENT_SLICEMAPPER_H__

#include <GL/gl.h>
#include "geom_3d.h"
#include <gl_compute/glx_data.h>
#include "id_mesh.h"
#include "multi_component_slicemap.h"

class MultiComponentSlicemapper {
 public:
  // LoadOpenGlFunctions() and glx_context.SetUpOffscreenRendering() must be
  // called before initializing a MultiComponentSlicemapper.
  MultiComponentSlicemapper(GLXData & glx_context);
  ~MultiComponentSlicemapper();
  
  // Sets the rendering volume for the next call to Voxelize().
  // 
  // Arguments:
  // - corner: the position in model space of the bottom corner of the
  //     rendering volume.  Note this is *not* the same as the center of the
  //     bottom-corner voxel!
  // - size: the size of the rendering volume in model space, from the
  //     bottom corner to the top corner.  Again note that these are the
  //     corners of the volumes, not the centers of the corner voxels.
  // - num_voxels: the number of image-space voxels in each dimension into
  //     which to render the rendering volume.  Z may be rounded up, per the
  //     comments in Slicemap.h; if so the rendered model will occupy the lower
  //     Z values.
  // - num_components: the number of model components that will be
  //     rendered in this volume.
  // 
  // Returns false if the requested image volume or number of components were
  // too large.
  bool SetBox(Point const & corner, Vec const & size, UIVec const & num_voxels,
              unsigned int num_components);
  
  // Voxelizes the mesh using the current rendering volume.
  MultiComponentSlicemap const & Voxelize(IDMesh & mesh);
  
  // Gets the current slicemap.
  MultiComponentSlicemap const & slicemap() const { return slicemap_; }
  
 protected:
  MultiComponentSlicemap slicemap_;
  GLXData & glx_context_;
  
  static unsigned int const MAX_Z_SLABS = 32;
  // OpenGL fails on any 3D texture whose largest dimension is longer than this.
  GLint max_3d_texture_dim_;
  
  // The following strings must be consistent with the names of certain uniform
  // variables used in the shader program.
  static char const * SHADER_UNIFORM_NAME_SLICEMAP;
  static char const * SHADER_UNIFORM_NAME_DEPTH_MASK_LUT;
  static char const * SHADER_UNIFORM_NAME_BOX_POS;
  static char const * SHADER_UNIFORM_NAME_BOX_SIZE;
  static char const * SHADER_UNIFORM_NAME_IMAGE_SIZE;
  static char const * SHADER_UNIFORM_NAME_NUM_Z_SLABS;
  
  // Rendering box information.  Note that the slicemap itself may cover a
  // larger volume than image_size_; we have to store this size (rather than
  // reading it from slicemap_) in order to avoid warping the image space.
  Point model_box_corner_;
  Vec model_box_size_;
  UIVec image_size_;
  
  // Information for the depth mask LUT.
  unsigned int max_depth_;
  MultiComponentSlicemap::Bitfield * depth_mask_lut_data_;
  
  // Just a bunch of zeros, for clearing the slicemap image before each render.
  MultiComponentSlicemap empty_bitfield_;
  
  GLuint program_id_;
  
  // Various IDs for the images.
  static GLint const depth_mask_lut_image_unit_ = 0;
  static GLint const slicemap_image_unit_ = 1;
  GLuint depth_mask_lut_texture_name_;
  GLuint slicemap_texture_name_;
};

#endif
