#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <glm/glm.hpp>

class Mesh;
class Ray;
class Hit;
class Material;
class ArgParser;

// ====================================================================
// The base class for implicit object representations.  These objects
// can be intersected with a ray for ray tracing and also be
// rasterized for OpenGL and for use in radiosity.

class Primitive {
 public:
  virtual ~Primitive() {}

  // accessor
  Material* getMaterial() const { return material; }

  // for ray tracing
  virtual bool intersect(const Ray &r, Hit &h) const = 0;

  // for OpenGL rendering & radiosity
  virtual void addRasterizedFaces(Mesh *m, ArgParser *args) = 0;

 protected:
  // REPRESENTATION
  Material *material;
};

#endif
