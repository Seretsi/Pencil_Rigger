#ifndef _RADIOSITY_H_
#define _RADIOSITY_H_

#include <glm/glm.hpp>

#include "argparser.h"
#include "vbo_structs.h"

class Mesh;
class Face;
class Vertex;
class RayTracer;
class PhotonMapping;

// ====================================================================
// ====================================================================
// This class manages the radiosity calculations, including form factors
// and radiance solution.

class Radiosity {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Radiosity(Mesh *m, ArgParser *args);
  ~Radiosity();
  void Reset();
  void Cleanup();
  void ComputeFormFactors();
  void setRayTracer(RayTracer *r) { raytracer = r; }
  void setPhotonMapping(PhotonMapping *pm) { photon_mapping = pm; }

  void initializeVBOs(); 
  void setupVBOs(); 
  void drawVBOs();
  void cleanupVBOs();

  // =========
  // ACCESSORS
  Mesh* getMesh() const { return mesh; }
  float getFormFactor(int i, int j) const {
    // F_i,j radiant energy leaving i arriving at j
    assert (i >= 0 && i < num_faces);
    assert (j >= 0 && j < num_faces);
    assert (formfactors != NULL);
    return formfactors[i*num_faces+j]; }
  float getArea(int i) const {
    assert (i >= 0 && i < num_faces);
    return area[i]; }
  glm::vec3 getUndistributed(int i) const {
    assert (i >= 0 && i < num_faces);
    return undistributed[i]; }
  glm::vec3 getAbsorbed(int i) const {
    assert (i >= 0 && i < num_faces);
    return absorbed[i]; }
  glm::vec3 getRadiance(int i) const {
    assert (i >= 0 && i < num_faces);
    return radiance[i]; }
  
  // =========
  // MODIFIERS
  float Iterate();
  void setFormFactor(int i, int j, float value) { 
    assert (i >= 0 && i < num_faces);
    assert (j >= 0 && j < num_faces);
    assert (formfactors != NULL);
    formfactors[i*num_faces+j] = value; }
  void normalizeFormFactors(int i) {
    float sum = 0;
    int j;
    for (j = 0; j < num_faces; j++) {
      sum += getFormFactor(i,j); }
    if (sum == 0) return;
    for (j = 0; j < num_faces; j++) {
      setFormFactor(i,j,getFormFactor(i,j)/sum); } }
  void setArea(int i, float value) {
    assert (i >= 0 && i < num_faces);
    area[i] = value; }
  void setUndistributed(int i, glm::vec3 value) { 
    assert (i >= 0 && i < num_faces);
    undistributed[i] = value; }
  void findMaxUndistributed();
  void setAbsorbed(int i, glm::vec3 value) { 
    assert (i >= 0 && i < num_faces);
    absorbed[i] = value; }
  void setRadiance(int i, glm::vec3 value) { 
    assert (i >= 0 && i < num_faces);
    radiance[i] = value; }

private:

  glm::vec3 setupHelperForColor(Face *f, int i, int j);

  // ==============
  // REPRESENTATION
  Mesh *mesh;
  ArgParser *args;
  int num_faces;
  RayTracer *raytracer;
  PhotonMapping *photon_mapping;

  // a nxn matrix
  // F_i,j radiant energy leaving i arriving at j
  float *formfactors;

  // length n vectors
  float *area;
  glm::vec3 *undistributed; // energy per unit area
  glm::vec3 *absorbed;      // energy per unit area
  glm::vec3 *radiance;      // energy per unit area

  int max_undistributed_patch;  // the patch with the most undistributed energy
  float total_undistributed;    // the total amount of undistributed light
  float total_area;             // the total area of the scene

  // VBOs
  GLuint mesh_tri_verts_VBO;
  GLuint mesh_tri_indices_VBO;
  GLuint mesh_textured_tri_indices_VBO;

  std::vector<VBOPosNormalColor> mesh_tri_verts; 
  std::vector<VBOIndexedTri> mesh_tri_indices;
  std::vector<VBOIndexedTri> mesh_textured_tri_indices;
};

// ====================================================================
// ====================================================================

#endif
