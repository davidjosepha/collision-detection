#include "geom_3d.h"

#include <math.h>

#include <limits>
#include <sstream>
#include <string>

Vec::Vec(IVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
Vec::Vec(UIVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
Vec::Vec(Quat const & q) : x(q.v.x/q.w), y(q.v.y/q.w), z(q.v.z/q.w) {}
IVec::IVec(Vec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
IVec::IVec(UIVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
UIVec::UIVec(Vec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
UIVec::UIVec(IVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}

std::string Vec::AsString(std::string const & sep) const {
  std::stringstream ss;
  ss << x << sep << y << sep << z;
  return ss.str();
}
std::string Vec::AsStringPrecise(std::string const & sep) const {
  std::stringstream ss;
  ss.precision(std::numeric_limits<float>::digits10);
  ss << x << sep << y << sep << z;
  return ss.str();
}
std::string IVec::AsString(std::string const & sep) const {
  std::stringstream ss;
  ss << x << sep << y << sep << z;
  return ss.str();
}
std::string UIVec::AsString(std::string const & sep) const {
  std::stringstream ss;
  ss << x << sep << y << sep << z;
  return ss.str();
}

std::string Quat::AsString(std::string const & sep) const {
  std::stringstream ss;
  ss << "[(" << v.x << sep << v.y << sep << v.z << ")" << sep << w << "]";
  return ss.str();
}

UIVec UIVec::CeilDivide(UIVec const & b) const {
  return UIVec(::CeilDivide(x,b.x),::CeilDivide(y,b.y),::CeilDivide(z,b.z));
}

UIVec ComponentwiseMin(UIVec const & a, UIVec const & b) {
  return UIVec(a.x < b.x ? a.x : b.x,
               a.y < b.y ? a.y : b.y,
               a.z < b.z ? a.z : b.z);
}
UIVec ComponentwiseMax(UIVec const & a, UIVec const & b) {
  return UIVec(a.x > b.x ? a.x : b.x,
               a.y > b.y ? a.y : b.y,
               a.z > b.z ? a.z : b.z);
}

unsigned int CeilDivide(unsigned int a, unsigned int b) {
  return (a + b - 1) / b;
}

Quat RotQuat(Vec const & v, float theta) {
  return Quat(v.Normalized()*sin(theta/2.0), cos(theta/2.0));
}
