#ifndef __MULTI_COMPONENT_SLICEMAP_H__
#define __MULTI_COMPONENT_SLICEMAP_H__

// A compact representation of a 4-dimensional binary array, with dimensions X,
// Y, Z, and C.  The C dimension represents different components of a 3-D model,
// and the value at (x,y,z,c) is meant to indicate the in/out status of the
// point (x,y,z) with respect to component c.
// 
// The internal representation is a 3-D array of 32-bit bitfields.  X is the
// fastest-changing index, followed by Y.  For each component, there are
//          S = ceil(Z / MultiComponentSlicemap::NUM_BITS_PER_BITFIELD)
// repetitions of the basic X-Y slab, concatenated together in the Z dimension.
// The slicemaps for the C components are also concatenated together in Z, so
// the size of the underlying 3-D array is (X,Y,S*C).  This entire array is
// laid out flat in memory.
// 
// Say we want to know the in/out value of the voxel at index (x,y,z) with
// respect to component c.  First we have to compute the index of the
// Z-slab in which to look:
//          s = floor(z / MultiComponentSlicemap::NUM_BITS_PER_BITFIELD).
// We will therefore find the relevant bitfield at index (x,y,S*c+s).  Within
// this bitfield, the in/out value is stored at bit i:
//          i = z % MultiComponentSlicemap::NUM_BITS_PER_BITFIELD.
class MultiComponentSlicemap {
 public:
  typedef unsigned int Bitfield;
  static unsigned int const NUM_BITS_PER_BITFIELD = sizeof(Bitfield) * 8;
  
  MultiComponentSlicemap();
  
  // Allocates a data array of the proper size.  Z will be rounded up to a
  // multiple of NUM_BITS_PER_BITFIELD.  Contents are undefined.
  MultiComponentSlicemap(unsigned int X, unsigned int Y, unsigned int Z,
                         unsigned int C);
  
  // Copying and assignment are disallowed.  Use a pointer or reference
  // instead.
  MultiComponentSlicemap(MultiComponentSlicemap const &);
  void operator=(MultiComponentSlicemap const &);
  
  ~MultiComponentSlicemap();
  
  // Resizes the data array, with Z rounded up to a multiple of
  // NUM_BITS_PER_BITFIELD.  Since the slicemap gets resized frequently, it
  // maintains a contiguous region of "reserved" memory that only ever grows.
  // Therefore the reserved memory for the slicemap may be much larger than the
  // requested size, analogous with the STL's vector implementation.
  //   If the requested size was larger, in total, than the reserved memory, a
  // new region will be allocated and the flag "new_allocation" will be set.  In
  // this case the contents are undefined.  If the reserved memory was
  // sufficient, then the contents will be a linear re-indexing of whatever was
  // stored in the reserved memory previously.
  //   Returns false on allocation failure.
  bool Resize(unsigned int X, unsigned int Y, unsigned int Z, unsigned int C,
              bool & new_allocation);
  
  // Sets all bitfields to zero.
  void Clear();
  
  // Frees all reserved memory and resizes the slicemap to 0 in all dimensions.
  void Free();
  
  // Gets the data array.
  Bitfield const * data() const { return data_; }
  Bitfield * data() { return data_; }
  
  // Gets the total number of Bitfields in the data array.
  unsigned int size() const { return size_; }
  // Gets the size of the slicemap's image space in the x direction, X.
  unsigned int x() const { return x_; }
  // Gets the size of the slicemap's image space in the y direction, Y.
  unsigned int y() const { return y_; }
  // Gets the number of slabs, S.  The total number of distinct z values that
  // are stored in this slicemap is num_z_slabs() * NUM_BITS_PER_BITFIELD.
  unsigned int num_z_slabs() const { return num_z_slabs_; }
  // Gets the number of components.
  unsigned int num_components() const { return num_components_; }
  
  // Gets the bitfield at a particular (x,y,r,c) coordinate, with no bounds-
  // checking.
  Bitfield const & At(unsigned int x, unsigned int y, unsigned int r,
                      unsigned int c) const;
  Bitfield & At(unsigned int x, unsigned int y, unsigned int r,
                unsigned int c);
  // Checks the occupancy value at (x,y,z,c), with no bounds-checking.
  bool OccupiedAt(unsigned int x, unsigned int y, unsigned int z,
                  unsigned int c) const;
  // Sets the occupancy value at (x,y,z,c), with no bounds-checking.
  void SetOccupiedAt(unsigned int x, unsigned int y, unsigned int z,
                     unsigned int c, bool value);
  
  // Prints an ASCII representation of (x,y) columns that are nonzero for each
  // component.
  void Print() const;
  
  // Writes raw data to disk.
  bool Write(char const * filename) const;
  
 private:
  Bitfield * data_;
  unsigned int x_;
  unsigned int y_;
  unsigned int num_z_slabs_;
  unsigned int num_components_;
  unsigned int size_;
  unsigned int reserved_size_;
};

#endif
