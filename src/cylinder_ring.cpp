#include "utils.h"
#include "argparser.h"
#include "material.h"
#include "cylinder_ring.h"
#include "mesh.h"
#include "ray.h"
#include "hit.h"

// ====================================================================
// ====================================================================
// HELPER FUNCTIONS FOR RING INTERSECTION

bool IntersectFiniteCylinder(const Ray &r, const glm::vec3 &center, float radius, float height, float &t, glm::vec3 &normal) {
  // assumes cylinder is aligned with the y axis
  const glm::vec3 &ori = r.getOrigin();
  const glm::vec3 &dir = r.getDirection();

  // insert explict ray equation into implicit cylinder equation and
  // solve for t using the quadratic formula

  float A = dir.x*dir.x +dir.z*dir.z;
  float B = 2 * (dir.x*ori.x + dir.z*ori.z);
  float C = ori.x*ori.x + ori.z*ori.z - radius*radius;

  // if the solutions are imaginary, no intersection!
  float radical = B*B - 4*A*C;
  if (radical < EPSILON) return false;

  // there might be 2 solutions...
  radical = sqrt(radical);
  float t_m = (-B - radical) / (2*A);
  float t_p = (-B + radical) / (2*A);
  assert (t_m <= t_p);
  glm::vec3 pt_m = r.pointAtParameter(t_m);
  t = t_m;
  // select the correct solution, the smallest non-negative solution
  if (t_m < EPSILON ||
      pt_m.y > center.y + height/2.0 ||
      pt_m.y < center.y - height/2.0) {
    t = t_p;
  } 
  glm::vec3 pt = r.pointAtParameter(t);
  if (t < EPSILON ||
      pt.y > center.y + height/2.0 ||
      pt.y < center.y - height/2.0) {
    return false;
  }

  // compute the normal at the interesection point
  normal = glm::normalize(glm::vec3(pt.x-center.x,0,pt.z-center.z));
  return true;
}

bool IntersectAnnulus(const Ray &r, const glm::vec3 &center, float inner_radius, float outer_radius,
		      float &t, glm::vec3 &normal) {
  // assume annulus is parallel to the y = 0 plane
  const glm::vec3 &ori = r.getOrigin();
  const glm::vec3 &dir = r.getDirection();
  assert (glm::length(r.getDirection())  > 0.9);
  t = (center.y-ori.y) / dir.y;

  // check if intersection is behind the eye
  if (t < EPSILON) return false;

  // check to see that the interesection is "between" the two circles
  glm::vec3 pt = r.pointAtParameter(t);
  float test = sqrt((pt.x-center.x)*(pt.x-center.x)+
		    (pt.z-center.z)*(pt.z-center.z));
  if (test < inner_radius || test > outer_radius) return false;

  normal = glm::vec3(0,1,0);
  return true;
}


bool CylinderRing::intersect(const Ray &r, Hit &h) const {

  // intersect with the 4 parts of the ring
  float outer_t;
  glm::vec3 outer_normal;
  bool outer = IntersectFiniteCylinder(r,center,outer_radius,height,outer_t,outer_normal);
  float inner_t;
  glm::vec3 inner_normal;
  bool inner = IntersectFiniteCylinder(r,center,inner_radius,height,inner_t,inner_normal);
  float top_t;
  glm::vec3 top_normal;
  bool top = IntersectAnnulus(r,center+glm::vec3(0,height/2.0,0),inner_radius,outer_radius,top_t,top_normal);
  float bottom_t;
  glm::vec3 bottom_normal;
  bool bottom = IntersectAnnulus(r,center-glm::vec3(0,height/2.0,0),inner_radius,outer_radius,bottom_t,bottom_normal);
  bool answer = false;

  // return the closest intersection
  if (outer && (outer_t < h.getT())) {
    h.set(outer_t,this->getMaterial(),outer_normal);
    answer = true;
  }
  if (inner && (inner_t < h.getT())) {
    h.set(inner_t,this->getMaterial(),-inner_normal);
    answer = true;
  }
  if (top && (top_t < h.getT())) {
    h.set(top_t,this->getMaterial(),top_normal);
    answer = true;
  }
  if (bottom && (bottom_t < h.getT())) {
    h.set(bottom_t,this->getMaterial(),-bottom_normal);
    answer = true;
  }
  return answer;
} 

// ====================================================================
// ====================================================================

// helper function to place a grid of points on the cylinderRing
glm::vec3 ComputeCylinderRingPoint(float s, const glm::vec3 center, float radius, float height) {
  float angle = 2*M_PI*s;
  float x = radius*cos(angle);
  float z = radius*-sin(angle);
  glm::vec3 answer = center + glm::vec3(x,height,z);
  return answer;
}

void CylinderRing::addRasterizedFaces(Mesh *m, ArgParser *args) {
  int crr = args->cylinder_ring_rasterization;
  int i;
  int va,vb,vc,vd;
  Vertex *a,*b,*c,*d;
  int offset = m->numVertices(); 
  // place vertices
  for (i = 0; i < crr; i++) {
    float s = i / float(crr);
    m->addVertex(ComputeCylinderRingPoint(s,center,outer_radius,-height/2.0));
    m->addVertex(ComputeCylinderRingPoint(s,center,outer_radius,+height/2.0));
    m->addVertex(ComputeCylinderRingPoint(s,center,inner_radius,+height/2.0));
    m->addVertex(ComputeCylinderRingPoint(s,center,inner_radius,-height/2.0));
  }
  // the patches
  for (i = 0; i < crr; i++) {
    for (int j = 0; j < 4; j++) {
      va = 4*i + j;
      vb = 4*((i+1)%crr) + j;
      vc = 4*((i+1)%crr)+ (j+1)%4;
      vd = 4*i + (j+1)%4;
      a = m->getVertex(offset + va);
      b = m->getVertex(offset + vb);
      c = m->getVertex(offset + vc);
      d = m->getVertex(offset + vd);
      m->addRasterizedPrimitiveFace(a,b,c,d,material);
    }
  }
}
