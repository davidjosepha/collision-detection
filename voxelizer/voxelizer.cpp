#include "voxelizer.h"

#if DEBUG
# define TIMING 0
#endif

#include <stdio.h>
#include <string.h>   // For memset().
#if TIMING
# include <time.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <gl_compute/gl_helper.h>
#include <gl_compute/glx_data.h>

#include "geom_3d.h"
#include "id_mesh.h"
#include "model_space.h"
#include "multi_component_slicemap.h"
#include "multi_component_slicemapper.h"
#include "multi_component_occmapper.h"
#include "watertight_component.h"

using namespace std;

Voxelizer::Voxelizer(GLXData & glx_context) {
  slicer_ = new MultiComponentSlicemapper(glx_context);
  occer_ = new MultiComponentOccmapper(glx_context);
  boxes_ = NULL;
  boxes_allocated_ = 0;
  magnification_ = 1;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_3d_texture_dim_);
}

Voxelizer::~Voxelizer() {
  delete slicer_;
  delete occer_;
  delete[] boxes_;
}

bool Voxelizer::SetWholeVolume(UIVec const & size, unsigned int magnification) {
  // The shader represents the occmap as a 3D image with X-size equal to the
  // image X-size multiplied by the number of occmap slots.  So we pick the
  // number of slots to max out this dimension.
  return SetWholeVolume(size, magnification, max_3d_texture_dim_/size.x);
}
  
bool Voxelizer::SetWholeVolume(UIVec const & size,
                               unsigned int magnification,
                               unsigned int num_occmap_slots) {
  if(num_occmap_slots > max_3d_texture_dim_/size.x) {
    printf("        Voxelizer::SetWholeVolume(): too many occmap slots requested.\n"
           "Using %u instead of requested %u.\n", max_3d_texture_dim_/size.x,
           num_occmap_slots);
    num_occmap_slots = max_3d_texture_dim_/size.x;
  }
  
  magnification_ = magnification;
  size_ = size;
  size_int_ = IVec(size.x, size.y, size.z);
  size_float_ = Vec(size.x, size.y, size.z);
  
  return occer_->SetFinalBox(size, magnification_, num_occmap_slots);
}

void Voxelizer::SetModelMap(ModelMap const & model_map) {
  model_map_ = &model_map;
}

bool Voxelizer::RenderAll() {
  return RenderRegion(UIVec(0,0,0),
     ModelSpace::box_size * size_.CeilDivide(ModelSpace::box_size));
}

