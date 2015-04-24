#ifndef __MULTI_COMPONENT_OCCMAP_H__
#define __MULTI_COMPONENT_OCCMAP_H__

#include <stdint.h>
#include "geom_3d.h"

// A 3D occupancy (or overlap) map with multiple components in each voxel.  For
// a given voxel that is partially or fully overlapped by a model component, the
// voxel stores this component's ID and a fixed-point representation of the
// approximate fraction of the voxel's volume overlapped by the model component.
// Each voxel has a fixed number of "component slots" in which it can store
// occupancy data for different components.  It is the caller's responsibility
// to write the data into the proper slots and avoid collisions.
class MultiComponentOccmap {
 public:
  class Overlap {
   public:
    Overlap() : data_(0) {}
    Overlap(unsigned int raw_value) : data_(raw_value) {}
    Overlap(unsigned int id, unsigned int alpha, unsigned int num_alpha_bits) {
      Set(id, alpha, num_alpha_bits);
    }
    void Set(unsigned int id, unsigned int alpha, unsigned int num_alpha_bits) {
      data_ = (id << num_alpha_bits) | alpha;
    }
    unsigned int Id(unsigned int num_alpha_bits) const {
      return data_ >> num_alpha_bits;
    }
    unsigned int Alpha(unsigned int num_alpha_bits) const {
      return data_ & ~((~0u) << num_alpha_bits);
    }
    unsigned int data() const { return data_; }
   private:
    unsigned int data_;
  };
  
  MultiComponentOccmap();
  
  MultiComponentOccmap(MultiComponentOccmap const & rhs);
  
  // Allocates a data array of the proper size, with a fixed denominator for the
  // overlap fractions.  Contents are undefined.  C is the number of component
  // slots to allocate.
  MultiComponentOccmap(unsigned int X, unsigned int Y, unsigned int Z,
                       unsigned int C, unsigned int denominator);
  
  ~MultiComponentOccmap();
  
  // Resizes the data array, scrapping the old contents.  New contents are
  // undefined.  C is the number of component slots to allocate.
  bool Resize(unsigned int X, unsigned int Y, unsigned int Z, unsigned int C);
  
  void SetDenominator(unsigned int denominator);
  
  // Sets all elements to 0.
  void Clear();
  // Sets elements to 0 in the given region.
  void ClearRegion(UIVec const & corner, UIVec const & num_voxels);
  
  // Gets the data array.
  // c is the fastest-changing index, then x, then y, then z.
  Overlap const * data() const { return data_; }
  Overlap * data() { return data_; }
  
  // Gets the total number of Overlaps in the data array.
  unsigned int size() const { return size_; }
  // Gets the size of the modeled space in the x, y, or z direction.
  unsigned int x() const { return x_; }
  unsigned int y() const { return y_; }
  unsigned int z() const { return z_; }
  UIVec ModelSpaceSize() const { return UIVec(x_, y_, z_); }
  // Gets the number of component slots in each voxel.
  unsigned int num_component_slots() const { return num_component_slots_; }
  
  // The maximum alpha value, meaning full occupancy of a voxel.
  unsigned int denominator() const { return denominator_; }
  // The number of bits used to represent the alpha values.
  unsigned int num_alpha_bits() const { return num_alpha_bits_; }
  
  // Gets the overlap value at a particular (x,y,z,c) coordinate, with no
  // bounds-checking.  The alpha value stored within the Overlap struct is the
  // numerator of a fraction; use denominator() to get the denominator.
  Overlap const & At(unsigned int x, unsigned int y, unsigned int z,
                     unsigned int c) const;
  Overlap & At(unsigned int x, unsigned int y, unsigned int z, unsigned int c);
  
  // Prints an ASCII representation of (x,y) columns that are nonzero.
  void Print() const;
  
  // Writes raw data to disk.
  bool Write(char const * filename) const;
  
 private:
  // Static info for the ASCII printer.
  static unsigned int GRADIENT_STEPS;
  static char const * GRADIENT;
  
  Overlap * data_;
  unsigned int size_;
  unsigned int reserved_size_;
  unsigned int x_;
  unsigned int y_;
  unsigned int z_;
  unsigned int num_component_slots_;
  unsigned int denominator_;
  unsigned int num_alpha_bits_;
  
  // Assignment is disallowed.  Use a pointer or reference instead, or, if you
  // really need a copy, use the copy constructor.
  void operator=(MultiComponentOccmap const &);
};

#endif
