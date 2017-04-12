#ifndef _JOINT_H
#define _JOINT_H

#include <glm/glm.hpp>

class Joint {
public:
	Joint(glm::vec3 &position):pos(position){}
	glm::vec3 getPos() { return pos; }
	int getID() { return id; }
	Joint* getChild(int num) { return child[num]; }
	int numChildren() { return child.size(); }
	
	void setParent(Joint* j) { parent = j; }
	void addChild(Joint* j) { child.push_back(j); }
	void setPosition(glm::vec3 &position) { pos = position; }
	void setID(int &num) { id = num; }
private:
	//representation
	int id;
	glm::vec3 pos;
	Joint* parent;
	std::vector<Joint*> child;
	bool selected;

};

#endif 