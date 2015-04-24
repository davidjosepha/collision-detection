#include "multi_component_occmap.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <new>  // For bad_alloc exception.
#include <set>

#include "geom_3d.h"

using namespace std;

unsigned int MultiComponentOccmap::GRADIENT_STEPS = 11;
char const * MultiComponentOccmap::GRADIENT = " .:;~=xX&@$";

MultiComponentOccmap::MultiComponentOccmap()
    : data_(NULL), size_(0), reserved_size_(0), x_(0), y_(0), z_(0),
      num_component_slots_(0), denominator_(0) {}

MultiComponentOccmap::MultiComponentOccmap(MultiComponentOccmap const & rhs)
    : data_(NULL), size_(0), reserved_size_(0), x_(0), y_(0), z_(0),
      num_component_slots_(0), denominator_(0) {
  Resize(rhs.x_, rhs.y_, rhs.z_, rhs.num_component_slots_);
  SetDenominator(rhs.denominator_);
  memcpy(data_, rhs.data_, rhs.size_ * sizeof(rhs.data_[0]));
}

MultiComponentOccmap::MultiComponentOccmap(unsigned int X, unsigned int Y,
                                           unsigned int Z, unsigned int C,
                                           unsigned int denominator)
    : data_(NULL), size_(0), reserved_size_(0), x_(0), y_(0), z_(0),
      num_component_slots_(0), denominator_(0) {
  Resize(X, Y, Z, C);
  SetDenominator(denominator);
}

MultiComponentOccmap::~MultiComponentOccmap() {
  delete[] data_;
}

bool MultiComponentOccmap::Resize(unsigned int X, unsigned int Y,
                                  unsigned int Z, unsigned int C) {
  x_ = X;
  y_ = Y;
  z_ = Z;
  num_component_slots_ = C;
  size_ = X*Y*Z*C;
  
  // If we already allocated some data, but we now need more space, delete and
  // start over.
  if(data_ != NULL && size_ > reserved_size_) {
    delete[] data_;
    data_ = NULL;
  }
  
  // Now if the data pointer is not null, we know we've already allocated enough
  // space, so we're done.
  if(data_ != NULL) {
    return true;
  }
  
  // Otherwise, we need to allocate a new block of memory.
  try {
    data_ = new Overlap[size_];
    reserved_size_ = size_;
  } catch(std::bad_alloc & err) {
    printf("Failed to allocate a MultiComponentOccmap with %u elements, "
           "according to requested size %u x %u x %u x %u.\n",
           size_, X, Y, Z, C);
    delete[] data_;
    data_ = NULL;
    x_ = 0;
    y_ = 0;
    z_ = 0;
    num_component_slots_ = 0;
    size_ = 0;
    reserved_size_ = 0;
    return false;
  }
  return true;
}

void MultiComponentOccmap::SetDenominator(unsigned int denominator) {
  denominator_ = denominator;
  num_alpha_bits_ = 0;
  while(denominator > 0) {
    denominator >>= 1;
    ++num_alpha_bits_;
  }
}

void MultiComponentOccmap::Clear() {
  memset(data_, 0, size_ * sizeof(Overlap));
}

void MultiComponentOccmap::ClearRegion(UIVec const & corner,
                                       UIVec const & num_voxels) {
  //printf("MultiComponentOccmap::ClearRegion((%s),(%s))\n", corner.AsString().c_str(), num_voxels.AsString().c_str());
  UIVec vx = ComponentwiseMin(num_voxels, ModelSpaceSize() - corner);
  // Move to bottom corner of the ROI to be cleared.
  Overlap * del = data_ +
    num_component_slots_*(corner.x + x_*(corner.y + y_*corner.z));
  // x_length bytes must be zeroed out in each row at constant Y and Z.
  unsigned int x_length = num_component_slots_ * vx.x * sizeof(Overlap);
  // y_skip is the linear pointer offset to increment y.
  unsigned int y_skip = num_component_slots_ * x_;
  // z_skip goes from (0,0,c.y+n.y,z) --- the beginning of the row just after
  // the ROI in plane z --- to (0,0,c.y,z+1) --- the beginning of the row where
  // the ROI starts in plane z+1.
  unsigned int z_skip = y_skip * (y_ - vx.y);
  
  for(unsigned int i_z = 0; i_z < vx.z; ++i_z) {
    for(unsigned int i_y = 0; i_y < vx.y; ++i_y) {
      memset(del, 0, x_length);
      // Move to the beginning of the next row.
      del += y_skip;
    }
    // y_skip + z_skip moves from the beginning of the last row of the ROI to
    // the beginning of the first row of the ROI in the next plane.
    del += z_skip;
  }
}

