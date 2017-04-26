#include "glCanvas.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <utility>
#include <iterator>
#include <ios>
#include <omp.h>
#include <sstream>


#include "argparser.h"
#include "vertex.h"
#include "boundingbox.h"
#include "mesh.h"
#include "edge.h"
#include "face.h"
#include "primitive.h"
#include "sphere.h"
#include "cylinder_ring.h"
#include "ray.h"
#include "hit.h"
#include "camera.h"


// =======================================================================
// DESTRUCTOR
// =======================================================================

Mesh::~Mesh() {
  unsigned int i;
  for (i = 0; i < rasterized_primitive_faces.size(); i++) {
    Face *f = rasterized_primitive_faces[i];
    removeFaceEdges(f);
    delete f;
  }
  if (subdivided_quads.size() != original_quads.size()) {
    for (i = 0; i < subdivided_quads.size(); i++) {
      Face *f = subdivided_quads[i];
      removeFaceEdges(f);
      delete f;
    }
  }
  for (i = 0; i < original_quads.size(); i++) {
    Face *f = original_quads[i];
    removeFaceEdges(f);
    delete f;
  }
  for (i = 0; i < primitives.size(); i++) { delete primitives[i]; }
  for (i = 0; i < materials.size(); i++) { delete materials[i]; }
  for (i = 0; i < vertices.size(); i++) { delete vertices[i]; }
  delete bbox;
}

// =======================================================================
// MODIFIERS:   ADD & REMOVE
// =======================================================================

Vertex* Mesh::addVertex(const glm::vec3 &position) {
  int index = numVertices();
  vertices.push_back(new Vertex(index,position));
  // extend the bounding box to include this point
  if (bbox == NULL) 
    bbox = new BoundingBox(position,position);
  else 
    bbox->Extend(position);
  return vertices[index];
}

void Mesh::addPrimitive(Primitive* p) {
  primitives.push_back(p);
  p->addRasterizedFaces(this,args);
}

void Mesh::addFace(Vertex *a, Vertex *b, Vertex *c, Vertex *d, Material *material, enum FACE_TYPE face_type) {
  // create the face
  Face *f = new Face(material);
  // create the edges
  Edge *ea = new Edge(a,b,f);
  Edge *eb = new Edge(b,c,f);
  Edge *ec = new Edge(c,d,f);
  Edge *ed = new Edge(d,a,f);
  // point the face to one of its edges
  f->setEdge(ea);
  // connect the edges to each other
  ea->setNext(eb);
  eb->setNext(ec);
  ec->setNext(ed);
  ed->setNext(ea);
  // verify these edges aren't already in the mesh 
  // (which would be a bug, or a non-manifold mesh)
  assert (edges.find(std::make_pair(a,b)) == edges.end());
  assert (edges.find(std::make_pair(b,c)) == edges.end());
  assert (edges.find(std::make_pair(c,d)) == edges.end());
  assert (edges.find(std::make_pair(d,a)) == edges.end());
  // add the edges to the master list
  edges[std::make_pair(a,b)] = ea;
  edges[std::make_pair(b,c)] = eb;
  edges[std::make_pair(c,d)] = ec;
  edges[std::make_pair(d,a)] = ed;
  // connect up with opposite edges (if they exist)
  edgeshashtype::iterator ea_op = edges.find(std::make_pair(b,a)); 
  edgeshashtype::iterator eb_op = edges.find(std::make_pair(c,b)); 
  edgeshashtype::iterator ec_op = edges.find(std::make_pair(d,c)); 
  edgeshashtype::iterator ed_op = edges.find(std::make_pair(a,d)); 
  if (ea_op != edges.end()) { ea_op->second->setOpposite(ea); }
  if (eb_op != edges.end()) { eb_op->second->setOpposite(eb); }
  if (ec_op != edges.end()) { ec_op->second->setOpposite(ec); }
  if (ed_op != edges.end()) { ed_op->second->setOpposite(ed); }
  // add the face to the appropriate master list
  if (face_type == FACE_TYPE_ORIGINAL) {
    original_quads.push_back(f);
    subdivided_quads.push_back(f);
  } else if (face_type == FACE_TYPE_RASTERIZED) {
    rasterized_primitive_faces.push_back(f); 
  } else {
    assert (face_type == FACE_TYPE_SUBDIVIDED);
    subdivided_quads.push_back(f);
  }
  // if it's a light, add it to that list too
  if (glm::length(material->getEmittedColor()) > 0 && face_type == FACE_TYPE_ORIGINAL) {
    original_lights.push_back(f);
  }
}

