#ifndef _RAY_TRACER_
#define _RAY_TRACER_

#include <glm/glm.hpp>
#include <vector>
#include "ray.h"
#include "hit.h"
#include "vbo_structs.h"

class Mesh;
class ArgParser;
class Radiosity;
class PhotonMapping;

// ====================================================================
// ====================================================================
// This class manages the ray casting and ray tracing work.

class RayTracer {

public:

  // CONSTRUCTOR & DESTRUCTOR
  RayTracer(Mesh *m, ArgParser *a) {
    mesh = m;
    args = a;
  }  
  // set access to the other modules for hybrid rendering options
  void setRadiosity(Radiosity *r) { radiosity = r; }
  void setPhotonMapping(PhotonMapping *pm) { photon_mapping = pm; }

  void initializeVBOs(); 
  void resetVBOs(); 
  void setupVBOs(); 
  void drawVBOs();
  void cleanupVBOs();

  // casts a single ray through the scene geometry and finds the closest hit
  bool CastRay(const Ray &ray, Hit &h, bool use_sphere_patches) const;

  // does the recursive work
  glm::vec3 TraceRay(Ray &ray, Hit &hit, int bounce_count = 0) const;

private:

  void drawVBOs_a();
  void drawVBOs_b();

  // REPRESENTATION
  Mesh *mesh;
  ArgParser *args;
  Radiosity *radiosity;
  PhotonMapping *photon_mapping;

public:
  //float pixels_a_size;
  ///float pixels_b_size;


  bool render_to_a;
  std::vector<VBOPosNormalColor> pixels_a;
  std::vector<VBOPosNormalColor> pixels_b;
  std::vector<VBOIndexedTri> pixels_indices_a;
  std::vector<VBOIndexedTri> pixels_indices_b;


private:

  GLuint pixels_a_VBO;
  GLuint pixels_b_VBO;
  GLuint pixels_indices_a_VBO;
  GLuint pixels_indices_b_VBO;

};

// ====================================================================
// ====================================================================

#endif
