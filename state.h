#ifndef STATE_H
#define STATE_H
#include <glm/glm.hpp>

class State {
  public:
    State() { }
    /*State(glm::vec4 & verts, glm::highp_uvec3 & tris, glm::mat4 & pose) {
      verts_ = &verts;
      tris_ = &tris;
      pose_ = &pose;
    }*/
    const glm::vec4 * verts() const { return verts_; }
    const glm::highp_uvec3 * tris() const { return tris_; }
    glm::mat4 * pose() { return &pose_; }

    void setVerts(const glm::vec4 & verts) { verts_ = &verts; }
    void setTris(const glm::highp_uvec3 & tris) { tris_ = &tris; }

  private:
    const glm::vec4 * verts_;
    const glm::highp_uvec3 * tris_;
    glm::mat4 pose_;
};

#endif