bool Voxelizer::RenderRegion(UIPoint const & corner, UIVec const & size) {
  // Set up the subvolume boxes to correspond to the model map's box size and
  // the region's total size.
  num_boxes_ = size.CeilDivide(ModelSpace::box_size);
  unsigned int num_boxes_to_allocate = num_boxes_.Product();
  if(num_boxes_to_allocate > boxes_allocated_) {
    delete[] boxes_;
    boxes_ = new unsigned int[num_boxes_to_allocate];
    boxes_allocated_ = num_boxes_to_allocate;
  }
  ResetSubvolumeRenderState();
  
  // Iterate through the subvolumes and render them.
  IDMesh mesh;
  vector<unsigned int> id_map;
  ComponentSet comps;
  UIPoint subvolume_low_corner;
  UIVec subvolume_size;
  Vec   subvolume_size_float;
  UIVec image_size;
  
#if TIMING
  clock_t t_start, t_end;
  long long int t_diff;
  long long int ms = 1000LL;
  long long int cps = CLOCKS_PER_SEC;
#endif
  
  while(GetNextSubvolume(corner, comps, subvolume_low_corner, subvolume_size)) {
    //if(subvolume_id_ - 1 > NUM_SUBVOLUMES) { break; }
    //PrintCoverageVolume();
    
    image_size = subvolume_size * magnification_;
    Point subvolume_low_corner_f(subvolume_low_corner.x,
                                 subvolume_low_corner.y,
                                 subvolume_low_corner.z);
    Vec subvolume_size_f(subvolume_size.x, subvolume_size.y, subvolume_size.z);
    
#if DEBUG
    printf("        Voxelizing (%s)--(%s), %zu components.\n",
           subvolume_low_corner.AsString().c_str(),
           (subvolume_low_corner + subvolume_size).AsString().c_str(),
           comps.size());
#endif
    
#if TIMING
    printf("        -Calling slicer_->SetBox((%s),(%s),(%s),%zu),\n    for a subvolume high corner of (%s).\n", subvolume_low_corner_f.AsString().c_str(), subvolume_size_f.AsString().c_str(), image_size.AsString().c_str(), comps.size(), (subvolume_low_corner + subvolume_size).AsString().c_str());
    t_start = clock();
#endif
    if(!slicer_->SetBox(subvolume_low_corner_f, subvolume_size_f, image_size,
                        comps.size())) {
      printf("        Error with call slicer_->SetBox((%s),(%s),(%s),%zu).\n",
             subvolume_low_corner_f.AsString().c_str(),
             subvolume_size_f.AsString().c_str(),
             image_size.AsString().c_str(),
             comps.size());
      return false;
    }
#if TIMING
    t_end = clock(); t_diff = ((long long int)(t_end-t_start)*ms)/cps; printf("        -Time: %lld.%03lld s\n\n", t_diff/ms, t_diff%ms);
    
    printf("        -Setting up the ID map.\n");
    t_start = clock();
#endif
    // Assign fake IDs in the arbitrary order of the component set, and add the
    // block triangles to a mesh using the fake IDs.
    id_map.reserve(comps.size());
    id_map.clear();
    mesh.Clear();
    unsigned int fake_id = 0;
    for(ComponentSet::iterator it = comps.begin(); it != comps.end(); ++it) {
      id_map.push_back((*it)->Id());
      (*it)->AddTrianglesToMesh(mesh, fake_id++);
    }
#if TIMING
    t_end = clock(); t_diff = ((long long int)(t_end-t_start)*ms)/cps; printf("        -Time: %lld.%03lld s\n\n", t_diff/ms, t_diff%ms);
    
    printf("        -Voxelizing.\n");
    t_start = clock();
#endif
    slicer_->Voxelize(mesh);
#if TIMING
    t_end = clock(); t_diff = ((long long int)(t_end-t_start)*ms)/cps; printf("        -Time: %lld.%03lld s\n\n", t_diff/ms, t_diff%ms);
    //slicer_->slicemap().Print();
    
    printf("        -Occmapping.\n");
    //printf("        Calling occer_->SetBox((%s),(%s)),\n    for a subvolume high corner of (%s).\n", subvolume_low_corner.AsString().c_str(), subvolume_size.AsString().c_str(), (subvolume_low_corner + subvolume_size).AsString().c_str());
    t_start = clock();
#endif
    occer_->SetBox(subvolume_low_corner, subvolume_size);
    //printf("        Begin occer_->Render()\n");
    occer_->Render(slicer_->slicemap(), id_map);
#if TIMING
    t_end = clock(); t_diff = ((long long int)(t_end-t_start)*ms)/cps; printf("        -Time: %lld.%03lld s\n\n", t_diff/ms, t_diff%ms);
#endif
    //occer_->occmap().Print();
  }
  return true;
}

void Voxelizer::ResetSubvolumeRenderState() {
  memset(boxes_, 0, boxes_allocated_ * sizeof(unsigned int));
  corner_plane_ = 0;
  corner_box_ = UIVec(0,0,0);
  subvolume_id_ = 1;
}

