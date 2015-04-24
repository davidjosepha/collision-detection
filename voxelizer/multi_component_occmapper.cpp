#include "multi_component_occmapper.h"

#include <stdio.h>
#include <string.h>

#include <GL/gl.h>
#include <gl_compute/gl_helper.h>
#include <gl_compute/glx_data.h>

#include "anonymesh.h"
#include "geom_3d.h"
#include "multi_component_occmap.h"
#include "multi_component_slicemap.h"

// Comment-stripped GLSL source code as a character array.
#include "multi_component_occmapper.glsl.h"

#if DEBUG
# define OCCMAP_OVERFLOW_CHECK 0
#endif

char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_SLICEMAP =
  "slicemap";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_OCCMAP =
  "occmap";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_OCCMAP_OPEN_SLOTS =
  "occmap_open_slots";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_Z_REGION_MASK_LUT =
  "z_region_mask_lut";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_MASKED_Z_SHIFT_LUT =
  "masked_z_shift_lut";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_ID_MAP =
  "id_map";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_BOX_POS =
  "box_pos";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_BOX_SIZE =
  "box_size";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_SLICEMAP_NUM_Z_SLABS =
  "slicemap_num_z_slabs";
char const *
  MultiComponentOccmapper::SHADER_UNIFORM_NAME_OCCMAP_NUM_COMPONENT_SLOTS =
  "occmap_num_component_slots";
char const *
  MultiComponentOccmapper::SHADER_UNIFORM_NAME_OCCMAP_NUM_ALPHA_BITS =
  "occmap_num_alpha_bits";
char const * MultiComponentOccmapper::SHADER_UNIFORM_NAME_BIT_COUNTER =
  "bit_counter";

MultiComponentOccmapper::MultiComponentOccmapper(GLXData & glx_context)
    : slot_counts_(NULL), glx_context_(glx_context),
      program_id_(0),
      slicemap_image_unit_(          0), slicemap_texture_name_(0),
      occmap_image_unit_(            1), occmap_texture_name_(0),
      slot_counts_image_unit_(       2), slot_counts_texture_name_(0),
      z_region_mask_lut_image_unit_( 3), z_region_mask_lut_texture_name_(0),
      masked_z_shift_lut_image_unit_(4), masked_z_shift_lut_texture_name_(0),
      id_map_image_unit_(            5), id_map_texture_name_(0),
      z_region_mask_lut_data_(NULL), masked_z_shift_lut_data_(NULL),
      bit_counter_(NULL) {
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_3d_texture_dim_);
  
  // Generate textures for the slicemap (input) and occupancy map (output).
  // We'll load the image data into them in Render().
  glGenTextures(1, &slicemap_texture_name_); PrintOpenglError();
  glGenTextures(1, &occmap_texture_name_); PrintOpenglError();
  glGenTextures(1, &slot_counts_texture_name_); PrintOpenglError();
  glGenTextures(1, &z_region_mask_lut_texture_name_); PrintOpenglError();
  glGenTextures(1, &masked_z_shift_lut_texture_name_); PrintOpenglError();
  glGenTextures(1, &id_map_texture_name_); PrintOpenglError();
  
  // Set up the full-screen quad (actually two triangles).
  full_screen_quad_.verts.push_back(Vec(0, 0, 0));
  full_screen_quad_.verts.push_back(Vec(0, 0, 0));
  full_screen_quad_.verts.push_back(Vec(0, 0, 0));
  full_screen_quad_.verts.push_back(Vec(0, 0, 0));
  full_screen_quad_.tris.push_back(0);
  full_screen_quad_.tris.push_back(1);
  full_screen_quad_.tris.push_back(2);
  full_screen_quad_.tris.push_back(0);
  full_screen_quad_.tris.push_back(2);
  full_screen_quad_.tris.push_back(3);
}

MultiComponentOccmapper::~MultiComponentOccmapper() {
  delete[] slot_counts_;
  delete[] bit_counter_;
  delete[] z_region_mask_lut_data_;
  delete[] masked_z_shift_lut_data_;
  glDeleteTextures(1, &slicemap_texture_name_);
  glDeleteTextures(1, &occmap_texture_name_);
  glDeleteTextures(1, &slot_counts_texture_name_);
  glDeleteTextures(1, &z_region_mask_lut_texture_name_);
  glDeleteTextures(1, &masked_z_shift_lut_texture_name_);
  glDeleteTextures(1, &id_map_texture_name_);
  glDeleteProgram(program_id_);
}

