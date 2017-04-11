#ifndef _RAY_TREE_H
#define _RAY_TREE_H

#include "glCanvas.h"
#include <vector>
#include "ray.h"
#include "vbo_structs.h"

// ====================================================================
// ====================================================================
// data structure to store a segment

class Segment {

public:
  // CONSTRUCTOR
  Segment(const Ray &ray, float tstart, float tstop) {
    // first clamp the segment to "reasonable" values 
    // to make sure it is drawn correctly in OpenGL
    if (tstart < -1000) tstart = -1000;
    if (tstop  >  1000) tstop  =  1000;
    a = ray.pointAtParameter(tstart);
    b = ray.pointAtParameter(tstop); }
  const glm::vec3& getStart() const { return a; }
  const glm::vec3& getEnd() const { return b; }
private:
  // REPRESENTATION
  glm::vec3 a;
  glm::vec3 b;
};

// ====================================================================
// ====================================================================
//
// This class only contains static variables and static member
// functions so there is no need to call the constructor, destructor
// etc.  It's just a wrapper for the ray tree visualization data.
//

class RayTree {

public:

  // most of the time the RayTree is NOT activated, so the segments are not updated
  static void Activate() { Clear(); activated = 1; }
  static void Deactivate() { activated = 0; }

  // when activated, these function calls store the segments of the tree
  static void AddMainSegment(const Ray &ray, float tstart, float tstop) {
    if (!activated) return;
    main_segments.push_back(Segment(ray,tstart,tstop));
  }
  static void AddShadowSegment(const Ray &ray, float tstart, float tstop) {
    if (!activated) return;
    shadow_segments.push_back(Segment(ray,tstart,tstop));
  }
  static void AddReflectedSegment(const Ray &ray, float tstart, float tstop) {
    if (!activated) return;
    reflected_segments.push_back(Segment(ray,tstart,tstop));
  }
  static void AddTransmittedSegment(const Ray &ray, float tstart, float tstop) {
    if (!activated) return;
    transmitted_segments.push_back(Segment(ray,tstart,tstop));
  }

  static void initializeVBOs(); 
  static void setupVBOs(float width); 
  static void drawVBOs();
  static void cleanupVBOs();
  
private:
  
  // HELPER FUNCTIONS
  static void Clear() {
    main_segments.clear();
    shadow_segments.clear();
    reflected_segments.clear();
    transmitted_segments.clear();
  }
  
  // REPRESENTATION
  static int activated;
  static std::vector<Segment> main_segments;
  static std::vector<Segment> shadow_segments;
  static std::vector<Segment> reflected_segments;
  static std::vector<Segment> transmitted_segments;

  // VBO
  static GLuint raytree_verts_VBO;
  static GLuint raytree_edge_indices_VBO;
  static std::vector<VBOPosNormalColor> raytree_verts; 
  static std::vector<VBOIndexedTri> raytree_edge_indices;
};

// ====================================================================
// ====================================================================

#endif
