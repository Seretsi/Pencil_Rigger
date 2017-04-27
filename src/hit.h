#ifndef _HIT_H_
#define _HIT_H_

#include <glm/glm.hpp>
#include <float.h>
#include <ostream>
#include <vector>

#include "ray.h"

class Material;

// Hit class mostly copied from Peter Shirley and Keith Morley
// ====================================================================
// ====================================================================

class Hit {
  
public:

  // CONSTRUCTOR & DESTRUCTOR
  Hit() { 
    t0 = 10000;
    t1 = 10000;
    material = NULL;
    normal = glm::vec3(0,0,0); 
    texture_s = 0;
    texture_t = 0;
  }
  Hit(const Hit &h) { 
    t0 = h.t0;
    t1 = h.t1;
    material = h.material; 
    normal = h.normal; 
    texture_s = h.texture_s;
    texture_t = h.texture_t;
  }
  ~Hit() {}

  // ACCESSORS
  float getT(int i) const { if (i == 1) return t1; else return t0;}
  Material* getMaterial() const { return material; }
  glm::vec3 getNormal() const { return normal; }
  float get_s() const { return texture_s; }
  float get_t() const { return texture_t; }
  int num_hits() const { if (t0 != 10000 && t1 != 10000) return 2;
                         else if (t1 != 10000 || t0 != 10000) return 1;
                         else return 0;}
  void push_t(float t) {
    if (t0 == 10000) t0 = t; 
    else t1 = t;
  }
  // MODIFIER
  void set(float _t, Material *m, glm::vec3 n) {
    if (t0 == 10000) t0 = _t; 
    else if (t1 == 10000) t1 = _t;
    else t1 = _t;
     material = m; normal = n; 
    texture_s = 0; texture_t = 0; }

  void setTextureCoords(float t_s, float t_t) {
    texture_s = t_s; texture_t = t_t; 
  }

private: 

  // REPRESENTATION
  float t0;
  float t1;
  Material *material;
  glm::vec3 normal;
  float texture_s, texture_t;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
  os << "Hit <" << h.getT(0) << ", " << h.getT(1) << " < ";
  
  os << h.getNormal().x << "," 
    << h.getNormal().y << "," 
    << h.getNormal().z << " > > ";
  return os;
}
// ====================================================================
// ====================================================================

#endif