bool MultiComponentOccmapper::SetFinalBox(UIVec const & num_voxels,
                                          unsigned int zoom,
                                          unsigned int max_overlaps) {
  //printf("MultiComponentOccmapper::SetFinalBox((%s), %u, %u)\n",
  //       num_voxels.AsString().c_str(), zoom, max_overlaps);
  
  zoom_ = zoom;
  
  // Set up the occmap.
  if(num_voxels.x * max_overlaps > max_3d_texture_dim_ ||
     num_voxels.y > max_3d_texture_dim_ ||
     num_voxels.z > max_3d_texture_dim_) {
    printf("MultiComponentOccmapper::SetFinalBox:\n"
           "  Requested image size %s with %u overlap slots\n"
           "  is too big for OpenGl.\n",
           num_voxels.AsString().c_str(), max_overlaps);
    return false;
  }
  occmap_.Resize(num_voxels.x, num_voxels.y, num_voxels.z, max_overlaps);
  occmap_.SetDenominator(zoom_*zoom_*zoom_);
  occmap_.Clear();
  //printf("occmap_.size() = %u\n", occmap_.size());
  //printf("occmap_.x() = %u\n", occmap_.x());
  //printf("occmap_.y() = %u\n", occmap_.y());
  //printf("occmap_.z() = %u\n", occmap_.z());
  //printf("occmap_.num_component_slots() = %u\n", occmap_.num_component_slots());
  //printf("occmap_.denominator() = %u\n", occmap_.denominator());
  
  delete[] slot_counts_;
  slot_counts_ = new unsigned int[num_voxels.x*num_voxels.y*num_voxels.z];
  memset(slot_counts_, 0,
         num_voxels.x*num_voxels.y*num_voxels.z*sizeof(slot_counts_[0]));
  
  MakeZRegionLUT();
  MakeBitCounter();
  
  // Compile the program.
  glDeleteProgram(program_id_);
  char define_zoom_line[64];
  sprintf(define_zoom_line, "#define ZOOM %u", zoom_);
  program_id_ = LoadOpenGlShaderProgramFromSource(
    reinterpret_cast<char const *>(multi_component_occmapper_glsl_src),
    multi_component_occmapper_glsl_src_len, define_zoom_line);
  if(PrintOpenglError()) {
    return false;
  }
  
  return true;
}

bool MultiComponentOccmapper::SetBox(UIVec const & corner,
                                     UIVec const & num_voxels) {
  model_box_corner_ = corner;
  model_box_size_ = ComponentwiseMin(num_voxels,
                                     occmap_.ModelSpaceSize()-corner);
  
  // Clear out the region in the occmap and the slot-counts array.
  occmap_.ClearRegion(model_box_corner_, model_box_size_);
  
  // Move to bottom corner of the ROI to be cleared in slot_counts_.
  unsigned int * del = slot_counts_ + model_box_corner_.x +
    occmap_.x()*(model_box_corner_.y + occmap_.y()*model_box_corner_.z);
  // x_length bytes must be zeroed out in each row at constant Y and Z.
  unsigned int x_length = model_box_size_.x * sizeof(slot_counts_[0]);
  // y_skip is the linear pointer offset to increment y.
  unsigned int y_skip = occmap_.x();
  // z_skip goes from (0,0,c.y+n.y,z) --- the beginning of the row just after
  // the ROI in plane z --- to (0,0,c.y,z+1) --- the beginning of the row where
  // the ROI starts in plane z+1.
  unsigned int z_skip = y_skip * (occmap_.y() - model_box_size_.y);
  
  for(unsigned int i_z = 0; i_z < model_box_size_.z; ++i_z) {
    for(unsigned int i_y = 0; i_y < model_box_size_.y; ++i_y) {
      memset(del, 0, x_length);
      // Move to the beginning of the next row.
      del += y_skip;
    }
    // y_skip + z_skip moves from the beginning of the last row of the ROI to
    // the beginning of the first row of the ROI in the next plane.
    del += z_skip;
  }
  
  // Resize the full-screen quad.  All other coordinates remain zero.
  full_screen_quad_.verts[1].x = num_voxels.x;
  full_screen_quad_.verts[2].x = num_voxels.x;
  full_screen_quad_.verts[2].y = num_voxels.y;
  full_screen_quad_.verts[3].y = num_voxels.y;
  return true;
}


