#include "vertex.h"
#include "edge.h"

// EDGE CONSTRUCTOR
Edge::Edge(Vertex *vs, Vertex *ve, Face *f) {
  start_vertex = vs;
  end_vertex = ve;
  face = f;
  next = NULL;
  opposite = NULL;
}

// EDGE DESTRUCTOR
Edge::~Edge() { 
  // disconnect from the opposite edge
  if (opposite != NULL)
    opposite->opposite = NULL;
  // NOTE: the "prev" edge (which has a "next" pointer pointing to
  // this edge) will also be deleted as part of the triangle removal,
  // so we don't need to disconnect that
}

float Edge::Length() const {
  glm::vec3 diff = start_vertex->get() - end_vertex->get();
  return glm::length(diff);
}
