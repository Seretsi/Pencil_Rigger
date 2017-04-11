#include "kdtree.h"
#include "utils.h"

#define MAX_PHOTONS_BEFORE_SPLIT 100
#define MAX_DEPTH 15

// ==================================================================
// DESTRUCTOR
// ==================================================================
KDTree::~KDTree() {
  if (!isLeaf()) {
    delete child1;
    delete child2;
  } else {
    // delete all the photons (this is done automatically since they
    // are stored directly in an STL vector, not using pointers)
  }
}


// ==================================================================
// HELPER FUNCTIONS

bool KDTree::PhotonInCell(const Photon &p) {
  const glm::vec3& min = bbox.getMin();
  const glm::vec3& max = bbox.getMax();
  const glm::vec3 &position = p.getPosition();
  if (position.x > min.x - EPSILON &&
      position.y > min.y - EPSILON &&
      position.z > min.z - EPSILON &&
      position.x < max.x + EPSILON &&
      position.y < max.y + EPSILON &&
      position.z < max.z + EPSILON)
    return true;
  return false;
}

bool KDTree::overlaps(const BoundingBox &bb) const {
  const glm::vec3& bb_min = bb.getMin();
  const glm::vec3& bb_max = bb.getMax();
  const glm::vec3& tmp_min = bbox.getMin();
  const glm::vec3& tmp_max = bbox.getMax();
  if (bb_min.x > tmp_max.x) return false;
  if (tmp_min.x > bb_max.x) return false;
  if (bb_min.y > tmp_max.y) return false;
  if (tmp_min.y > bb_max.y) return false;
  if (bb_min.z > tmp_max.z) return false;
  if (tmp_min.z > bb_max.z) return false;
  return true;
}


// ==================================================================
void KDTree::AddPhoton(const Photon &p) {
  const glm::vec3 &position = p.getPosition();
  assert (PhotonInCell(p));
  if (isLeaf()) {
    // this cell is a leaf node
    photons.push_back(p);
    if (photons.size() > MAX_PHOTONS_BEFORE_SPLIT && depth < MAX_DEPTH) {
      SplitCell();
    }
  } else {
    // this cell is not a leaf node
    // decide which subnode to recurse into
    if (split_axis == 0) {
      if (position.x < split_value)
	child1->AddPhoton(p);
      else
	child2->AddPhoton(p);
    } else if (split_axis == 1) {
      if (position.y < split_value)
	child1->AddPhoton(p);
      else
	child2->AddPhoton(p);
    } else {
      assert (split_axis == 2);
      if (position.z < split_value)
	child1->AddPhoton(p);
      else
	child2->AddPhoton(p);
    }
  }
}


// ==================================================================
void KDTree::CollectPhotonsInBox(const BoundingBox &bb, std::vector<Photon> &photons) const {
  // explicitly store the queue of cells that must be checked (rather
  // than write a recursive function)
  std::vector<const KDTree*> todo;  
  todo.push_back(this);
  while (!todo.empty()) {
    const KDTree *node = todo.back();
    todo.pop_back(); 
    if (!node->overlaps(bb)) continue;
    if (node->isLeaf()) {
      // if this cell overlaps & is a leaf, add all of the photons into the master list
      // NOTE: these photons may not be inside of the query bounding box
      const std::vector<Photon> &photons2 = node->getPhotons();
      int num_photons = photons2.size();
      for (int i = 0; i < num_photons; i++) {
	photons.push_back(photons2[i]);
      }
    } else {
      // if this cell is not a leaf, explore both children
      todo.push_back(node->getChild1());
      todo.push_back(node->getChild2());
    } 
  }
}


// ==================================================================
void KDTree::SplitCell() {
  const glm::vec3& min = bbox.getMin();
  const glm::vec3& max = bbox.getMax();
  float dx = max.x-min.x;
  float dy = max.y-min.y;
  float dz = max.z-min.z;
  // split this cell in the middle of the longest axis
  glm::vec3 min1,min2,max1,max2;
  if (dx >= dy && dx >= dz) {
    split_axis = 0;
    split_value = min.x+dx/2.0;
    min1 = glm::vec3(min.x    ,min.y,min.z);
    max1 = glm::vec3(split_value,max.y,max.z);
    min2 = glm::vec3(split_value,min.y,min.z);
    max2 = glm::vec3(max.x       ,max.y,max.z);
  } else if (dy >= dx && dy >= dz) {
    split_axis = 1;
    split_value = min.y+dy/2.0;
    min1 = glm::vec3(min.x,min.y    ,min.z);
    max1 = glm::vec3(max.x,split_value,max.z);
    min2 = glm::vec3(min.x,split_value,min.z);
    max2 = glm::vec3(max.x,max.y    ,max.z);
  } else {
    assert (dz >= dx && dz >= dy);
    split_axis = 2;
    split_value = min.z+dz/2.0;
    min1 = glm::vec3(min.x,min.y,min.z    );
    max1 = glm::vec3(max.x,max.y,split_value);
    min2 = glm::vec3(min.x,min.y,split_value);
    max2 = glm::vec3(max.x,max.y,max.z    );
  }
  // create two new children
  child1 = new KDTree(BoundingBox(min1,max1),depth+1);
  child2 = new KDTree(BoundingBox(min2,max2),depth+1);
  int num_photons = photons.size();
  std::vector<Photon> tmp = photons;
  photons.clear();
  // add all the photons to one of those children
  for (int i = 0; i < num_photons; i++) {
    const Photon &p = tmp[i];
    this->AddPhoton(p);
  }
}

// ==================================================================
