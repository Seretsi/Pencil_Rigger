#ifndef _JOINT_H
#define _JOINT_H

#include <glm/glm.hpp>
#include <map>
#include <vector>

class Joint {
public:
	//Constructor
	Joint(glm::vec3 &position) : pos(position){
		if (count == NULL) {
			count = 1;
		}
		else {
			count++;
		}
		setID(count);
	}

	//Destructor
	~Joint() {}
	
	//Accessors
	glm::vec3 getPos() { return pos; }
	int getID() { return id; }
	Joint* getChild(int num) { return child[num]; }
	int numChildren() { return child.size(); }
	
	//Modifiers
	void setParent(Joint* j) { parent = j; }
	void addChild(Joint* j) { child.push_back(j); }
	void setPosition(glm::vec3 &position) { pos = position; }
	void setID(int &num) { id = num; }

private:

	//representation
  	static int count;
	int id;
	glm::vec3 pos;
	Joint* parent;
	std::vector<Joint*> child;
	bool selected;

};

class JointTree {
public:
	Joint* getJoint(int id) { return jointsmap[id]; }

	void addJoint(Joint &j) {
		assert(jointsmap.find(j.getID()) == jointsmap.end());

	}
	//helpers
	void clearJoints();

private:
	std::map<int, Joint*> jointsmap;
};
#endif 