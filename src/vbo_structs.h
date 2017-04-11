#ifndef __VBO_STRUCTS_H__
#define __VBO_STRUCTS_H__

// ======================================================================
// helper structures for VBOs, for rendering (note, the data stored in
// each of these is application specific, adjust as needed!)


struct VBOPosNormalColor {

  VBOPosNormalColor(const glm::vec3 &p, const glm::vec3 &n, const glm::vec4 &c) {
    x = p.x; y = p.y; z = p.z;
    nx = n.x; ny = n.y; nz = n.z;
    r  =  c.x;  g =  c.y;  b =  c.z; a = c.z;
    wr = 1; wg = 1; wb = 1; wa = 1;
    s = 0;
    t = 0;
  }

  VBOPosNormalColor(const glm::vec3 &p, const glm::vec3 &n, const glm::vec4 &c, const glm::vec4 &wc, float s_, float t_) {
    x = p.x; y = p.y; z = p.z;
    nx = n.x; ny = n.y; nz = n.z;
    r  =  c.x;  g =  c.y;  b =  c.z;  a = c.a;
    wr = wc.x; wg = wc.y; wb = wc.z; wa = wc.a;
    s = s_;
    t = t_;
  }

  float x, y, z;         // position
  float nx, ny, nz;      // normal
  float r, g, b, a;      // color
  float wr, wg, wb, wa;  // wireframe color
  float s, t;            // texture coordinates
};


// ======================================================================

struct VBOIndexedTri {
  VBOIndexedTri() {}
  VBOIndexedTri(unsigned int a, unsigned int b, unsigned int c) {
    verts[0] = a;
    verts[1] = b;
    verts[2] = c;
  }
  unsigned int verts[3];
};


// ======================================================================

#endif