bool Voxelizer::GetNextSubvolume(UIPoint const & region_corner,
                                 ComponentSet & comps_to_render,
                                 UIPoint & corner,
                                 UIVec & size) {
  ModelMap::const_iterator map_lookup;
  //printf("        ============== Finding the next SV ==============\n");
  // Set the low corner to every combination of indices in the volume, starting
  // at (0,0,0) and scanning outward within parallel planes for which the sum
  // of the coordinatees is constant.  (These planes are orthogonal to the
  // vector <1,1,1>.)  Since each element of num_boxes_ is one larger than the
  // maximum coordinate in that dimension, the open bound on the sum is two
  // lower.
  // 
  // Here we use while-loops with persistent state (stored in data members)
  // rather than "for"s so that we don't restart from the low corner on every
  // call to get the next subvolume.  Instead we pick up where we left off.
  while(corner_plane_ < num_boxes_.Sum() - 2) {
  while(corner_box_.z < min(corner_plane_+1,               num_boxes_.z)) {
  while(corner_box_.y < min(corner_plane_+1-corner_box_.z, num_boxes_.y)) {
    corner_box_.x = corner_plane_ - corner_box_.y - corner_box_.z;
    // Skip this iteration if the box is out of bounds or already rendered.
    if(corner_box_.x >= num_boxes_.x || RenderedAt(corner_box_) > 0) {
      // Continue along the X-Y scan line at constant Z.
      ++(corner_box_.y);
      continue;
    }
    
    // We've found an open spot to render; now we grow the subvolume to fill a
    // rectangular region of unrendered boxes, or to use as much texture memory
    // as we can.  Regardless, at this point we know we're going to render
    // *something* (unless this single box contains so many blocks that we can't
    // render them all at once).
    //printf("        SV low corner: %s\n", corner_box_.AsString().c_str());
    UIVec corner_coord = region_corner + corner_box_ * ModelSpace::box_size;
    
    // Find the blocks in the box with this corner.
    map_lookup = model_map_->find(corner_coord);
    if(map_lookup != model_map_->end()) {
      comps_to_render = map_lookup->second;
    } else {
      comps_to_render.clear();
    }
    
    //printf("        %zu comps in the singleton box.\n", comps_to_render.size());
    
    UIVec sv_num_boxes(1,1,1);
    UIVec slicemap_size =
      ComponentwiseMin(ModelSpace::box_size, size_ - corner_coord) *
      magnification_;
    slicemap_size.z = CeilDivide(
      slicemap_size.z, MultiComponentSlicemap::NUM_BITS_PER_BITFIELD)
      * comps_to_render.size();
    //printf("        Singleton box slicemap size: %s\n", slicemap_size.AsString().c_str());
    if(slicemap_size.x > max_3d_texture_dim_ ||
       slicemap_size.y > max_3d_texture_dim_ ||
       slicemap_size.z > max_3d_texture_dim_ ||
       slicemap_size.Product() > MAX_SLICEMAP_SIZE) {
      // TODO: If this comes up often enough in realistic scenarios, the
      // voxelizer should know how to do a subset of the components.
      printf("        Error!  A single box is too big for the slicemap!\n"
             "%u x %u x %u boxes @ %ux magnification w/ %zu blocks => "
             " slicemap size %u x %u x %u.\n", 1, 1, 1, magnification_,
             comps_to_render.size(), slicemap_size.x, slicemap_size.y,
             slicemap_size.z);
      printf("        max_3d_texture_dim_ = %u\n", max_3d_texture_dim_);
      printf("        MAX_SLICEMAP_SIZE = %u, slicemap_size.Product() = %u\n",
             MAX_SLICEMAP_SIZE, slicemap_size.Product());
    }
    ComponentSet comps_to_render_prev;
    bool growing_x = true;
    bool growing_y = true;
    bool growing_z = true;
    unsigned int try_dim = 2;
    while(growing_x || growing_y || growing_z) {
      try_dim = (try_dim + 1) % 3;
      
      // Try growing in the current dimension.
      bool grew = false;
      switch(try_dim) {
        case 0: {
          if(growing_x) {
            if(corner_box_.x + sv_num_boxes.x < num_boxes_.x) {
              sv_num_boxes.x += 1;
              grew = true;
              //printf("          Grew in x: %s\n", sv_num_boxes.AsString().c_str());
            } else {
              growing_x = false;
            }
          }
          break;
        }
        case 1: {
          if(growing_y) {
            if(corner_box_.y + sv_num_boxes.y < num_boxes_.y) {
              sv_num_boxes.y += 1;
              grew = true;
              //printf("          Grew in y: %s\n", sv_num_boxes.AsString().c_str());
            } else {
              growing_y = false;
            }
          }
          break;
        }
        case 2: {
          if(growing_z) {
            if(corner_box_.z + sv_num_boxes.z < num_boxes_.z) {
              sv_num_boxes.z += 1;
              grew = true;
              //printf("          Grew in z: %s\n", sv_num_boxes.AsString().c_str());
            } else {
              growing_z = false;
            }
          }
          break;
        }
      }
      if(!grew) {
        // Skip the rest and try to grow in the next dimension.
        continue;
      }
      
      // Check to see whether growing in this dimension would overlap an
      // already-rendered SV.
      bool overlap = false;
      UIVec delta;
      for(delta.z = 0; !overlap && delta.z < sv_num_boxes.z; ++delta.z) {
      for(delta.y = 0; !overlap && delta.y < sv_num_boxes.y; ++delta.y) {
      for(delta.x = 0; !overlap && delta.x < sv_num_boxes.x; ++delta.x) {
        if(RenderedAt(corner_box_ + delta) > 0) {
          //printf("          But this overlaps previously-rendered box %s.\n", (corner_box_ + delta).AsString().c_str());
          overlap = true;
        }
      }
      }
      }
      if(overlap) {
        // Undo the growth.
        switch(try_dim) {
          case 0: { sv_num_boxes.x -= 1; growing_x = false; break; }
          case 1: { sv_num_boxes.y -= 1; growing_y = false; break; }
          case 2: { sv_num_boxes.z -= 1; growing_z = false; break; }
        }
        continue;
      }
      
      // Count the number of blocks in this subvolume.
      // TODO: prospective incremental additions to the comp set, rather than
      // rebuilding the entire comp set each time.
      comps_to_render_prev = comps_to_render;
      comps_to_render.clear();
      UIVec new_corner = corner_coord;
      UIVec limit = corner_coord + (sv_num_boxes * ModelSpace::box_size);
      for(new_corner.z = corner_coord.z; new_corner.z < limit.z;
          new_corner.z += ModelSpace::box_size.z) {
      for(new_corner.y = corner_coord.y; new_corner.y < limit.y;
          new_corner.y += ModelSpace::box_size.y) {
      for(new_corner.x = corner_coord.x; new_corner.x < limit.x;
          new_corner.x += ModelSpace::box_size.x) {
        map_lookup = model_map_->find(new_corner);
        if(map_lookup != model_map_->end()) {
          comps_to_render.insert(map_lookup->second.begin(), map_lookup->second.end());
        }
      }
      }
      }
      /*cout << "    " << comps_to_render.size() << " blocks in this SV: ";
      for(ComponentSet::iterator it = comps_to_render.begin(); it != comps_to_render.end(); ++it) {
        cout << (*it)->Id() << " ";
      }
      cout << endl;*/
      slicemap_size = ComponentwiseMin(
        ModelSpace::box_size * sv_num_boxes, size_ - corner_coord)
        * magnification_;
      slicemap_size.z = CeilDivide(
        slicemap_size.z, MultiComponentSlicemap::NUM_BITS_PER_BITFIELD)
        * comps_to_render.size();
      //printf("            %zu comps in this SV.\n", comps_to_render.size());
      //printf("            Slicemap size: %s\n", slicemap_size.AsString().c_str());
      if(slicemap_size.x > max_3d_texture_dim_ ||
         slicemap_size.y > max_3d_texture_dim_ ||
         slicemap_size.z > max_3d_texture_dim_ ||
         slicemap_size.Product() > MAX_SLICEMAP_SIZE) {
        //printf("          But this makes the slicemap too big: %s.\n", slicemap_size.AsString().c_str());
        comps_to_render = comps_to_render_prev;
        switch(try_dim) {
          case 0: { sv_num_boxes.x -= 1; growing_x = false; break; }
          case 1: { sv_num_boxes.y -= 1; growing_y = false; break; }
          case 2: { sv_num_boxes.z -= 1; growing_z = false; break; }
        }
      }
    }
    
    // We've now picked a subvolume.
#if DEBUG
    printf("        --- Begin Subvolume %u ---\n", subvolume_id_);
#endif
    //cout << "SV size: " << sv_num_boxes.AsString()
    //  << " w/ " << comps_to_render.size() << " blocks" << endl;
    // Mark all the boxes with the current SV ID.
    UIVec delta;
    for(delta.z = 0; delta.z < sv_num_boxes.z; ++delta.z) {
    for(delta.y = 0; delta.y < sv_num_boxes.y; ++delta.y) {
    for(delta.x = 0; delta.x < sv_num_boxes.x; ++delta.x) {
      RenderedAt(corner_box_ + delta) = subvolume_id_;
    }
    }
    }
    
    // Set the arguments and prepare for the next call of this function.
    corner = region_corner + corner_box_ * ModelSpace::box_size;
    size = ComponentwiseMin(sv_num_boxes * ModelSpace::box_size,
                            size_ - corner);
    ++subvolume_id_;
    return true;
  }
  // Continue on to the next X-Y scan line, at the next higher Z value.
  corner_box_.y = 0;
  ++(corner_box_.z);
  }
  // Continue on to the next plane.
  corner_box_.z = 0;
  ++corner_plane_;
  }
  // We've scanned through the whole volume, and all the boxes are rendered.
  //printf("        ===============   Nothing left!   ===============\n");
  return false;
}

