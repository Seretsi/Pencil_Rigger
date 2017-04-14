#ifndef _JOINT_H
#define _JOINT_H

#include <glm/glm.hpp>
#include <map>
#include <vector>

class Joint {
public:
	//Constructor
	Joint(int id, glm::vec3 &position) : pos(position){
		setID(id);
	}

	//Destructor
	~Joint() {}
	
	//Accessors
	glm::vec3 getPos() { return pos; }
	int getID() { return id; }
	int getChild(int num) { return child[num]; }
	int numChildren() { return child.size(); }
	
	//Modifiers
	void setParent(int &j) { parent = j; }
	void addChild(int &j) { child.push_back(j); }
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
	Joint& getJoint(int id) { return joints[id]; }
	
	void addJoint(Joint &j) {
		joints.push_back(j);
	}
	int size() {
		return joints.size();
	}

	//helpers
	void clearJoints();

private:
	std::vector<Joint> joints;
};
#endif 