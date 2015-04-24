#include "multi_component_slicemapper.h"

#include <stdio.h>
#include <string.h>

#include <GL/gl.h>
#include <gl_compute/gl_helper.h>
#include <gl_compute/glx_data.h>

#include "geom_3d.h"
#include "id_mesh.h"
#include "multi_component_slicemap.h"

// Comment-stripped GLSL source code as a character array.
#include "multi_component_slicemapper.glsl.h"

char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_SLICEMAP =
  "slicemap";
char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_DEPTH_MASK_LUT =
  "depth_mask_lut";
char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_BOX_POS =
  "box_pos";
char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_BOX_SIZE =
  "box_size";
char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_IMAGE_SIZE =
  "image_size";
char const * MultiComponentSlicemapper::SHADER_UNIFORM_NAME_NUM_Z_SLABS =
  "num_z_slabs";

MultiComponentSlicemapper::MultiComponentSlicemapper(GLXData & glx_context)
    : glx_context_(glx_context) {
  model_box_corner_ = Point(0,0,0);
  model_box_size_ = Vec(0,0,0);
  image_size_ = UIVec(0,0,0);
  
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_3d_texture_dim_);
  
  // Generate the data for the depth mask LUT.  It's a 2-D image for which the
  // value at (x,y) is the depth mask in Z-slab x for the depth y.
  max_depth_ = MultiComponentSlicemap::NUM_BITS_PER_BITFIELD * MAX_Z_SLABS + 1;
  //printf("Allocating %ux%u = %u texels for the depth mask LUT.\n", MAX_Z_SLABS, max_depth_, MAX_Z_SLABS * max_depth_);
  depth_mask_lut_data_ =
    new MultiComponentSlicemap::Bitfield[MAX_Z_SLABS * max_depth_];
  MultiComponentSlicemap::Bitfield full_depth_mask =
    ~(MultiComponentSlicemap::Bitfield(0));
  unsigned int * it = depth_mask_lut_data_;
  // For OpenGL images, the X coordinate is the fastest-changing index.
  for(unsigned int depth = 0; depth < max_depth_; ++depth) {
    for(unsigned int slab = 0; slab < MAX_Z_SLABS; ++slab) {
      if(depth < slab * MultiComponentSlicemap::NUM_BITS_PER_BITFIELD) {
        *(it++) = 0;
      } else if(depth >= (slab+1) *
                MultiComponentSlicemap::NUM_BITS_PER_BITFIELD) {
        *(it++) = full_depth_mask;
      } else {
        *(it++) = ~(full_depth_mask <<
                    (depth % MultiComponentSlicemap::NUM_BITS_PER_BITFIELD));
      }
    }
  }
  //it = depth_mask_lut_data_; for(unsigned int depth = 0; depth < max_depth_; ++depth) { for(unsigned int slab = 0; slab < MAX_Z_SLABS; ++slab) { for(unsigned int bit = 0; bit < 32; ++bit) { if(*it & (1u << bit)) { printf("X"); } else { printf("-"); } } printf(" "); ++it; } printf("\n"); }
  
  // Reserve texture names for the slicemap and the depth mask LUT.  We'll load
  // the images into the textures in Voxelize().
  glGenTextures(1, &slicemap_texture_name_); PrintOpenglError();
  glGenTextures(1, &depth_mask_lut_texture_name_); PrintOpenglError();
  
  // Compile the program.
  program_id_ = LoadOpenGlShaderProgramFromSource(
    reinterpret_cast<char const *>(multi_component_slicemapper_glsl_src),
    multi_component_slicemapper_glsl_src_len); PrintOpenglError();
}

MultiComponentSlicemapper::~MultiComponentSlicemapper() {
  glDeleteTextures(1, &depth_mask_lut_texture_name_);
  glDeleteTextures(1, &slicemap_texture_name_);
  glDeleteProgram(program_id_);
  delete[] depth_mask_lut_data_;
}