void Mesh::removeFaceEdges(Face *f) {
  // helper function for face deletion
  Edge *ea = f->getEdge();
  Edge *eb = ea->getNext();
  Edge *ec = eb->getNext();
  Edge *ed = ec->getNext();
  assert (ed->getNext() == ea);
  Vertex *a = ea->getStartVertex();
  Vertex *b = eb->getStartVertex();
  Vertex *c = ec->getStartVertex();
  Vertex *d = ed->getStartVertex();
  // remove elements from master lists
  edges.erase(std::make_pair(a,b)); 
  edges.erase(std::make_pair(b,c)); 
  edges.erase(std::make_pair(c,d)); 
  edges.erase(std::make_pair(d,a)); 
  // clean up memory
  delete ea;
  delete eb;
  delete ec;
  delete ed;
}

// ==============================================================================
// EDGE HELPER FUNCTIONS

Edge* Mesh::getEdge(Vertex *a, Vertex *b) const {
  edgeshashtype::const_iterator iter = edges.find(std::make_pair(a,b));
  if (iter == edges.end()) return NULL;
  return iter->second;
}

Vertex* Mesh::getChildVertex(Vertex *p1, Vertex *p2) const {
  vphashtype::const_iterator iter = vertex_parents.find(std::make_pair(p1,p2)); 
  if (iter == vertex_parents.end()) return NULL;
  return iter->second; 
}

void Mesh::setParentsChild(Vertex *p1, Vertex *p2, Vertex *child) {
  assert (vertex_parents.find(std::make_pair(p1,p2)) == vertex_parents.end());
  vertex_parents[std::make_pair(p1,p2)] = child; 
}

//
// ===============================================================================
// the load function parses our (non-standard) extension of very simple .obj files
// ===============================================================================

void Mesh::Parallel(ArgParser *_args) {
  //set up filename, open up original ifstream for counting number of points
  args = _args;
  std::string file = args->path+'/'+args->input_file;
  std::ifstream objfile(file.c_str());
  if (!objfile.good()) {
    return;
  }
  int num_verts = 0;
  std::string token;
  do {
    getline(objfile, token);
    num_verts++;
  }
  while ((token != "")&&(token != "\n"));

  //now, set it back to the beginning. You still need to write the code for this one to parse the extra lines in front.

  objfile.close();
  objfile.open(file);
  
  //calculate the number of lines, etc for each thread to handle
  const int nt = 8;
  int extra_lines = num_verts%nt;
  int lpt = num_verts/nt;
  std::cout << "number of verts: " << num_verts << "\n";
  std::cout << "lines per thread: " << lpt << "\n";
  std::cout << "extra lines at beginning: " << extra_lines << "\n";
  std::cout << "opening extra file pointers...\n";
  //each thread has its own ifstream
  std::ifstream fp[nt];
  for (int i = 0; i < nt; i++) {
    //open new ifstream and seek it to the start of the points it will read
    fp[i].open(file, std::ifstream::in);
    int temp_ct = 0;
    std::string tok;
    while (temp_ct < extra_lines+i*lpt) {
      //just skip the line and inc
      getline(fp[i],tok);
      temp_ct++;
    }
    std::cout << "thread " << i << " will start at line " << temp_ct << "\n";
  }
  int lock = 0;
#pragma omp parallel for num_threads(nt)
  for (int i = 0; i < num_verts; i++) {
    //each thread will getline with its respective fp
    //need to add a lock for push_back to make sure shit doesn't hit the fan
    int tn = omp_get_thread_num();
    std::string tok;
    getline(fp[tn], tok);
    std::string toss_out;
    float x, y, z;
    std::stringstream ss(tok, std::ios_base::in);
    ss >> toss_out >> x >> y >> z;
    glm::vec3 pt(x,y,z);
    // while (lock);
    // lock = 1;
    #pragma omp critical
    {
    addVertex(pt);
    //lock = 0;
    }
  }
  std::cout << "finished reading verts\n";
  // there should be a line of space between these
  // advance the objfile to count the number of face lines
  do {
    getline(objfile, token);
  }
  while ((token != "")&&(token != "\n"));

  //now find number of faces
  num_verts = 0;
  do {
    getline(objfile, token);
    std::cout << token << std::endl;
    num_verts++;
  }
  while ((token != "")&&(token != "\n"));
  std::cout << "number of face lines: " << num_verts;
  //compute thread info
  extra_lines = num_verts%nt;
  lpt = num_verts/nt;
  std::cout << "number of faces: " << num_verts << std::endl;
  std::cout << "number of extra lines: " << extra_lines << std::endl;
  std::cout << "number of lines per thread: " << lpt << std::endl;
  //advance threads to their starting positions
  for (int i = 0; i < nt; i++) {
    std::string tok;
    if ( i != nt -1) {
      do {
        getline(fp[i],tok);
      }
      while ((tok != "") && (tok != "\n"));
    }
    int t = 0;
    while (t < i*lpt+extra_lines) {
      getline(fp[i], tok);
      t++;
    }
    std::cout << "thread " << i << ": " << i*lpt+extra_lines << std::endl;
  }
  
  Material* active_material = new Material("",glm::vec3(0.5,0.5,0.5), glm::vec3(1,1,1), glm::vec3(0,0,0), 0.3);
#pragma omp parallel for num_threads(nt)
  for (int i = 0; i < num_verts; i++) {
    int tn = omp_get_thread_num();
    std::string tok;
    getline(fp[tn], tok);
    std::string ignore;
    int f, s, t, fr;
    if (tok == "") continue;
    std::stringstream ss(tok, std::ios_base::in);
    ss >> ignore >> f >> s >> t >> fr;
    f--;s--;t--;fr--;
    #pragma omp critical
    {
      //std::cerr << "line " << tn << ": " << tok << std::endl;
      //std::cerr << "thread " << tn << ": " << f << " " << s << " " << t << " " << fr << "\n";
      std::cerr << "thread " << tn <<  ": number of shits read: " << i%lpt << std::endl;
      addOriginalQuad(getVertex(f),getVertex(s),getVertex(t),getVertex(fr),active_material);
    }
  }
  std::cout << "finished assembling faces\n";
}


