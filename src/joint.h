#ifndef _JOINT_H
#define _JOINT_H

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <sstream>
#include "argparser.h"
#include <omp.h>

class Joint {
public:
	//Constructor
	Joint() {}
	Joint(int _id, glm::vec3 position) :  id(_id), pos(position){
		parent = -1;
		child = std::vector<int>();
	}

	//Destructor
	~Joint() {}
	
	//Accessors
	glm::vec3 getPos() { return pos; }
	int getID() { return id; }
	int getParent() { return parent; }
	int getChild(int num) { return child[num]; }
	int numChildren() { return child.size(); }
	bool isSelected() { return selected; }
	
	//Modifiers
	void setParent(int j) { parent = j; }
	void addChild(int j) { child.push_back(j); }
	void setPosition(glm::vec3 &position) { pos = position; }
	void setID(int num) { id = num; }
	void select() {selected = true;}
	void deselect() {selected = false;}

private:

	//representation
	int id;
	glm::vec3 pos;
	int parent;
	std::vector<int> child;
	bool selected;

};

class JointTree {
public:
	JointTree() {
		joints = std::vector<Joint>();
		int root = -1;
	}
	JointTree(int _size, int _root) {
		assert (_size > _root);
		assert (_root >= 0);
		assert (_size > 0);
		joints = std::vector<Joint>(_size);
		root = _root;
	}
	Joint getJoint(int id){ return joints[id]; }
	
	int addJoint(Joint &j) {
		joints.push_back(j);
		j.setID(joints.size()-1);
		if (joints.size() == 1) {
			root = 0;
		}
		return joints.size()-1;
	}
	int getRoot() const {
		return root;
	}
	int size() const {
		return joints.size();
	}
	int getClosest(int i);

	//parent joint i to joint j
	void parent(int child, int parent) {
		Joint *iJoint = &joints[child];
		Joint *jJoint = &joints[parent];

		iJoint->setParent(parent);
		jJoint->addChild(child);
	}
	int Select(glm::vec3 pos);
	void DeselectAll();

	//helpers
	void clearJoints();
	bool Parallel_load(std::string filename);
	bool Save(std::string fname);

private:
	std::vector<Joint> joints;
	int root;
};
#endif 