void Voxelizer::PrintCoverageVolume() {
  unsigned int * read = boxes_;
  unsigned int min_corner = num_boxes_.Product();
  for(unsigned int rd_z = 0; rd_z < num_boxes_.z; ++rd_z) {
    printf("%3u   ", rd_z);
    for(unsigned int hd_x = 0; hd_x < num_boxes_.x; ++hd_x) { printf("%3u ", rd_z + hd_x); };
    printf("\n     ");
    for(unsigned int hd_x = 0; hd_x < num_boxes_.x; ++hd_x) { printf("  / "); };
    printf("\n");
    for(unsigned int rd_y = 0; rd_y < num_boxes_.y; ++rd_y) {
      printf("%3u-", rd_z + rd_y);
      for(unsigned int rd_x = 0; rd_x < num_boxes_.x; ++rd_x) {
        if(*read == 0 && (rd_x + rd_y + rd_z < min_corner)) {
          min_corner = rd_x + rd_y + rd_z;
          printf(" ## ");
          ++read;
        } else if(*read == subvolume_id_ - 1) {
          printf("%3i*", *(read++));
        } else {
          printf("%3i ", *(read++));
        }
      }
      printf(" -%3u\n", rd_z + rd_y + num_boxes_.x - 1);
    }
    printf("\n");
  }
  printf("---------------------------------\n");
}