void Mesh::Load(ArgParser *_args) {
  args = _args;

  std::string file = args->path+'/'+args->input_file;

  std::ifstream objfile(file.c_str());
  if (!objfile.good()) {
    std::cout << "ERROR! CANNOT OPEN " << file << std::endl;
    return;
  }

  std::string token;
  Material *active_material = NULL;
  active_material = new Material("",glm::vec3(0.5,0.5,0.5), glm::vec3(1,1,1), glm::vec3(0,0,0), 0.3);
  camera = NULL;
  background_color = glm::vec3(1,1,1);

 
  while (objfile >> token) {
    if (token == "v") {
      float x,y,z;
      objfile >> x >> y >> z;
      addVertex(glm::vec3(x,y,z));
    } else if (token == "vt") {
      assert (numVertices() >= 1);
      float s,t;
      objfile >> s >> t;
      getVertex(numVertices()-1)->setTextureCoordinates(s,t);
    } else if (token == "f") {
      int a,b,c,d;
      objfile >> a >> b >> c >> d;
      a--;
      b--;
      c--;
      d--;
      assert (a >= 0 && a < numVertices());
      assert (b >= 0 && b < numVertices());
      assert (c >= 0 && c < numVertices());
      assert (d >= 0 && d < numVertices());
      assert (active_material != NULL);
      addOriginalQuad(getVertex(a),getVertex(b),getVertex(c),getVertex(d),active_material);
    } else if (token == "s") {
      float x,y,z,r;
      objfile >> x >> y >> z >> r;
      assert (active_material != NULL);
      addPrimitive(new Sphere(glm::vec3(x,y,z),r,active_material));
    } else if (token == "r") {
      float x,y,z,h,r,r2;
      objfile >> x >> y >> z >> h >> r >> r2;
      assert (active_material != NULL);
      addPrimitive(new CylinderRing(glm::vec3(x,y,z),h,r,r2,active_material));
    } else if (token == "background_color") {
      float r,g,b;
      objfile >> r >> g >> b;
      background_color = glm::vec3(r,g,b);
    } else if (token == "PerspectiveCamera") {
      camera = new PerspectiveCamera();
      objfile >> *(PerspectiveCamera*)camera;
    } else if (token == "OrthographicCamera") {
      camera = new OrthographicCamera();
      objfile >> *(OrthographicCamera*)camera;
    } else if (token == "m") {
      // this is not standard .obj format!!
      // materials
      int m;
      objfile >> m;
      assert (m >= 0 && m < (int)materials.size());
      active_material = materials[m];
    } else if (token == "material") {
      // this is not standard .obj format!!
      std::string texture_file = "";
      glm::vec3 diffuse(0,0,0);
      float r,g,b;
      objfile >> token;
      if (token == "diffuse") {
	objfile >> r >> g >> b;
	diffuse = glm::vec3(r,g,b);
      } else {
	assert (token == "texture_file");
	objfile >> texture_file;
	// prepend the directory name
	texture_file = args->path + '/' + texture_file;
      }
      glm::vec3 reflective,emitted;      
      objfile >> token >> r >> g >> b;
      assert (token == "reflective");
      reflective = glm::vec3(r,g,b);
      float roughness = 0;
      objfile >> token;
      if (token == "roughness") {
	objfile >> roughness;
	objfile >> token;
      } 
      assert (token == "emitted");
      objfile >> r >> g >> b;
      emitted = glm::vec3(r,g,b);
      materials.push_back(new Material(texture_file,diffuse,reflective,emitted,roughness));
    } else {
      std::cout << "UNKNOWN TOKEN " << token << std::endl;
      exit(0);
    }
  }
  std::cout << " mesh loaded: " << numFaces() << " faces and " << numEdges() << " edges." << std::endl;

  if (camera == NULL) {
    // if not initialized, position a perspective camera and scale it so it fits in the window
    assert (bbox != NULL);
    glm::vec3 point_of_interest; bbox->getCenter(point_of_interest);
    float max_dim = bbox->maxDim();
    glm::vec3 camera_position = point_of_interest + glm::vec3(0,0,4*max_dim);
    glm::vec3 up = glm::vec3(0,1,0);
    camera = new PerspectiveCamera(camera_position, point_of_interest, up, 20 * 3.14159265359 /180.0);
  }
}

