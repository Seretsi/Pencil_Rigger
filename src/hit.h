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
    t = std::vector<float>();
    material = NULL;
    normal = glm::vec3(0,0,0); 
    texture_s = 0;
    texture_t = 0;
  }
  Hit(const Hit &h) { 
    t = std::vector<float>(); 
    for (int i = 0; i < h.t.size(); i++) {
      t.push_back(h.t[i]);
    }
    material = h.material; 
    normal = h.normal; 
    texture_s = h.texture_s;
    texture_t = h.texture_t;
  }
  ~Hit() {}

  // ACCESSORS
  float getT(int i) const { return t[i]; }
  Material* getMaterial() const { return material; }
  glm::vec3 getNormal() const { return normal; }
  float get_s() const { return texture_s; }
  float get_t() const { return texture_t; }
  int num_hits() const { return t.size(); }
  void push_t(float t) {
    this->t.push_back(t);
  }
  // MODIFIER
  void set(float _t, Material *m, glm::vec3 n) {
    t.push_back(_t); material = m; normal = n; 
    texture_s = 0; texture_t = 0; }

  void setTextureCoords(float t_s, float t_t) {
    texture_s = t_s; texture_t = t_t; 
  }

private: 

  // REPRESENTATION
  std::vector<float> t;
  Material *material;
  glm::vec3 normal;
  float texture_s, texture_t;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
  os << "Hit <";
  for (int i = 0; i < h.num_hits(); i++) {
    os << h.getT(i) << ", < ";
  }
  os << h.getNormal().x << "," 
    << h.getNormal().y << "," 
    << h.getNormal().z << " > > ";
  return os;
}
// ====================================================================
// ====================================================================

#endif
