#include "multi_component_slicemap.h"

#include <stdio.h>
#include <string.h>  // For memset().

#include <new>  // For bad_alloc exception.

MultiComponentSlicemap::MultiComponentSlicemap()
    : data_(NULL), x_(0), y_(0), num_z_slabs_(0), num_components_(0),
      size_(0), reserved_size_(0) {}

MultiComponentSlicemap::MultiComponentSlicemap(unsigned int X, unsigned int Y,
                                               unsigned int Z, unsigned int C)
    : data_(NULL), x_(0), y_(0), num_z_slabs_(0), num_components_(0),
      size_(0), reserved_size_(0) {
  bool new_alloc;
  Resize(X, Y, Z, C, new_alloc);
}

MultiComponentSlicemap::~MultiComponentSlicemap() {
  delete[] data_;
}

bool MultiComponentSlicemap::Resize(unsigned int X, unsigned int Y,
                                    unsigned int Z, unsigned int C,
                                    bool & new_allocation) {
  //printf("        MultiComponentSlicemap::Resize(%u,%u,%u,%u).\n  Old: Imaged region: %u %u %u %u\n   = Slicemap voxels: %u %u %u = %u (%u reserved)\n", X, Y, Z, C, x_, y_, num_z_slabs_ * NUM_BITS_PER_BITFIELD, num_components_, x_, y_, num_z_slabs_ * num_components_, size_, reserved_size_);
  x_ = X;
  y_ = Y;
  num_components_ = C;
  num_z_slabs_ = Z / NUM_BITS_PER_BITFIELD;
  if(Z % NUM_BITS_PER_BITFIELD != 0) {
    num_z_slabs_ += 1;
  }
  size_ = x_ * y_ * num_z_slabs_ * num_components_;
  new_allocation = false;
  //printf("          New: Imaged region: %u %u %u %u\n   = Slicemap voxels: %u %u %u = %u\n", x_, y_, num_z_slabs_ * NUM_BITS_PER_BITFIELD, num_components_, x_, y_, num_z_slabs_ * num_components_, size_);
  
  try {
    if(size_ > reserved_size_) {
      delete[] data_;
#if DEBUG
      printf("        MultiComponentSlicemap::Resize: allocating %u Bitfields.\n", size_);
#endif
      data_ = new Bitfield[size_];
      reserved_size_ = size_;
      new_allocation = true;
    } else {
      if(size_ > 0 && data_ == NULL) {
        // This shouldn't ever happen; if data_ == NULL then reserved_size_ = 0.
#if DEBUG
        printf("        MultiComponentSlicemap::Resize: allocating %u Bitfields (from NULL) --- this shouldn't ever happen.\n", size_);
#endif
        data_ = new Bitfield[size_];
        reserved_size_ = size_;
        new_allocation = true;
      }
    }
  } catch(std::bad_alloc & err) {
    printf("        Failed to allocate a MultiComponentSlicemap with %u elements,"
           " according to requested size %u x %u x %u x %u.\n",
           size_, X, Y, Z, C);
    delete[] data_;
    x_ = 0;
    y_ = 0;
    num_z_slabs_ = 0;
    num_components_ = 0;
    size_ = 0;
    reserved_size_ = 0;
    data_ = NULL;
    return false;
  }
  //printf("                  New: %u %u %u %u = %u, (%u)\n", x_, y_, num_components_, num_z_slabs_, size_, reserved_size_);
  return true;
}

void MultiComponentSlicemap::Clear() {
  memset(data_, 0, size_ * sizeof(Bitfield));
}

MultiComponentSlicemap::Bitfield const & MultiComponentSlicemap::At(
    unsigned int x, unsigned int y, unsigned int s, unsigned int c) const {
  return data_[x + x_*y + x_*y_*s + x_*y_*num_z_slabs_*c];
}

MultiComponentSlicemap::Bitfield & MultiComponentSlicemap::At(
    unsigned int x, unsigned int y, unsigned int s, unsigned int c) {
  return data_[x + x_*y + x_*y_*s + x_*y_*num_z_slabs_*c];
}

bool MultiComponentSlicemap::OccupiedAt(unsigned int x, unsigned int y,
                                        unsigned int z, unsigned int c) const {
  unsigned int s = z / NUM_BITS_PER_BITFIELD;
  z %= NUM_BITS_PER_BITFIELD;
  return (At(x,y,s,c) & (1u << z)) > 0;
}

void MultiComponentSlicemap::SetOccupiedAt(unsigned int x, unsigned int y,
                                           unsigned int z, unsigned int c,
                                           bool value) {
  unsigned int s = z / NUM_BITS_PER_BITFIELD;
  z %= NUM_BITS_PER_BITFIELD;
  if(value) {
    At(x,y,s,c) |= (1u << z);
  } else {
    At(x,y,s,c) &= ~(1u << z);
  }
}

void MultiComponentSlicemap::Print() const {
  Bitfield const * check = data_;
  Bitfield const * scan_xy;
  Bitfield const * scan_r;
  unsigned int xy_slab_size = x_ * y_;
  for(unsigned int c = 0; c < num_components_; ++c) {
    // The check pointer scans through the entire data array in storage order,
    // and is guaranteed to point to the beginning of each component's sub-
    // array at this point.
    scan_xy = check;
    // Check whether there's anything in this component.
    bool empty_component = true;
    for(unsigned int s = 0; s < num_z_slabs_; ++s) {
    for(unsigned int y = 0; y < y_; ++y) {
    for(unsigned int x = 0; x < x_; ++x) {
      if(*(check++) != 0) {
        empty_component = false;
      }
    }
    }
    }
    if(empty_component) {
      continue;
    }
    printf("Component %u:\n", c);
    printf("          z =");
    for(unsigned int s = 0; s < num_z_slabs_; ++s) {
      printf("%4u            %4u            ", 32*s, 32*s+16);
    }
    printf("\n              ");
    for(unsigned int s = 0; s < num_z_slabs_; ++s) {
      printf("/               /               ");
    }
    printf("\n");
    for(unsigned int y = 0; y < y_; ++y) {
      bool first_in_y = true;
      for(unsigned int x = 0; x < x_; ++x) {
        // We only print this (x,y) column if it's nonempty, so we check first.
        bool empty_line = true;
        scan_r = scan_xy;
        for(unsigned int s = 0; s < num_z_slabs_; ++s) {
          if(*scan_r != 0) {
            empty_line = false;
            break;
          }
          scan_r += xy_slab_size;
        }
        // Now that we've checked, we need to reset the slab scanner in case we
        // have to print this line.
        scan_r = scan_xy++;
        if(empty_line) {
          continue;
        }
        // Now print all the bitfields in this (x,y) column.
        printf("  (%4u,%4u) ", x, y);
        for(unsigned int s = 0; s < num_z_slabs_; ++s) {
          Bitfield mask = 1;
          for(unsigned int b = 0; b < NUM_BITS_PER_BITFIELD; ++b) {
            if(*scan_r & mask) {
              putchar('X');
            } else {
              if(first_in_y) {
                if(b == 0) {
                  putchar('!');
                } else if(b % 2 == 0) {
                  printf("'");
                } else {
                  putchar('`');
                }
              } else {
                if(b == 0) {
                  putchar(';');
                } else if(b % 2 == 0) {
                  putchar('-');
                } else {
                  putchar(' ');
                }
              }
            }
            mask <<= 1;
          }
          scan_r += xy_slab_size;
        }
        printf("\n");
        first_in_y = false;
      }
    }
  }
}

bool MultiComponentSlicemap::Write(char const * filename) const {
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
