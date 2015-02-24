#ifndef __GEOM_3D_H__
#define __GEOM_3D_H__

// By Jadrian Miles, Spring 2013.  If I were to do it all again, instead of
// rolling my own, I'd use GLM (OpenGL Math), which is at http://glm.g-truc.net

#include <math.h>
#include <string>

struct IVec;
struct UIVec;
struct Quat;

struct Vec {
  union {
    struct {
      float x, y, z;
    };
    float xyz[3];
  };
  Vec() : x(0), y(0), z(0) {}
  Vec(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
  Vec(Vec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
  explicit Vec(IVec const & _v);
  explicit Vec(UIVec const & _v);
  explicit Vec(Quat const & q);
  std::string AsString(std::string const & sep = " ") const;
  std::string AsStringPrecise(std::string const & sep = " ") const;
  Vec   operator+ (Vec const & b) const { return Vec(x+b.x, y+b.y, z+b.z); }
  Vec & operator+=(Vec const & b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
  Vec   operator- (Vec const & b) const { return Vec(x-b.x, y-b.y, z-b.z); }
  Vec & operator-=(Vec const & b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
  Vec   operator- () const { return Vec(-x, -y, -z); }
  // The vector multiplication and division operators are componentwise.
  Vec   operator* (Vec const & b) const { return Vec(x*b.x, y*b.y, z*b.z); }
  Vec & operator*=(Vec const & b) { x*=b.x; y*=b.y; z*=b.z; return *this; }
  Vec   operator/ (Vec const & b) const { return Vec(x/b.x, y/b.y, z/b.z); }
  Vec & operator/=(Vec const & b) { x/=b.x; y/=b.y; z/=b.z; return *this; }
  Vec   operator* (float b) const { return Vec(x*b, y*b, z*b); }
  Vec & operator*=(float b) { x*=b; y*=b; z*=b; return *this; }
  Vec   operator/ (float b) const { return Vec(x/b, y/b, z/b); }
  Vec & operator/=(float b) { x/=b; y/=b; z/=b; return *this; }
  Vec Cross(Vec const & b) const { return Vec(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x); }
  float Dot(Vec const & b) const { return x*b.x + y*b.y + z*b.z; }
  float Sum() const { return x+y+z; }
  float Product() const { return x*y*z; }
  float Norm2() const { return x*x + y*y + z*z; }
  float Norm() const { return sqrt(x*x + y*y + z*z); }
  Vec Normalized() const { return *this / Norm(); }
  Vec & Normalize() { return (*this /= Norm()); }
  // Lexicographic ordering.
  bool operator<(Vec const & b) const { return z<b.z || (z==b.z && (y<b.y || (y==b.y && x<b.x))); }
  
  Vec & SetToMin(Vec const & b) { x = (b.x < x ? b.x : x); y = (b.y < y ? b.y : y); z = (b.z < z ? b.z : z); return *this; }
  Vec & SetToMax(Vec const & b) { x = (b.x > x ? b.x : x); y = (b.y > y ? b.y : y); z = (b.z > z ? b.z : z); return *this; }
};
typedef Vec Point;

struct IVec {
  union {
    struct {
      int x, y, z;
    };
    int xyz[3];
  };
  IVec() : x(0), y(0), z(0) {}
  IVec(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
  IVec(IVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
  explicit IVec(Vec const & _v);
  explicit IVec(UIVec const & _v);
  std::string AsString(std::string const & sep = " ") const;
  IVec   operator+ (IVec const & b) const { return IVec(x+b.x, y+b.y, z+b.z); }
  IVec & operator+=(IVec const & b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
  IVec   operator- (IVec const & b) const { return IVec(x-b.x, y-b.y, z-b.z); }
  IVec & operator-=(IVec const & b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
  IVec   operator- () const { return IVec(-x, -y, -z); }
  // The vector multiplication and division operators are componentwise.
  IVec   operator* (IVec const & b) const { return IVec(x*b.x, y*b.y, z*b.z); }
  IVec & operator*=(IVec const & b) { x*=b.x; y*=b.y; z*=b.z; return *this; }
  IVec   operator/ (IVec const & b) const { return IVec(x/b.x, y/b.y, z/b.z); }
  IVec & operator/=(IVec const & b) { x/=b.x; y/=b.y; z/=b.z; return *this; }
  IVec   operator* (int b) const { return IVec(x*b, y*b, z*b); }
  IVec & operator*=(int b) { x*=b; y*=b; z*=b; return *this; }
  IVec   operator/ (int b) const { return IVec(x/b, y/b, z/b); }
  IVec & operator/=(int b) { x/=b; y/=b; z/=b; return *this; }
  int Dot(IVec const & b) const { return x*b.x + y*b.y + z*b.z; }
  int Sum() const { return x+y+z; }
  int Product() const { return x*y*z; }
  int Norm2() const { return x*x + y*y + z*z; }
  float Norm() const { return sqrt(float(x*x + y*y + z*z)); }
  bool operator==(IVec const & b) const { return (x==b.x) && (y==b.y) && (z==b.z); }
  bool operator!=(IVec const & b) const { return (x!=b.x) || (y!=b.y) || (z==b.z); }
  // Lexicographic ordering.
  bool operator<(IVec const & b) const { return z<b.z || (z==b.z && (y<b.y || (y==b.y && x<b.x))); }
  
  IVec & SetToMin(IVec const & b) { x = (b.x < x ? b.x : x); y = (b.y < y ? b.y : y); z = (b.z < z ? b.z : z); return *this; }
  IVec & SetToMax(IVec const & b) { x = (b.x > x ? b.x : x); y = (b.y > y ? b.y : y); z = (b.z > z ? b.z : z); return *this; }
};
typedef IVec IPoint;

struct PackedUIVec;

struct UIVec {
  union {
    struct {
      unsigned int x, y, z;
    };
    unsigned int xyz[3];
  };
  UIVec() : x(0), y(0), z(0) {}
  UIVec(unsigned int _x, unsigned int _y, unsigned int _z) : x(_x), y(_y), z(_z) {}
  UIVec(UIVec const & _v) : x(_v.x), y(_v.y), z(_v.z) {}
  explicit UIVec(Vec const & _v);
  explicit UIVec(IVec const & _v);
  std::string AsString(std::string const & sep = " ") const;
  UIVec   operator+ (UIVec const & b) const { return UIVec(x+b.x, y+b.y, z+b.z); }
  UIVec & operator+=(UIVec const & b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
  UIVec   operator- (UIVec const & b) const { return UIVec(x-b.x, y-b.y, z-b.z); }
  UIVec & operator-=(UIVec const & b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
  // The vector multiplication and division operators are componentwise.
  UIVec   operator* (UIVec const & b) const { return UIVec(x*b.x, y*b.y, z*b.z); }
  UIVec & operator*=(UIVec const & b) { x*=b.x; y*=b.y; z*=b.z; return *this; }
  UIVec   operator/ (UIVec const & b) const { return UIVec(x/b.x, y/b.y, z/b.z); }
  UIVec & operator/=(UIVec const & b) { x/=b.x; y/=b.y; z/=b.z; return *this; }
  UIVec   operator* (unsigned int b) const { return UIVec(x*b, y*b, z*b); }
  UIVec & operator*=(unsigned int b) { x*=b; y*=b; z*=b; return *this; }
  UIVec   operator/ (unsigned int b) const { return UIVec(x/b, y/b, z/b); }
  UIVec & operator/=(unsigned int b) { x/=b; y/=b; z/=b; return *this; }
  unsigned int Dot(UIVec const & b) const { return x*b.x + y*b.y + z*b.z; }
  unsigned int Sum() const { return x+y+z; }
  unsigned int Product() const { return x*y*z; }
  unsigned int Norm2() const { return x*x + y*y + z*z; }
  float Norm() const { return sqrt(float(x*x + y*y + z*z)); }
  UIVec CeilDivide(UIVec const & b) const;
  // Considering this vector as the size of a 3-D array, convert the given set
  // of index coordinates into the equivalent linear index in that array.
  unsigned int ToIndex(UIVec const & c) const { return c.x + x*(c.y + y*c.z); }
  // Considering this vector as the size of a 3-D array, convert the given
  // linear index in that array into a set of index coordinates.
  UIVec FromIndex(unsigned int i) const { return UIVec(i%x, (i/x)%y, i/(x*y)); }
  bool operator==(UIVec const & b) const { return (x==b.x) && (y==b.y) && (z==b.z); }
  bool operator!=(UIVec const & b) const { return (x!=b.x) || (y!=b.y) || (z==b.z); }
  // Lexicographic ordering.
  bool operator<(UIVec const & b) const { return z<b.z || (z==b.z && (y<b.y || (y==b.y && x<b.x))); }
  
  UIVec & SetToMin(UIVec const & b) { x = (b.x < x ? b.x : x); y = (b.y < y ? b.y : y); z = (b.z < z ? b.z : z); return *this; }
  UIVec & SetToMax(UIVec const & b) { x = (b.x > x ? b.x : x); y = (b.y > y ? b.y : y); z = (b.z > z ? b.z : z); return *this; }
};
typedef UIVec UIPoint;

struct UIVecHash {
  size_t operator()(UIVec const & x) {
#if __LP64__
    // Adapted from Thomas Wang's 64-bit integer mixing function, from
    //   http://www.concentric.net/~Ttwang/tech/inthash.htm
    // This page went offline sometime between November 2012 and March 2013, but
    // is available through the Wayback Machine.
    unsigned long int key = (unsigned long int)(x.z);
    key = ((key<<21)|(key>>43)) ^ (unsigned long int)(x.y);
    key = ((key<<21)|(key>>43)) ^ (unsigned long int)(x.z);
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
    unsigned int key = x.x ^ ((x.y<<10)|(x.y>>22)) ^ ((x.z<<20)|(x.z>>12));
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

struct PackedUIVec {
  const static unsigned int BITS_PER_COMPONENT = 10;
  const static unsigned int MASK = ~(~0u << BITS_PER_COMPONENT);
  const static unsigned int OFFSET_X = 0;
  const static unsigned int OFFSET_Y = BITS_PER_COMPONENT;
  const static unsigned int OFFSET_Z = 2*BITS_PER_COMPONENT;
  const static unsigned int MASK_X = MASK << OFFSET_X;
  const static unsigned int MASK_Y = MASK << OFFSET_Y;
  const static unsigned int MASK_Z = MASK << OFFSET_Z;
  unsigned int xyz;
  PackedUIVec() : xyz(0) {}
  PackedUIVec(unsigned int x, unsigned int y, unsigned int z)
    : xyz(((x & MASK) << OFFSET_X) |
          ((y & MASK) << OFFSET_Y) |
          ((z & MASK) << OFFSET_Z)) {}
  PackedUIVec(UIVec const & rhs)
    : xyz(((rhs.x & MASK) << OFFSET_X) |
          ((rhs.y & MASK) << OFFSET_Y) |
          ((rhs.z & MASK) << OFFSET_Z)) {}
  operator unsigned int () { return xyz; }
  unsigned int X() const { return (xyz & MASK_X) >> OFFSET_X; }
  unsigned int Y() const { return (xyz & MASK_Y) >> OFFSET_Y; }
  unsigned int Z() const { return (xyz & MASK_Z) >> OFFSET_Z; }
  UIVec Unpack() const { return UIVec(X(), Y(), Z()); }
  void Unpack(UIVec & rhs) const {
    rhs.x = X();
    rhs.y = Y();
    rhs.z = Z();
  }
};

UIVec ComponentwiseMin(UIVec const & a, UIVec const & b);
UIVec ComponentwiseMax(UIVec const & a, UIVec const & b);

unsigned int CeilDivide(unsigned int a, unsigned int b);

struct Quat {
  Vec v;
  float w;
  Quat() : v(0,0,0), w(0) {}
  Quat(Quat const & q) : v(q.v), w(q.w) {}
  explicit Quat(float _w) : v(0,0,0), w(_w) {}
  explicit Quat(Vec const & _v) : v(_v), w(0) {}
  Quat(Vec const & _v, float _w) : v(_v), w(_w) {}
  Quat(float _x, float _y, float _z, float _w) : v(_x,_y,_z), w(_w) {}
  std::string AsString(std::string const & sep = " ") const;
  Quat   operator+ (Quat const & p) const { return Quat(v+p.v, w+p.w); }
  Quat & operator+=(Quat const & p) { v+=p.v; w+=p.w; return *this; }
  Quat   operator- (Quat const & p) const { return Quat(v-p.v, w-p.w); }
  Quat & operator-=(Quat const & p) { v-=p.v; w-=p.w; return *this; }
  Quat   operator- () const { return Quat(-v, -w); }
  // Quaternion multiplication is defined in the usual way.
  Quat   operator* (Quat const & p) const { return Quat(v.Cross(p.v)+v*p.w+p.v*w, w*p.w-v.Dot(p.v)); }
  Quat & operator*=(Quat const & p) { float d=v.Dot(p.v); v=v.Cross(p.v)+v*p.w+p.v*w; w=w*p.w-d; return *this; }
  // No such thing, really, as quaternion division. Use Inverted() instead.
  Quat   operator* (float b) const { return Quat(v*b, w*b); }
  Quat & operator*=(float b) { v*=b; w*=b; return *this; }
  Quat   operator/ (float b) const { return Quat(v/b, w/b); }
  Quat & operator/=(float b) { v/=b; w/=b; return *this; }
  float Norm2() const { return v.Norm2() + w*w; }
  float Norm() const { return sqrt(v.Norm2() + w*w); }
  Quat Conjugated() const { return Quat(-v, w); }
  Quat & Conjugate() { v = -v; return *this; }
  Quat Inverted() const { return Conjugated() / Norm2(); }
  Quat & Invert() { return (Conjugate() /= Norm2()); }
  Quat Normalized() const { return *this / Norm(); }
  Quat & Normalize() { return (*this /= Norm()); }
  // Treats the quaternion as homogeneous coordinates and divides out by w.
  Quat Homogenized() const { return Quat(v/w,1.0); }
  Quat & Homogenize() { v/=w; w=1.0; return *this; }
  // Rotate the vector v by the rotation represented by this.
  Quat Rotate(Quat const & p) const { return (*this)*p*Conjugated()/Norm2(); }
};

Quat RotQuat(Vec const & v, float theta);

#endif
