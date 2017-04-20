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
	int getChild(int num) { return child[num]; }
	int numChildren() { return child.size(); }
	bool isSelected() { return selected; }
	
	//Modifiers
	void setParent(int j) { parent = j; }
	void addChild(int j) { child.push_back(j); }
	void setPosition(glm::vec3 &position) { pos = position; }
	void setID(int &num) { id = num; }

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
	Joint& getJoint(int id){ return joints[id]; }
	
	void addJoint(Joint &j) {
		joints.push_back(j);
	}
	int getRoot() const {
		return root;
	}
	int size() const {
		return joints.size();
	}
	Joint& getClosest(int i);

	//helpers
	void clearJoints();
	bool Parallel_load(std::string filename, ArgParser *args);

private:
	std::vector<Joint> joints;
	int root;
};
#endif 