MultiComponentOccmap const & MultiComponentOccmapper::Render(
    MultiComponentSlicemap const & slicemap,
    std::vector<unsigned int> const & id_map) {
  if(slicemap.size() == 0) {
    return occmap_;
  }
  
  glUseProgram(program_id_); PrintOpenglError();
  glx_context_.SetViewportSize(model_box_size_.x, model_box_size_.y);
  
  // Set the values for the regular shader uniforms.
  // We lie to the shader and tell it that the Z-position of the box is 0,
  // because we only transfer the portion of the occmap that has Z in the box.
  glUniform3i(glGetUniformLocation(program_id_,
                                   SHADER_UNIFORM_NAME_BOX_POS),
              float(model_box_corner_.x), float(model_box_corner_.y), 0.0);
  glUniform3f(glGetUniformLocation(program_id_,
                                   SHADER_UNIFORM_NAME_BOX_SIZE),
              float(model_box_size_.x), float(model_box_size_.y),
              float(model_box_size_.z));
  glUniform1ui(glGetUniformLocation(program_id_,
                                    SHADER_UNIFORM_NAME_SLICEMAP_NUM_Z_SLABS),
               1 + (model_box_size_.z*zoom_ - 1) /
               MultiComponentSlicemap::NUM_BITS_PER_BITFIELD);
  glUniform1ui(glGetUniformLocation(program_id_,
                 SHADER_UNIFORM_NAME_OCCMAP_NUM_COMPONENT_SLOTS),
               occmap_.num_component_slots());
  glUniform1ui(glGetUniformLocation(program_id_,
               SHADER_UNIFORM_NAME_OCCMAP_NUM_ALPHA_BITS),
               occmap_.num_alpha_bits());
  glUniform1uiv(glGetUniformLocation(program_id_,
                                     SHADER_UNIFORM_NAME_BIT_COUNTER),
                1u << zoom_,
                bit_counter_);
  PrintOpenglError();
  
  // Copy image data into textures and attach them to the right uniforms.
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_SLICEMAP,
                       slicemap.data(),
                       GL_READ_ONLY,
                       GL_UNSIGNED_INT,
                       program_id_,
                       slicemap_texture_name_,
                       slicemap_image_unit_,
                       slicemap.x(),
                       slicemap.y(),
                       slicemap.num_z_slabs() * slicemap.num_components());
  PrintOpenglError();
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_Z_REGION_MASK_LUT,
                       z_region_mask_lut_data_,
                       GL_READ_ONLY,
                       GL_UNSIGNED_INT,
                       program_id_,
                       z_region_mask_lut_texture_name_,
                       z_region_mask_lut_image_unit_,
                       MAX_Z_SLABS,
                       max_native_z_size_);
  PrintOpenglError();
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_MASKED_Z_SHIFT_LUT,
                       masked_z_shift_lut_data_,
                       GL_READ_ONLY,
                       GL_UNSIGNED_INT,
                       program_id_,
                       masked_z_shift_lut_texture_name_,
                       masked_z_shift_lut_image_unit_,
                       MAX_Z_SLABS,
                       max_native_z_size_);
  PrintOpenglError();
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_ID_MAP,
                       &(id_map[0]),
                       GL_READ_ONLY,
                       GL_UNSIGNED_INT,
                       program_id_,
                       id_map_texture_name_,
                       id_map_image_unit_,
                       id_map.size());
  PrintOpenglError();
  // For the occmap and slot-count images, transfer starting at the subvolume's
  // minimum Z value, and transfer only as much of the images in Z as we need.
  unsigned int occ_array_z_offset = occmap_.x()*occmap_.y()*model_box_corner_.z;
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_OCCMAP,
                       occmap_.data()
                         + occ_array_z_offset * occmap_.num_component_slots(),
                       GL_READ_WRITE,
                       GL_UNSIGNED_INT,
                       program_id_,
                       occmap_texture_name_,
                       occmap_image_unit_,
                       occmap_.x() * occmap_.num_component_slots(),
                       occmap_.y(),
                       model_box_size_.z);
  if(PrintOpenglError()) {
    printf("  ... while trying to set up a %ux%ux%u image for the occmap.\n",
           occmap_.x()*occmap_.num_component_slots(),
           occmap_.y(), model_box_size_.z);
  }
  SetOpenGlShaderImage(SHADER_UNIFORM_NAME_OCCMAP_OPEN_SLOTS,
                       slot_counts_ + occ_array_z_offset,
                       GL_READ_WRITE,
                       GL_UNSIGNED_INT,
                       program_id_,
                       slot_counts_texture_name_,
                       slot_counts_image_unit_,
                       occmap_.x(),
                       occmap_.y(),
                       model_box_size_.z);
  PrintOpenglError();
  
  /*
  printf("  program_id_ = %u\n", program_id_);
  printf("  slicemap_image_unit_ = %i\n", slicemap_image_unit_);
  printf("  slicemap_texture_name_ = %u\n", slicemap_texture_name_);
  printf("  occmap_image_unit_ = %i\n", occmap_image_unit_);
  printf("  occmap_texture_name_ = %u\n", occmap_texture_name_);
  printf("  z_region_mask_lut_image_unit_ = %i\n", z_region_mask_lut_image_unit_);
  printf("  z_region_mask_lut_texture_name_ = %u\n", z_region_mask_lut_texture_name_);
  */
  
  // Draw the full-screen quads as instanced geometry.
  full_screen_quad_.LoadIntoGl();
  glBindVertexArray(full_screen_quad_.vao);
  
  unsigned int num_quads = slicemap.num_components() * model_box_size_.z;
  //printf("Occmapper: drawing %i quads...\n", num_quads);
  glDrawElementsInstanced(GL_TRIANGLES, full_screen_quad_.tris.size(),
                          GL_UNSIGNED_INT, NULL, num_quads);
  glMemoryBarrier(~0u);  // Wait for all possible shader memory transactions.
  PrintOpenglError();
  glUseProgram(0);
  
  // Read the updated occmap back in.
  //printf("Downloading occmap from graphics memory.\n");
  glBindTexture(GL_TEXTURE_3D, occmap_texture_name_);
  PrintOpenglError();
  glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                occmap_.data()
                  + occ_array_z_offset * occmap_.num_component_slots());
  PrintOpenglError();
  glBindTexture(GL_TEXTURE_3D, 0);
  
  // Read the occmap open-slot image back in too.
  //printf("Downloading occmap open slots from graphcs memory.\n");
  glBindTexture(GL_TEXTURE_3D, slot_counts_texture_name_);
  PrintOpenglError();
  glGetTexImage(GL_TEXTURE_3D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
                slot_counts_ + occ_array_z_offset);
  PrintOpenglError();
  glBindTexture(GL_TEXTURE_3D, 0);
  
