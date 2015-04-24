#ifndef __VOXELIZER_H__
#define __VOXELIZER_H__

#include <map>
#include <set>
#include <GL/gl.h>
#include "geom_3d.h"
#include <gl_compute/glx_data.h>
#include "id_mesh.h"
#include "model_space.h"
#include "multi_component_slicemap.h"
#include "multi_component_slicemapper.h"
#include "multi_component_occmap.h"
#include "multi_component_occmapper.h"
#include "watertight_component.h"

// A high-level class for GPU-accelerated multi-component voxelization.  Because
// this pipeline renders multiple components in each pass, it can't fit the
// entire render volume into texture memory.  This class automatically handles
// the subdivision of the volume into manageable pieces, configuring the
// different render stages, and shuttling data between them.
// 
// The voxelizer has no conception of model space; it assumes that all
// coordinates, volume sizes, and volume offsets are measured in units of
// voxels.
// 
// Most notably, this means that the origin of image space (that is, coordinate
// (0,0,0)) is the BOTTOM CORNER of the bottom voxel of the image.  This is
// different than the NIfTI standard (in which (0,0,0) is the center of the
// bottom voxel).
// 
// For an in-depth discussion of the conventions used throughout this project,
// see the documentation at the top of model_space.h.
class Voxelizer {
 public:
  // LoadOpenGlFunctions() and glx_context.SetUpOffscreenRendering() must be
  // called before initializing a Voxelizer.
  Voxelizer(GLXData & glx_context);
  ~Voxelizer();
  
  // The magnification level, M, controls antialiasing; there will be M^3
  // possible levels of overlap measured between a model component and a voxel.
  // This requires 1+floor(3*log2(M)) bits of storage in each occupancy slot,
  // leaving 32-(1+floor(3*log2(M))) bits for storage of component IDs.
  // The two-argument version automatically selects the number of occmap
  // slots to max out the available texture size in OpenGL.
  bool SetWholeVolume(UIVec const & size, unsigned int magnification);
  bool SetWholeVolume(UIVec const & size,
                      unsigned int magnification,
                      unsigned int num_occmap_slots);
  
  // Sets the model map.  This is how the Voxelizer knows what components are in
  // the model (and in particular, what subset of these components is in each
  // render box).  The voxelizer stores a pointer to the model map, so that it
  // automatically sees any changes; whatever is in the model map at the moment
  // that Render*() (below) gets called is precisely what will be rendered.
  //   This means that you only need to call SetModelMap() *once*, even if you
  // modify the model (and therefore the conents of the map) multiple times,
  // and re-render it after multiple modifications.
  //   However, you also *must* call SetModelMap() before you ever call either
  // of the Render*() functions below, or otherwise the voxelizer will segfault
  // (because it will be trying to treat a null pointer as a pointer to a
  // ModelMap).  Presumably, you should also populate the model map before
  // calling Render*.
  //   IN ADDITION, you must set ModelSpace::box_size before calling Render*.
  // This is how the voxelizer knows what subvolumes are covered by each
  // render box, and therefore what the keys in the model map should be.  For
  // more discussion, see model_space.h.
  void SetModelMap(ModelMap const & model_map);
  
  bool RenderAll();
  
  // "corner" and "size" are measured in voxels, but each component must be a
  // multiple of the corresponding component of ModelSpace::box_size.  This
  // condition is not checked.  See model_space.h for discussion.
  bool RenderRegion(UIPoint const & corner, UIVec const & size);
  
  // Render the given region, checking for occupancy overflows that existed on
  // the last pass and re-rendering adjacent regions if necessary.
  // TODO: not yet implemented.
  //bool RenderRegionSafe(UIPoint const & corner, UIVec const & size);
  
  MultiComponentSlicemapper const & Slicemapper() const { return *slicer_; }
  MultiComponentSlicemap const & Slicemap() const { return slicer_->slicemap(); }
  MultiComponentOccmapper const & Occmapper() const { return *occer_; }
  MultiComponentOccmap const & Occmap() const { return occer_->occmap(); }
  unsigned int const * OccmapSlotCounts() const { return occer_->slot_counts(); }
  
 private:
  unsigned int magnification_;
  UIVec size_;
  IVec size_int_;
  Vec size_float_;
  
  ModelMap const * model_map_;
  IDMesh mesh_;
  MultiComponentSlicemapper * slicer_;
  MultiComponentOccmapper * occer_;
  
  // ===========================================================================
  // The following functions and data members have to do with the mechanism that
  // automatically picks the subvolumes to render on each pass.
  
  // Marks all subvolumes as unrendered.
  void ResetSubvolumeRenderState();
  
  // Picks an as-yet-unrendered subvolume and sets the arguments to the set of
  // components overlapping this subvolume, its low corner, and its size.  Marks
  // the chosen subvolume as rendered.  Returns false if all subvolumes are
  // marked as rendered, or on error.
  bool GetNextSubvolume(UIPoint const & corner_offset,
                        ComponentSet & comps_to_render,
                        UIPoint & corner,
                        UIVec & size);
  
  void PrintCoverageVolume();
  
  // Stores the render state of the boxes.
  unsigned int * boxes_;
  unsigned int boxes_allocated_;
  UIVec num_boxes_;
  
  unsigned int & RenderedAt(UIVec const & i) {
    return boxes_[i.x + num_boxes_.x*i.y + num_boxes_.x*num_boxes_.y*i.z];
  }
  
  // The following variables store the current progress of the scan through the
  // subvolume boxes.
  unsigned int corner_plane_;
  UIVec corner_box_;
  unsigned int subvolume_id_;
  
  // OpenGL fails on any 3D texture whose largest dimension is longer than this.
  GLint max_3d_texture_dim_;
  // Max number of 32-bit texels in the slicemap to avoid overrunning texture
  // memory.  This basically needs to be tweaked until slicer_->Voxelize() stops
  // giving out-of-memory errors.
  const static unsigned int MAX_SLICEMAP_SIZE = 100000000;
};

#endif
