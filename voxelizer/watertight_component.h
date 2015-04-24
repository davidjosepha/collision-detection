#ifndef __WATERTIGHT_COMPONENT_H__
#define __WATERTIGHT_COMPONENT_H__

#include <vector>

#include "geom_3d.h"
#include "id_mesh.h"

// An interface for a single watertight component with an ID number that can add
// itself to an IDMesh.  Subclass this to create a class that the Voxelizer
// can work with.
class WatertightComponent {
 public:
  virtual ~WatertightComponent() {}
  virtual unsigned int Id() const = 0;
  
  virtual void AddTrianglesToMesh(IDMesh & mesh) const = 0;
  virtual void AddTrianglesToMesh(IDMesh & mesh, unsigned int fake_id) const=0;
};

#endif