MultiComponentOccmap::Overlap const & MultiComponentOccmap::At(unsigned int x,
                                                               unsigned int y,
                                                               unsigned int z,
                                                               unsigned int c)
                                                               const {
  /*return data_[c + x*num_component_slots_ + y*x_*num_component_slots_ +
               z*x_*y_*num_component_slots_];*/
  return data_[c + num_component_slots_*(x + x_*(y + y_*z))];
}
MultiComponentOccmap::Overlap & MultiComponentOccmap::At(unsigned int x,
                                                         unsigned int y,
                                                         unsigned int z,
                                                         unsigned int c) {
  /*return data_[c + x*num_component_slots_ + y*x_*num_component_slots_ +
               z*y_*x_*num_component_slots_];*/
  return data_[c + num_component_slots_*(x + x_*(y + y_*z))];
}

void MultiComponentOccmap::Print() const {
  for(unsigned int c = 0; c < num_component_slots_; ++c) {
    // First we need to find all the different IDs in this slot.
    set<uint16_t> ids;
    Overlap const * o;
    for(unsigned int z = 0; z < z_; ++z) {
      for(unsigned int y = 0; y < y_; ++y) {
        for(unsigned int x = 0; x < x_; ++x) {
          o = &At(x, y, z, c);
          if(o->Alpha(num_alpha_bits_) > 0) {
            //printf("slot %u, %3u %3u %3u: %5u %2u\n", c, x, y, z,
            //       o->Id(num_alpha_bits_), o->Alpha(num_alpha_bits_));
            ids.insert(o->Id(num_alpha_bits_));
          }
        }
      }
    }
    if(!ids.empty()) {
      printf("========== Component slot %u ==========\n", c);
      for(set<uint16_t>::iterator it = ids.begin(); it != ids.end(); ++it) {
        printf("  ----==== Component %u ====----\n", (unsigned int)(*it));
        for(unsigned int y = 0; y < y_; ++y) {
          bool y_occupied = false;
          for(unsigned int x = 0; x < x_; ++x) {
            bool x_occupied = false;
            for(unsigned int z = 0; z < z_; ++z) {
              o = &At(x, y, z, c);
              if(o->Alpha(num_alpha_bits_) > 0 &&
                 o->Id(num_alpha_bits_) == *it) {
                x_occupied = true;
                y_occupied = true;
                break;
              }
            }
            if(x_occupied) {
              printf("(%3u,%3u)", x, y);
              for(unsigned int z = 0; z < z_; ++z) {
                if(At(x, y, z, c).Id(num_alpha_bits_) != *it) {
                  putchar(GRADIENT[0]);
                } else {
                  uint16_t alpha = At(x, y, z, c).Alpha(num_alpha_bits_);
                  if(alpha == 0) {
                    putchar(GRADIENT[0]);
                  } else if(alpha == denominator_) {
                    putchar(GRADIENT[GRADIENT_STEPS-1]);
                  } else {
                    putchar(GRADIENT[1 + (alpha-1)*(GRADIENT_STEPS-2) /
                                    (denominator_-1)]);
                  }
                }
              }
              printf("\n");
            }
          }
          if(y_occupied) {
            printf("-------\n");
          }
        }
      }
    }
  }
}

bool MultiComponentOccmap::Write(char const * filename) const {
  FILE * fid = fopen(filename, "wb");
  if(fid == NULL) {
    printf("Could not open %s for writing.\n", filename);
    return false;
  }
  fwrite(data_,
         sizeof(data_[0]),
         size_,
         fid);
  fclose(fid);
  return true;
}
