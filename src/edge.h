#ifndef _EDGE_H_
#define _EDGE_H_

#include <cassert>
#include <cstdlib>

class Vertex;
class Face;

// ===================================================================
// half-edge data structure

class Edge { 

public:

  // ========================
  // CONSTRUCTORS & DESTRUCTOR
  Edge(Vertex *vs, Vertex *ve, Face *f);
  ~Edge();

  // =========
  // ACCESSORS
  Vertex* getStartVertex() const { assert (start_vertex != NULL); return start_vertex; }
  Vertex* getEndVertex() const { assert (end_vertex != NULL); return end_vertex; }
  Edge* getNext() const { assert (next != NULL); return next; }
  Face* getFace() const { assert (face != NULL); return face; }
  Edge* getOpposite() const {
    // warning!  the opposite edge might be NULL!
    return opposite; }
  float Length() const;

  // =========
  // MODIFIERS
  void setOpposite(Edge *e) {
    assert (opposite == NULL); 
    assert (e != NULL);
    assert (e->opposite == NULL);
    opposite = e; 
    e->opposite = this; 
  }
  void clearOpposite() { 
    if (opposite == NULL) return; 
    assert (opposite->opposite == this); 
    opposite->opposite = NULL;
    opposite = NULL; 
  }
  void setNext(Edge *e) {
    assert (next == NULL);
    assert (e != NULL);
    assert (face == e->face);
    next = e;
  }

private:

  Edge(const Edge&) { assert(0); }
  Edge& operator=(const Edge&) { assert(0); exit(0); }

  // ==============
  // REPRESENTATION
  // in the half edge data adjacency data structure, the edge stores everything!
  // note: it's technically not necessary to store both vertices, but it makes
  //   dealing with non-closed meshes easier
  Vertex *start_vertex;
  Vertex *end_vertex;
  Face *face;
  Edge *opposite;
  Edge *next;
};

// ===================================================================

#endif