bool MultiComponentSlicemapper::SetBox(Point const & corner, Vec const & size,
                                       UIVec const & num_voxels,
                                       unsigned int num_components) {
  //printf("Resizing the slicemap.\n");
  bool good_alloc = true;
  bool new_alloc = false;
#if DEBUG
  unsigned int num_z_slabs = CeilDivide(num_voxels.z,
    MultiComponentSlicemap::NUM_BITS_PER_BITFIELD);
  if(num_voxels.x > max_3d_texture_dim_ ||
     num_voxels.y > max_3d_texture_dim_ ||
     num_z_slabs * num_components > max_3d_texture_dim_ ||
     num_z_slabs > MAX_Z_SLABS) {
    printf("Requested slicemap num voxels (%s) with %u components is too big.\n",
           num_voxels.AsString().c_str(), num_components);
    good_alloc = false;
  }
#endif
  if(good_alloc) {
    good_alloc = slicemap_.Resize(num_voxels.x, num_voxels.y, num_voxels.z,
                                  num_components, new_alloc);
  }
  if(good_alloc) {
    //printf("Resizing the slicemapper's empty bitfield.\n");
    good_alloc = empty_bitfield_.Resize(num_voxels.x, num_voxels.y,
                                        num_voxels.z, num_components,
                                        new_alloc);
    if(new_alloc) {
      empty_bitfield_.Clear();
    }
  }
  if(!good_alloc) {
    printf("Allocation error in MultiComponentSlicemapper::SetBox()!\n");
    //printf("Resizing both the slicemap and empty bitfield to zero.\n");
    slicemap_.Resize(0,0,0,0, new_alloc);
    empty_bitfield_.Resize(0,0,0,0, new_alloc);
    model_box_corner_ = Point(0,0,0);
    model_box_size_ = Vec(0,0,0);
    image_size_ = UIVec(0,0,0);
    return false;
  }
  
  model_box_corner_ = corner;
  model_box_size_ = size;
  image_size_ = num_voxels;
  return true;
}

// Voxelizes the mesh using the current rendering volume.
MultiComponentSlicemap const & MultiComponentSlicemapper::Voxelize(IDMesh & mesh)
{
  if(slicemap_.size() == 0) {
    return slicemap_;
  }
  
  glUseProgram(program_id_);
  
  // Set the values for the regular shader uniforms.
  //printf("Setting box_pos = %s\n", model_box_corner_.AsString().c_str());
  glUniform3f(glGetUniformLocation(program_id_,
                                   SHADER_UNIFORM_NAME_BOX_POS),
              model_box_corner_.x, model_box_corner_.y, model_box_corner_.z);
  //printf("Setting box_size = %s\n", model_box_size_.AsString().c_str());
  glUniform3f(glGetUniformLocation(program_id_,
                                   SHADER_UNIFORM_NAME_BOX_SIZE),
              model_box_size_.x, model_box_size_.y, model_box_size_.z);
  //printf("Setting image_size = %s\n", image_size_.AsString().c_str());
  glUniform3f(glGetUniformLocation(program_id_,
                                   SHADER_UNIFORM_NAME_IMAGE_SIZE),
              image_size_.x, image_size_.y, image_size_.z);
  glUniform1ui(glGetUniformLocation(program_id_,
                                    SHADER_UNIFORM_NAME_NUM_Z_SLABS),
               slicemap_.num_z_slabs());
  PrintOpenglError();
  
  glx_context_.SetViewportSize(slicemap_.x(), slicemap_.y());
  
  
  // Copy image data into textures and attach them to the right uniforms.
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_DEPTH_MASK_LUT,
                       depth_mask_lut_data_,
                       GL_READ_ONLY,
                       GL_UNSIGNED_INT,
                       program_id_,
                       depth_mask_lut_texture_name_,
                       depth_mask_lut_image_unit_,
                       MAX_Z_SLABS,
                       max_depth_);
  PrintOpenglError();
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_SLICEMAP,
                       empty_bitfield_.data(),
                       GL_READ_WRITE,
                       GL_UNSIGNED_INT,
                       program_id_,
                       slicemap_texture_name_,
                       slicemap_image_unit_,
                       slicemap_.x(),
                       slicemap_.y(),
                       slicemap_.num_z_slabs() * slicemap_.num_components());
  if(PrintOpenglError()) {
    printf("Tried to create a 3D image %ux%ux%u = %u; max size is %i.\n",
           slicemap_.x(), slicemap_.y(),
           slicemap_.num_z_slabs() * slicemap_.num_components(),
           slicemap_.size(), max_3d_texture_dim_);
  }
  
  // Invoke the shader to render the mesh.
  mesh.LoadIntoGl();
  glBindVertexArray(mesh.vao);
  
  // Depth clamp makes it so that fragments located beyond the far clipping
  // plane (maximum Z plane of rendering volume) get clamped in Z to that
  // maximum value, so that they still contribute a full mask to the slicemap.
  // Failing to enable depth clamp makes the slicemap turn "inside out" for
  // columns where the far fragment of a component's bounding surface is beyond
  // the maximum Z value.
  glEnable(GL_DEPTH_CLAMP);
  
  glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, NULL);
  glMemoryBarrier(~0u);  // Wait for all possible shader memory transactions.
  PrintOpenglError();
  glUseProgram(0);
  
  
  // Read the slicemap back in from texture memory.
  glBindTexture(GL_TEXTURE_3D, slicemap_texture_name_);
  PrintOpenglError();
  glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                slicemap_.data());
  PrintOpenglError();
  glBindTexture(GL_TEXTURE_3D, 0);
  
  return slicemap_;
}
