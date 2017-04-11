#include "raytree.h"
#include "utils.h"

// ====================================================================
// Initialize the static variables
int RayTree::activated = 0;  
std::vector<Segment> RayTree::main_segments;
std::vector<Segment> RayTree::shadow_segments;
std::vector<Segment> RayTree::reflected_segments;
std::vector<Segment> RayTree::transmitted_segments;

GLuint RayTree::raytree_verts_VBO;
GLuint RayTree::raytree_edge_indices_VBO;
std::vector<VBOPosNormalColor> RayTree::raytree_verts; 
std::vector<VBOIndexedTri> RayTree::raytree_edge_indices;

// ====================================================================

void RayTree::initializeVBOs() {
  glGenBuffers(1, &raytree_verts_VBO);
  glGenBuffers(1, &raytree_edge_indices_VBO);
}

void RayTree::setupVBOs(float width) {
  HandleGLError("enter ray treesetup"); 
  raytree_verts.clear();
  raytree_edge_indices.clear();

  glm::vec4 main_color(0.7,0.7,0.7,0.7);
  glm::vec4 shadow_color(0.1,0.9,0.1,0.7);
  glm::vec4 reflected_color(0.9,0.1,0.1,0.7);
  glm::vec4 transmitted_color(0.1,0.1,0.9,0.7);

  // initialize the data
  unsigned int i;
  for (i = 0; i < main_segments.size(); i++) {
    addEdgeGeometry(raytree_verts,raytree_edge_indices,
                    main_segments[i].getStart(),
                    main_segments[i].getEnd(),
                    main_color,main_color,
                    width,width);
  }
  for (i = 0; i < shadow_segments.size(); i++) {
    addEdgeGeometry(raytree_verts,raytree_edge_indices,
                    shadow_segments[i].getStart(),
                    shadow_segments[i].getEnd(),
                    shadow_color,shadow_color,
                    width,width);
  }
  for (i = 0; i < reflected_segments.size(); i++) {
    addEdgeGeometry(raytree_verts,raytree_edge_indices,
                    reflected_segments[i].getStart(),
                    reflected_segments[i].getEnd(),
                    reflected_color,reflected_color,
                    width,width);
  }
  for (i = 0; i < transmitted_segments.size(); i++) {
    addEdgeGeometry(raytree_verts,raytree_edge_indices,
                    transmitted_segments[i].getStart(),
                    transmitted_segments[i].getEnd(),
                    transmitted_color,transmitted_color,
                    width,width);
  }

  assert (2*raytree_edge_indices.size() == raytree_verts.size());

  // copy the data to each VBO
  glBindBuffer(GL_ARRAY_BUFFER,raytree_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(VBOPosNormalColor) * raytree_verts.size(),
	       &raytree_verts[0],
	       GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,raytree_edge_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       sizeof(VBOIndexedTri) * raytree_edge_indices.size(),
	       &raytree_edge_indices[0], GL_STATIC_DRAW);

  HandleGLError("ray treesetup leaving"); 
}

void RayTree::drawVBOs() {
  if (raytree_edge_indices.size() > 0) {
    // no local shading (lighting)!
    glUniform1i(GLCanvas::colormodeID, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER,raytree_verts_VBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,raytree_edge_indices_VBO); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2 + sizeof(glm::vec4)));
    glDrawElements(GL_TRIANGLES,
                   raytree_edge_indices.size()*3,
                   GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
  }
}

void RayTree::cleanupVBOs() {
  glDeleteBuffers(1, &raytree_verts_VBO);
  glDeleteBuffers(1, &raytree_edge_indices_VBO);
}
