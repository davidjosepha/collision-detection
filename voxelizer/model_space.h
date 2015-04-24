#ifndef __MODEL_SPACE_H__
#define __MODEL_SPACE_H__

// The entire voxelizer project assumes that its input is in a normalized space
// where the voxel is the unit of measure: that is, the units on all input data
// are assumed to be such that voxels are 1x1x1.
//   Furthermore, voxels are understood to occupy the unit volume of space
// ABOVE some integer lattice point.  That is, the voxel (a, b, c) encloses all
// points (x, y, z) such that a < x < a+1; b < y < b+1; c < z < c+1.
//   Thus, the point in space corresponding to a voxel's integer coordinates is
// its BOTTOM CORNER.  This is contrary to some standards (like the NIfTI spec),
// in which the corresponding point is the center of the voxel.  The decisison
// to use the bottom corner makes the math cleaner in the voxelizer
// implementation.
// 
// In the interest of brevity in the following discussion, we define a few
// convenience terms:
// - A "free multiple" of some vector V is another vector, each of whose
//   components is an integer multiple of the corresponding component of V.
//   (Note that the factors may be different in each component.)  For example,
//   <10, 6> is a free multiple of <5, 2>.
// - An "AIRC" is an Axis-aligned Integer-side-length Rectangular Cuboid.  An
//   AIRC has its bottom corner at some integer lattice point (a, b, c) and
//   side lengths (m, n, p), and encloses all points (x, y, z) such that
//   a < x < a+m; b < y < b+n; c < z < c+p.
//    - The AIRC described above is "at" or "located at" (a, b, c).
//      (a, b, c) is the "location" of this AIRC.
//    - The AIRC described above has "size" (m, n, p).
// - An "AIRC lattice of stride S" is a set of AIRCs, all of which have size S,
//   that are at all (relevant) free multiples of S.
//    - The AIRCs making up a lattice are the "cells" of that lattice.
//    - An AIRC "on" a given lattice is one at a free multiple of S whose size
//      is a free multiple of S.  (It's an AIRC "made up of" lattice cells.)
// 
// So, to give some trivial examples:
// - Every voxel is an AIRC of size (1, 1, 1).
// - The set of voxels is an infinite AIRC lattice of stride (1, 1, 1).
// - Every AIRC (of any size, at any location) is an AIRC on the voxel lattice.
// 
// The volume of the model space is assumed to be an AIRC at the origin, and its
// size is ModelSpace::size (declared in this file).
// 
// Since the GPU probably can't work on the entire model space at once, the
// voxelizer project needs a globablly consistent way to decompose the model
// space into smaller chunks.  We standardize on a simple solution: an AIRC
// lattice with a given stride.  Each cell of this lattice is called a "render
// box", and we often just use the term "box" to mean this specifically.
// - ModelSpace::box_size (declared in this file) specifies the stride of the
//   render-box lattice (and therefore the size of every render box, and also
//   the set of coordinates at which they are located).
// - Rendering ordinarily takes place in AIRCs on the render-box lattice; see,
//   for example, the comment on Voxelizer::RenderRegion().
// 
// Note that above, we've defined AIRCs as open sets: they don't contain their
// boundaries.  AIRC lattices (like the voxel lattice and render-box lattice)
// therefore do not cover points on the boundaries between neighboring cells.
// There are two ways to deal with this:
// A. Since the voxelizer project is concerned with turning watertight surface
//    meshes into solid approximations of their interiors, we define the
//    interior of a watertight surface as an open set, not inclusive of the
//    surface.  So if a surface runs along a seam between two voxels, it doesn't
//    matter which voxel each point *on* the surface "belongs" to; we care about
//    the interior, and the interior belongs to the voxel on the inner side of
//    the surface.
//      The general principle, then, is that "ties go to the interior": if the
//    ownership of point P is ambiguous, then it belongs to the AIRC that also
//    encloses points in the neighborhood of P that are relevant to the
//    structure in question.
// B. In all other cases (where there is no higher-level structure that gives
//    preference to one "side" of a point over another), then the ranges used in
//    the definitions above are understood to be inclusive on the low end.
// 
// This project establishes another non-standard convention: a redefinition of
// the term "bounding box".
// - The "bounding box" of a set of points is the minimum-volume AIRC that
//   encloses those points AND ALSO is on the render-box lattice.
// - The term "tight bounding box" is instead used to specify the usual sense:
//   a minimum-volume axis-aligned rectangular cuboid enclosing some points.
// - The "tight integer bounding box" is the minum-volume enclosing AIRC.
// 
// A bounding box is specified by the locations of the bottom and top render
// boxes that are inside it.  There are four important points to note:
// - The "upper-bound" *box* is INCLUSIVE.  (That is, the upper-bound box is the
//   topmost one that is contained within the overall bounding box.  This is
//   contrary to many standard practices in C++, where upper bounds are given by
//   the next value outside the region of interest.)
// - However, the upper-bound box, like all boxes, is specified by its
//   *location* --- the point at its bottom corner.  Therefore the coordinates
//   given for this box are well *inside* the bounding box: the upper corner of
//   the bounding box is this location plus ModelSpace::box_size.
// - The location of the lower-bound box is the low corner of both this box and
//   also the overall bounding box.
// - The length of the bounding box in each dimension is upper-lower+box_size.

#include <map>
#include <set>
#include <vector>
#include "geom_3d.h"

