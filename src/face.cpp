#include "face.h"
#include "utils.h"

// =========================================================================
// =========================================================================

float Face::getArea() const {
  glm::vec3 a = (*this)[0]->get();
  glm::vec3 b = (*this)[1]->get();
  glm::vec3 c = (*this)[2]->get();
  glm::vec3 d = (*this)[3]->get();
  return 
    AreaOfTriangle(DistanceBetweenTwoPoints(a,b),
                   DistanceBetweenTwoPoints(a,c),
                   DistanceBetweenTwoPoints(b,c)) +
    AreaOfTriangle(DistanceBetweenTwoPoints(c,d),
                   DistanceBetweenTwoPoints(a,d),
                   DistanceBetweenTwoPoints(a,c));
}

// =========================================================================

glm::vec3 Face::RandomPoint() const {
  glm::vec3 a = (*this)[0]->get();
  glm::vec3 b = (*this)[1]->get();
  glm::vec3 c = (*this)[2]->get();
  glm::vec3 d = (*this)[3]->get();

  float s = GLCanvas::args->rand(); // random real in [0,1]
  float t = GLCanvas::args->rand(); // random real in [0,1]

  glm::vec3 answer = s*t*a + s*(1-t)*b + (1-s)*t*d + (1-s)*(1-t)*c;
  return answer;
}

// =========================================================================
// the intersection routines

bool Face::intersect(const Ray &r, Hit &h, bool intersect_backfacing) const {
  // intersect with each of the subtriangles
  Vertex *a = (*this)[0];
  Vertex *b = (*this)[1];
  Vertex *c = (*this)[2];
  Vertex *d = (*this)[3];
  return triangle_intersect(r,h,a,b,c,intersect_backfacing) || triangle_intersect(r,h,a,c,d,intersect_backfacing);
}

bool Face::triangle_intersect(const Ray &r, Hit &h, Vertex *a, Vertex *b, Vertex *c, bool intersect_backfacing) const {

  // compute the intersection with the plane of the triangle
  Hit h2 = Hit(h);
  if (!plane_intersect(r,h2,intersect_backfacing)) return 0;  

  // figure out the barycentric coordinates:
  glm::vec3 Ro = r.getOrigin();
  glm::vec3 Rd = r.getDirection();
  // [ ax-bx   ax-cx  Rdx ][ beta  ]     [ ax-Rox ] 
  // [ ay-by   ay-cy  Rdy ][ gamma ]  =  [ ay-Roy ] 
  // [ az-bz   az-cz  Rdz ][ t     ]     [ az-Roz ] 
  // solve for beta, gamma, & t using Cramer's rule
  
  glm::mat3 detA_mat(a->get().x-b->get().x,a->get().x-c->get().x,Rd.x,
                     a->get().y-b->get().y,a->get().y-c->get().y,Rd.y,
                     a->get().z-b->get().z,a->get().z-c->get().z,Rd.z);
  float detA = glm::determinant(detA_mat);

  if (fabs(detA) <= 0.000001) return 0;
  assert (fabs(detA) >= 0.000001);
  
  glm::mat3 beta_mat(a->get().x-Ro.x,a->get().x-c->get().x,Rd.x,
                     a->get().y-Ro.y,a->get().y-c->get().y,Rd.y,
                     a->get().z-Ro.z,a->get().z-c->get().z,Rd.z);
  
  glm::mat3 gamma_mat(a->get().x-b->get().x,a->get().x-Ro.x,Rd.x,
                      a->get().y-b->get().y,a->get().y-Ro.y,Rd.y,
                      a->get().z-b->get().z,a->get().z-Ro.z,Rd.z);

  float beta = glm::determinant(beta_mat) / detA;
  float gamma = glm::determinant(gamma_mat) / detA;

  if (beta >= -0.00001 && beta <= 1.00001 &&
      gamma >= -0.00001 && gamma <= 1.00001 &&
      beta + gamma <= 1.00001) {
    h = h2;
    // interpolate the texture coordinates
    float alpha = 1 - beta - gamma;
    float t_s = alpha * a->get_s() + beta * b->get_s() + gamma * c->get_s();
    float t_t = alpha * a->get_t() + beta * b->get_t() + gamma * c->get_t();
    h.setTextureCoords(t_s,t_t);
    assert (h.getT() >= EPSILON);
    return 1;
  }

  return 0;
}


bool Face::plane_intersect(const Ray &r, Hit &h, bool intersect_backfacing) const {

  // insert the explicit equation for the ray into the implicit equation of the plane

  // equation for a plane
  // ax + by + cz = d;
  // normal . p + direction = 0
  // plug in ray
  // origin + direction * t = p(t)
  // origin . normal + t * direction . normal = d;
  // t = d - origin.normal / direction.normal;

  glm::vec3 normal = computeNormal();
  float d = glm::dot(normal,(*this)[0]->get());

  float numer = d - glm::dot(r.getOrigin(),normal);
  float denom = glm::dot(r.getDirection(),normal);

  if (denom == 0) return 0;  // parallel to plane

  if (!intersect_backfacing && glm::dot(normal,r.getDirection()) >= 0) 
    return 0; // hit the backside

  float t = numer / denom;
  if (t > EPSILON && t < h.getT()) {
    h.set(t,this->getMaterial(),normal);
    assert (h.getT() >= EPSILON);
    return 1;
  }
  return 0;
}


inline glm::vec3 ComputeNormal(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
  glm::vec3 v12 = p2;
  v12 -= p1;
  glm::vec3 v23 = p3;
  v23 -= p2;
  glm::vec3 normal = glm::normalize(glm::cross(v12,v23));
  return normal;
}

glm::vec3 Face::computeNormal() const {
  // note: this face might be non-planar, so average the two triangle normals
  glm::vec3 a = (*this)[0]->get();
  glm::vec3 b = (*this)[1]->get();
  glm::vec3 c = (*this)[2]->get();
  glm::vec3 d = (*this)[3]->get();
  return 0.5f * (ComputeNormal(a,b,c) + ComputeNormal(a,c,d));
}

