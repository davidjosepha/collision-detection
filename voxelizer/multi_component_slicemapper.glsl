// This shader produces, from a mesh that contains up to a couple thousand
// individually-labeled watertight components, a slicemap at a given image
// resolution.  This is a modification of the slicemap algorithm described in
// these two papers:
//  * Eisemann and Decoret. Interactive 3D Graphics and Games, 2006, pp. 71–78.
//  * Eisemann and Decoret. Graphics Interface, 2008, pp. 73–80.
// The algorithm has been simplified to assume that the image space and volume
// space are related to each other only by (possibly non-uniform) scaling; that
// is, that the image volume is an axis-aligned rectangular box.  New features
// in OpenGL 4.2, specifically 3D random-access images with atomic
// XOR operations, allow us to handle multiple components simultaneously.
// 
// By Jadrian Miles (with assistance of Justin Kim at Nvidia), December 2012.

// We need to define the version and some extensions in order to use image units
// and atomic image operations.
#version 420 core
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable

// The slicemap is a 3D image.  The X and Y coordinates correspond to image
// space.  A single X-Y layer of the slicemap is called a "Z-slab" and
// corresponds to 32 Z values mapped into the 32 bits of the slicemap voxel
// values.  The Z dimension consists of a fixed number (S) of Z-slab
// repetitions, for a total of 32*S distinct Z values, in turn repeated C times
// to represent C distinct model components.
layout(size1x32) coherent restrict
  uniform uimage3D slicemap;

// The depth mask LUT is 2D.  Its dimensions are S in the X direction and
// (32*S)+1 in the Y direction.  For a given depth z and slab index s, the LUT
// stores the precomputed depth mask at (X=s,Y=z).
layout(size1x32) coherent restrict readonly
  uniform uimage2D depth_mask_lut;

uniform vec3 box_pos;        // Low corner of rendering box in model space.
uniform vec3 box_size;       // Dimensions of rendering box in model space.
uniform vec3 image_size;     // Size of image space represented by the slicemap.
uniform uint num_z_slabs;

#ifdef _VERTEX_ // vertex shader
// Vertex attributes.  The ID is assumed to be in the range [0, C).
layout(location = 0) in vec3 in_pos;
layout(location = 1) in uint id;

// Output variables to pass to the fragment shader.  We use flat shading on the
// ID because it must be the same for all vertices of a triangle (and of a model
// component, for that matter), so the fragment can get it from any vertex.
// For the fragment position, smooth shading does linear interpolation in world
// space, which is exactly what we want: the position of a fragment is a linear
// interpolation of the positions of the vertices.
smooth out vec3 frag_pos;
flat out uint component_slicemap_z_offset;
void main() {
  // gl_Position for vertices inside the rendering box must be transformed into
  // the canonical view volume ([-1,1] in each dimension) in order for fragments
  // to render.  Vertices outside the rendering box are not clipped, and we
  // want to keep them so they generate the correct primitives (triangles), and
  // in turn the correct fragments.
  gl_Position = vec4((in_pos-box_pos-(box_size/2.0)) / (box_size/2.0), 1);
  // We use the component ID to offset in the Z coordinate of the slicemap
  // array.  Doing the multiplication here saves clock ticks in the fragment
  // shader.
  component_slicemap_z_offset = id * num_z_slabs;
  // To render into the slicemap, we also need to convert coordinates into
  // [0,image_size.d] in each dimension.
  frag_pos = (in_pos - box_pos) / box_size * image_size;
}
#endif

#ifdef _FRAGMENT_ // fragment shader
smooth in vec3 frag_pos;
flat in uint component_slicemap_z_offset;
void main() {
  // Since we have clamping turned on, we know that only fragments outside the
  // canonical view volume in X and Y are clipped; Z is clamped instead.  We
  // don't actually use viewspace coordinates, but the important thing to note
  // is that fragments whose viewspace coordinates have been clamped in Z will
  // still have out-of-bounds frag_pos.z; we need to clamp it manually.
  float z = clamp(frag_pos.z, 0, image_size.z);
  for(uint s = 0; s < num_z_slabs; ++s) {
    imageAtomicXor(slicemap, ivec3(frag_pos.xy, component_slicemap_z_offset+s),
                   imageLoad(depth_mask_lut, ivec2(s, z)).r);
  }
}
#endif