// Forward declaration so we can use WatertightComponent pointers in the
// definition of the ComponentSet type below.
class WatertightComponent;

// The ModelSpace namespace is just a container for a bunch of static info
// regarding the setup of the model space for a particular voxelization problem
// (or set of voxelization problems that all share the same model space).
//   The library initializes the three extern variables below to zero (in
// model_space.cpp).  In order for the renderers to work correctly, the extern
// variables must be assigned new, nonzero values *before* creating any ModelMap
// objects or calling any ModelSpace functions.
//   Note that there are no technical restrictions on the number of times client
// code may reassign new values to size and box_size.  In most cases, you
// probably only want to do it once, very near the beginning of your program.
// It's conceivable, though, that you might want your program to render several
// entirely separate models, each with a different model space, one after the
// other.  So the library allows you the flexibility to reassign the ModelSpace
// variables.
//   However, note that assigning box_size a new value silently invalidates all
// existing ModelMap objects!  So before setting new values for these variables,
// be *absolutely certain* that you've destroyed all existing ModelMaps.
namespace ModelSpace {
  // The size of the render boxes into which the model space is subdivided.
  extern UIVec box_size;
  // The size (number of voxels in each dimension) of the model space.
  extern UIVec size;
  // The size of the model space, rounded up to a free multiple of box_size.  It
  // is the client's responsibility to correctly perform this rounding (or use
  // SetSize() below).
  extern UIVec size_rounded_to_boxes;
  // The number of boxes, in each dimension, that overlap with the model space.
  // It must be the case that size_rounded_to_boxes = num_boxes * box_size.  As
  // above, use SetSize() to assure this.
  extern UIVec num_boxes;
  
  // Sets size, size_rounded_to_boxes, and num_boxes to reflect the given model
  // space size.
  // Precondition: box_size must be nonzero in all components.
  void SetSize(UIVec const & true_size);
  
  // Returns the location (low corner) of the box containing the given point.
  // Note that box locations are always non-negative, and are represented with
  // UIVecs.  This function does no bounds-checking, and in particular if x has
  // a negative component, the corresponding component in the return value will
  // wrap around to an enormous positive integer near 2^32 (as the casting goes
  // float -> integer -> unsigned integer).  So do your bounds-checking before
  // calling this function!
  UIVec BoxContaining(Point const & x);
  
  // Sets min_box and max_box, respectively, to the bounding box of the given
  // set of boxes.  (Remember these are the low corners of the inclusive
  // bounds.)
  //   Precondition: known_boxes must not be empty.  You will probably get a
  // segfault if you call this with an emtpy set; since this code gets called
  // frequently, no bounds-checking is built in.
  void MinMaxBoxes(std::set<UIVec> const & known_boxes, UIVec & min_box,
                   UIVec & max_box);
  // Enumerates all boxes in the bounding volume of known_boxes, and appends
  // them to the end of all_boxes.
  //   Precondition: known_boxes must not be empty.  Ditto the warning above.
  //   Postcondition: all_boxes.front() is the min box, and all_boxes.back() is
  // the max.
  void BoxesInBoundingBox(std::set<UIVec> const & known_boxes,
                          std::vector<UIVec> & all_boxes);
}

typedef std::set<WatertightComponent const *> ComponentSet;

// A ModelMap is how a client communicates to the Voxelizer which components may
// be found in which portions of the model space.  In particular, each key of a
// ModelMap is expected to be the location of a render box --- that is, a free
// multiple of ModelSpace::box_size.
typedef std::map<UIVec, ComponentSet> ModelMap;

// TODO: we could optimize both of these by writing our own data structures.
// Respectively:
// - ComponentSet: specialized hash-set implementation, with hashing of pointers
// - ModelMap: a 1-D array, one entry for each valid render box in the
//             tesselation.
// These would give us O(1) expected performance on all calls, probably with a
// quite low coefficient.  As it stands, lookups in the ModelMap are O(log N),
// where N is the number of boxes, which is fixed, so it's essentially O(1).
// Lookups in a ComponentSet are O(log M), where M is the number of components
// already in the set, giving O(P log P) time overall for P successive
// insertions (rather than O(P) expected with a hash-set).
//   For posterity, the PointerHash function object is replicated below:
/*
struct PointerHash {
  size_t operator()(void const * x) {
#if __LP64__
    // Adapted from Thomas Wang's 64-bit integer mixing function, from
    //   http://www.concentric.net/~Ttwang/tech/inthash.htm
    // This page went offline sometime between November 2012 and March 2013, but
    // is available through the Wayback Machine.
    unsigned long int key = (unsigned long int)(x);
    key = (key << 61) | (key >> 3);
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);
#else
    // Adapted from Bob Jenkins's 6-shift 32-bit mixing function, from
    //   http://burtleburtle.net/bob/hash/integer.html
    unsigned int key = (unsigned int)(x);
    key = (key << 29) | (key >> 3);
    key = (key + 0x7ed55d16) + (key << 12);
    key = (key ^ 0xc761c23c) ^ (key >> 19);
    key = (key + 0x165667b1) + (key << 5);
    key = (key + 0xd3a2646c) ^ (key << 9);
    key = (key + 0xfd7046c5) + (key << 3);
    key = (key ^ 0xb55a4f09) ^ (key >> 16);
#endif
    return key;
  }
};
*/

#endif
