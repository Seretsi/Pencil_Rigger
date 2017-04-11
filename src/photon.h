#ifndef _PHOTON_H_
#define _PHOTON_H_

#include <glm/glm.hpp>


// ===========================================================
// Class to store the information when a photon hits a surface

class Photon {
 public:

  // CONSTRUCTOR
  Photon(const glm::vec3 &p, const glm::vec3 &d, const glm::vec3 &e, int b) :
    position(p),direction_from(d),energy(e),bounce(b) {}

  // ACCESSORS
  const glm::vec3& getPosition() const { return position; }
  const glm::vec3& getDirectionFrom() const { return direction_from; }
  const glm::vec3& getEnergy() const { return energy; }
  int whichBounce() const { return bounce; }

 private:
  // REPRESENTATION
  glm::vec3 position;
  glm::vec3 direction_from;
  glm::vec3 energy;
  int bounce;
};

#endif
