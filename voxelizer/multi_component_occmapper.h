#ifndef __MULTI_COMPONENT_OCCMAPPER_H__
#define __MULTI_COMPONENT_OCCMAPPER_H__

#include <vector>
#include <GL/gl.h>
#include <gl_compute/glx_data.h>
#include "anonymesh.h"
#include "geom_3d.h"
#include "multi_component_occmap.h"
#include "multi_component_slicemap.h"

// Combines the information from several multi-component slicemappers that
// represent different sub-volumes of the model volume into a single native-
// resolution occupancy map.
class MultiComponentOccmapper {
 public:
  // LoadOpenGlFunctions() and glx_context.SetUpOffscreenRendering() must be
  // called before initializing a MultiComponentOccmapper.
  MultiComponentOccmapper(GLXData & glx_context);
  ~MultiComponentOccmapper();
  
  // Sets the size of the full-volume native-resolution occupancy map that will
  // be filled in by subvolumes.  The size of the zoomed image space (of which
  // the slicemaps represent subvolumes) must be exactly zoom * num_voxels.
  // Since components may overlap in the same voxel, the occmap is configured to
  // handle up to max_overlaps components per voxel.
  //   Calling SetFinalBox() zeros out the current contents of the occmap.
  bool SetFinalBox(UIVec const & num_voxels, unsigned int zoom,
                   unsigned int max_overlaps);
  
  // Sets the native-resolution position and size of the subvolume to be written
  // on the next call to Render().  After this call, any slicemap passed in to
  // Render() must represent a volume of zoom*num_voxels at the zoomed
  // resolution.
  bool SetBox(UIVec const & corner, UIVec const & num_voxels);
  
  // Renders the subvolume chosen in SetBox to the occmap.  id_map[i] is the
  // true ID of the block given fake ID i in the slicemap.
  MultiComponentOccmap const & Render(MultiComponentSlicemap const & slicemap,
                                      std::vector<unsigned int> const & id_map);
  
  MultiComponentOccmap const & occmap() const { return occmap_; }
  unsigned int const * slot_counts() const { return slot_counts_; }
  
  // Writes the slot counts (how many components overlapped each voxel) to disk.
  bool WriteSlotCounts(char const * filename) const;
  
  // Returns false (and prints errors) if any voxels have counts higher than
  // the number of slots allocated in the occmap.
  bool CheckSlotCounts() const;
  
 protected:
  MultiComponentOccmap occmap_;
  unsigned int * slot_counts_;
  GLXData & glx_context_;
  
  // Consistent with multi_component_slicemapper.h, this is the maximum number
  // of Z slabs in the slicemap passed in to Render().
  static unsigned int const MAX_Z_SLABS = 32;
  // OpenGL fails on any 3D texture whose largest dimension is longer than this.
  GLint max_3d_texture_dim_;
  
  static char const * SHADER_UNIFORM_NAME_SLICEMAP;
  static char const * SHADER_UNIFORM_NAME_OCCMAP;
  static char const * SHADER_UNIFORM_NAME_OCCMAP_OPEN_SLOTS;
  static char const * SHADER_UNIFORM_NAME_Z_REGION_MASK_LUT;
  static char const * SHADER_UNIFORM_NAME_MASKED_Z_SHIFT_LUT;
  static char const * SHADER_UNIFORM_NAME_ID_MAP;
  static char const * SHADER_UNIFORM_NAME_BOX_POS;
  static char const * SHADER_UNIFORM_NAME_BOX_SIZE;
  static char const * SHADER_UNIFORM_NAME_SLICEMAP_NUM_Z_SLABS;
  static char const * SHADER_UNIFORM_NAME_OCCMAP_NUM_COMPONENT_SLOTS;
  static char const * SHADER_UNIFORM_NAME_OCCMAP_NUM_ALPHA_BITS;
  static char const * SHADER_UNIFORM_NAME_BIT_COUNTER;
  
  Anonymesh full_screen_quad_;
  
  GLuint program_id_;
  GLint  slicemap_image_unit_;
  GLuint slicemap_texture_name_;
  GLint  occmap_image_unit_;
  GLuint occmap_texture_name_;
  GLint  slot_counts_image_unit_;
  GLuint slot_counts_texture_name_;
  GLint  z_region_mask_lut_image_unit_;
  GLuint z_region_mask_lut_texture_name_;
  GLint  masked_z_shift_lut_image_unit_;
  GLuint masked_z_shift_lut_texture_name_;
  GLint  id_map_image_unit_;
  GLuint id_map_texture_name_;
  
  // Zoom values only change on calls to SetFinalBox().  The max native Z size
  // depends on the zoom and determines extent of one of the dimensions of the
  // Z-region-mask LUT, the other dimension being MAX_Z_SLABS.
  unsigned int zoom_;
  unsigned int max_native_z_size_;
  
  // Rendering sub-volume information at native resolution.  These change on
  // each call to SetBox().
  UIVec model_box_corner_;
  UIVec model_box_size_;
  
  // The Z-region mask LUT, of size MAX_Z_SLABS x max_native_z_size_.
  MultiComponentSlicemap::Bitfield * z_region_mask_lut_data_;
  
  // The masked-Z shift LUT, same size as the Z-region mask LUT.
  unsigned int * masked_z_shift_lut_data_;
  
  // Another LUT, this one for counting bits in small numbers.
  GLuint * bit_counter_;
  
  void MakeZRegionLUT();
  void MakeBitCounter();
};

#endif