#if OCCMAP_OVERFLOW_CHECK
  unsigned int * rd = slot_counts_;
  for(unsigned int z = 0; z < occmap_.z(); ++z) {
    for(unsigned int y = 0; y < occmap_.y(); ++y) {
      for(unsigned int x = 0; x < occmap_.x(); ++x) {
        if(x >= model_box_corner_.x &&
           y >= model_box_corner_.y &&
           z >= model_box_corner_.z &&
           x < model_box_corner_.x + model_box_size_.x &&
           y < model_box_corner_.y + model_box_size_.y &&
           z < model_box_corner_.z + model_box_size_.z) {
          if(*rd > occmap_.num_component_slots()) {
            printf("Used %u slots in voxel %u %u %u!\n", *rd, x, y, z);
          }
        }
        ++rd;
      }
    }
  }
#endif
  return occmap_;
}

void MultiComponentOccmapper::MakeZRegionLUT() {
  delete[] z_region_mask_lut_data_;
  delete[] masked_z_shift_lut_data_;
  
  // Generate the Z-region mask LUT to tell the shader which bits to look at
  // when antialiasing.  This is a 2-D image for which the value at (x,y) is the
  // Z region mask in Z-slab x for the native Z value y in the subvolume.
  //   At the same time, generate a LUT of the same size that stores the number
  // of zero bits to the right of the rightmost nonzero bit in each
  // corresponding element of the Z-region mask LUT.  This tells the shader how
  // far to right-shift masked values to bring them into the canonical range of
  // [0, 2^zoom-1], to prepare them for bit-counting.
  unsigned int slab_size = MultiComponentSlicemap::NUM_BITS_PER_BITFIELD;
  max_native_z_size_ = 1 + (MAX_Z_SLABS * slab_size - 1) / zoom_;
  z_region_mask_lut_data_ = new
    MultiComponentSlicemap::Bitfield[MAX_Z_SLABS * max_native_z_size_];
  masked_z_shift_lut_data_ = new unsigned int[MAX_Z_SLABS * max_native_z_size_];
  
  // Since we're not guaranteed that zoom_ evenly divides NUM_BITS_PER_BITFIELD,
  // it's easiest to split the creation of the Z region mask LUT into two steps.
  //   First we set each mask to cover the appropriate region of bits, as well
  // as all higher-order bits.
  MultiComponentSlicemap::Bitfield * region_write = z_region_mask_lut_data_;
  unsigned int * shift_write = masked_z_shift_lut_data_;
  
  for(unsigned int z = 0; z < max_native_z_size_; ++z) {
    unsigned int shift = z * zoom_;
    for(unsigned int s = 0; s < MAX_Z_SLABS; ++s) {
      if(shift >= (s+1) * slab_size) {
        *(region_write++) = 0u;
        *(shift_write++) = 0u;
      } else if(shift < s * slab_size) {
        *(region_write++) = ~(0u);
        *(shift_write++) = 0u;
      } else {
        *(region_write++) = ~(0u) << (shift - s*slab_size);
        *(shift_write++) = shift - s*slab_size;
      }
    }
  }
  // Now we xor each mask with the corresponding mask for the next Z value.
  // This zeros out the higher-order bits, leaving only the region we want.
  region_write = z_region_mask_lut_data_;
  for(unsigned int z = 0; z < max_native_z_size_ - 1; ++z) {
    for(unsigned int s = 0; s < MAX_Z_SLABS; ++s) {
      *region_write ^= *(region_write + MAX_Z_SLABS);
      ++region_write;
    }
  }
}