// =================================================================
// SUBDIVISION
// =================================================================

Vertex* Mesh::AddEdgeVertex(Vertex *a, Vertex *b) {
  Vertex *v = getChildVertex(a,b);
  if (v != NULL) return v;
  glm::vec3 pos = 0.5f*a->get() + 0.5f*b->get();
  float s = 0.5f*a->get_s() + 0.5f*b->get_s();
  float t = 0.5f*a->get_t() + 0.5f*b->get_t();
  v = addVertex(pos);
  v->setTextureCoordinates(s,t);
  setParentsChild(a,b,v);
  return v;
}

Vertex* Mesh::AddMidVertex(Vertex *a, Vertex *b, Vertex *c, Vertex *d) {
  glm::vec3 pos = 0.25f*a->get() + 0.25f*b->get() + 0.25f*c->get() + 0.25f*d->get();
  float s = 0.25f*a->get_s() + 0.25f*b->get_s() + 0.25f*c->get_s() + 0.25f*d->get_s();
  float t = 0.25f*a->get_t() + 0.25f*b->get_t() + 0.25f*c->get_t() + 0.25f*d->get_t();
  Vertex *v = addVertex(pos);
  v->setTextureCoordinates(s,t);
  return v;
}

void Mesh::Subdivision() {

  bool first_subdivision = false;
  if (original_quads.size() == subdivided_quads.size()) {
    first_subdivision = true;
  }

  std::vector<Face*> tmp = subdivided_quads;
  subdivided_quads.clear();
  
  for (unsigned int i = 0; i < tmp.size(); i++) {
    Face *f = tmp[i];
    
    Vertex *a = (*f)[0];
    Vertex *b = (*f)[1];
    Vertex *c = (*f)[2];
    Vertex *d = (*f)[3];
    // add new vertices on the edges
    Vertex *ab = AddEdgeVertex(a,b);
    Vertex *bc = AddEdgeVertex(b,c);
    Vertex *cd = AddEdgeVertex(c,d);
    Vertex *da = AddEdgeVertex(d,a);
    // add new point in the middle of the patch
    Vertex *mid = AddMidVertex(a,b,c,d);

    assert (getEdge(a,b) != NULL);
    assert (getEdge(b,c) != NULL);
    assert (getEdge(c,d) != NULL);
    assert (getEdge(d,a) != NULL);

    // copy the color and emission from the old patch to the new
    Material *material = f->getMaterial();
    if (!first_subdivision) {
      removeFaceEdges(f);
      delete f;
    }

    // create the new faces
    addSubdividedQuad(a,ab,mid,da,material);
    addSubdividedQuad(b,bc,mid,ab,material);
    addSubdividedQuad(c,cd,mid,bc,material);
    addSubdividedQuad(d,da,mid,cd,material);

    assert (getEdge(a,ab) != NULL);
    assert (getEdge(ab,b) != NULL);
    assert (getEdge(b,bc) != NULL);
    assert (getEdge(bc,c) != NULL);
    assert (getEdge(c,cd) != NULL);
    assert (getEdge(cd,d) != NULL);
    assert (getEdge(d,da) != NULL);
    assert (getEdge(da,a) != NULL);
  }
}

