#include "anonymesh.h"

#include <vector>
#include <gl_compute/gl_helper.h>
#include "geom_3d.h"

Anonymesh::Anonymesh()
    : vao(0), vbuf(0), ibuf(0) {
  // Create new vertex array object, vertex buffer, and index buffer.
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbuf);
  glGenBuffers(1, &ibuf);
}

Anonymesh::~Anonymesh() {
  glDeleteBuffers(1, &vbuf);
  glDeleteBuffers(1, &ibuf);
  glDeleteVertexArrays(1, &vao);
}

void Anonymesh::Clear() {
  verts.clear();
  tris.clear();
}

void Anonymesh::LoadIntoGl() {
  // Bind the vertex array object so all subsequent vertex and index buffers
  // will be associate with it.
  glBindVertexArray(vao);
  
  // Bind the vertex buffer so we can load up the vertices.
  glBindBuffer(GL_ARRAY_BUFFER, vbuf);
  // Copy our vertices into the vertex buffer in GPU memory.
  // Note that GL_STATIC_DRAW denotes that our mesh is static and will not
  // change its vertex values; this optimizes performance.
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec)*verts.size(),
                &verts[0].x, GL_STATIC_DRAW);
  
  // Tell GL how to find the position attribute of each vertex.
  glEnableVertexAttribArray(0);
  // Position: 3 floats at the beginning of the Vertex struct so offset is 0.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec),
                        (GLvoid *) 0);
  
  // Now bind the index buffer so we can copy the triangle edge data in.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
  // Copy the contents to GPU memory.
  // Note that glBufferData automatically clears the old buffer contents.
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*tris.size(),
                &tris[0], GL_STATIC_DRAW);
  
  // Unbind the vertex array for cleanliness.
  glBindVertexArray(0);
}
