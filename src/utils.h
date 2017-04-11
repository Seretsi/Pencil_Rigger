#ifndef _UTILS_H
#define _UTILS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "glCanvas.h"
#include "vbo_structs.h"
#include "argparser.h"

// ======================================================================

#define square(x) ((x)*(x))
// helper for VBOs
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// needed by Windows
// allows us to use std::min & std::max
#define NOMINMAX


// =========================================================================
// EPSILON is a necessary evil for raytracing implementations
// The appropriate value for epsilon depends on the precision of
// the floating point calculations on your hardware **AND** on the
// overall dimensions of the scene and your camera projection matrix.
#define EPSILON 0.0001


// =========================================================================
// These two functions convert between linear intensity values
// (approximate range 0->1) to an sRGB value (approximate range 0->1).
// The sRGB values make best use of 8 bit storage and are the best
// input for most displays and darkened viewing environments.

#define SRGB_ALPHA 0.055

inline float linear_to_srgb(float x) {
  float answer;
  if (x <= 0.0031308)
    answer = 12.92*x;
  else 
    answer = (1+SRGB_ALPHA)*(pow(x,1/2.4)-SRGB_ALPHA);
  return answer;
}

inline float srgb_to_linear(float x) {
  float answer;
  if (x <= 0.04045)
    answer = x/12.92;
  else 
    answer = pow((x+SRGB_ALPHA)/(1+SRGB_ALPHA),2.4);
  return answer;
}

// =========================================================================
// utility functions 
inline float DistanceBetweenTwoPoints(const glm::vec3 &p1, const glm::vec3 &p2) {
  glm::vec3 v = p1-p2;
  return glm::length(v);
}

inline float AreaOfTriangle(float a, float b, float c) {
  // from the lengths of the 3 edges, compute the area
  // Area of Triangle = (using Heron's Formula)
  //  sqrt[s*(s-a)*(s-b)*(s-c)]
  //    where s = (a+b+c)/2
  // also... Area of Triangle = 0.5 * x * c
  float s = (a+b+c) / (float)2;
  return sqrt(s*(s-a)*(s-b)*(s-c));
}

inline float AreaOfTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
  float aside = DistanceBetweenTwoPoints(a,b);
  float bside = DistanceBetweenTwoPoints(b,c);
  float cside = DistanceBetweenTwoPoints(c,a);
  return AreaOfTriangle(aside,bside,cside);
}

// utility function to generate random numbers used for sampling
inline glm::vec3 RandomUnitVector() {
  glm::vec3 tmp;
  while (true) {
    tmp = glm::vec3(2*GLCanvas::args->rand()-1,  // random real in [-1,1]
                    2*GLCanvas::args->rand()-1,  // random real in [-1,1]
                    2*GLCanvas::args->rand()-1); // random real in [-1,1]
    if (glm::length(tmp) < 1) break;
  }
  tmp = glm::normalize(tmp);
  return tmp;
}

// compute the perfect mirror direction
inline glm::vec3 MirrorDirection(const glm::vec3 &normal, const glm::vec3 &incoming) {
  float dot = glm::dot(incoming,normal);
  glm::vec3 r = (incoming*-1.0f) + normal * (2 * dot);
  return r*-1.0f;
}

// compute a random diffuse direction
// (not the same as a uniform random direction on the hemisphere)
inline glm::vec3 RandomDiffuseDirection(const glm::vec3 &normal) {
  glm::vec3 answer = normal+RandomUnitVector();
  return glm::normalize(answer);
}

void addEdgeGeometry(std::vector<VBOPosNormalColor> &verts,
                     std::vector<VBOIndexedTri> &tri_indices,
                     const glm::vec3 &a, const glm::vec3 &b, 
                     const glm::vec4 &acolor, const glm::vec4 &bcolor, 
                     float a_th,float b_th);


#endif