void MultiComponentOccmapper::MakeBitCounter() {
  delete[] bit_counter_;
  unsigned int num_vals = 1 << zoom_;
  bit_counter_ = new GLuint[num_vals];
  // Base case: no bits are on for 0x0.
  bit_counter_[0] = 0;
  unsigned int num_vals_so_far = 1;
  for(unsigned int pow = 0; pow < zoom_; ++pow) {
    // Each number from 2^pow to 2^(pow+1)-1 (inclusive) has one more bit set
    // than the corresponding number from 0 to 2^pow-1.
    for(unsigned int i = 0; i < num_vals_so_far; ++i) {
      bit_counter_[num_vals_so_far + i] = 1 + bit_counter_[i];
    }
    num_vals_so_far <<= 1;
  }
}

bool MultiComponentOccmapper::WriteSlotCounts(char const * filename) const {
  FILE * fid = fopen(filename, "wb");
  if(fid == NULL) {
    printf("Could not open %s for writing.\n", filename);
    return false;
  }
  fwrite(slot_counts_,
         sizeof(slot_counts_[0]),
         occmap_.x() * occmap_.y() * occmap_.z(),
         fid);
  fclose(fid);
  return true;
}

bool MultiComponentOccmapper::CheckSlotCounts() const {
  bool all_legit = true;
  unsigned int const * c_it = slot_counts_;
  UIVec sz = occmap_.ModelSpaceSize();
  UIVec here;
  for(unsigned int i = 0; i < sz.Product(); ++i) {
    if(*c_it > occmap_.num_component_slots()) {
      all_legit = false;
      here = sz.FromIndex(i);
      printf("MultiComponentOccmapper::CheckSlotCounts : (%s) has %i components!\n",
             here.AsString().c_str(), *c_it);
      printf("                         components here :");
      for(unsigned int c = 0; c < occmap_.num_component_slots(); ++c) {
        printf(" %u", occmap_.At(here.x, here.y, here.z, c).Id(occmap_.num_alpha_bits()));
      }
      printf("\n");
      printf("      slot_counts_ says %u in next voxel :", *(c_it + 1));
      for(unsigned int c = 0; c < occmap_.num_component_slots(); ++c) {
        printf(" %u", occmap_.At(here.x+1, here.y, here.z, c).Id(occmap_.num_alpha_bits()));
      }
      printf("\n");
    }
    ++c_it;
  }
  return all_legit;
}
