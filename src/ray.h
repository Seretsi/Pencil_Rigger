#ifndef _RAY_H
#define _RAY_H

#include <iostream>
#include <glm/glm.hpp>

// Ray class mostly copied from Peter Shirley and Keith Morley
// ====================================================================
// ====================================================================

class Ray {

public:

  // CONSTRUCTOR & DESTRUCTOR
  Ray (const glm::vec3 &orig, const glm::vec3 &dir) {
    origin = orig; 
    direction = dir; }

  // ACCESSORS
  const glm::vec3& getOrigin() const { return origin; }
  const glm::vec3& getDirection() const { return direction; }
  glm::vec3 pointAtParameter(float t) const {
    return origin+direction*t; }

private:
  Ray () { assert(0); } // don't use this constructor

  // REPRESENTATION
  glm::vec3 origin;
  glm::vec3 direction;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
  os << "Ray < < " 
     << r.getOrigin().x << "," 
     << r.getOrigin().y << "," 
     << r.getOrigin().z << " > " 
     <<", "
     << r.getDirection().x << "," 
     << r.getDirection().y << "," 
     << r.getDirection().z << " > >";
  return os;
}

// ====================================================================
// ====================================================================

#endif
