#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "joint.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <queue>

void JointTree::clearJoints() {
	//use omp to enter jointtree and delete tree from root
}

int JointTree::getClosest(int i) {
	int closest = -1;
	float shortest = -1;
	glm::vec3 jt = joints[i].getPos();
	for (int j = 0; j < i; j++) {
		glm::vec3 cnd = joints[j].getPos();
		float distance = sqrt(pow((cnd.x-jt.x), 2) + pow((cnd.y-jt.y), 2) + pow((cnd.z-jt.z), 2));
		if (closest == -1 || (distance < shortest)) {
			closest = j;
			shortest = distance;
		}
	}
	for (int j = i+1; j < joints.size(); j++) {
		glm::vec3 cnd = joints[j].getPos();
		float distance = sqrt(pow((cnd.x-jt.x), 2) + pow((cnd.y-jt.y), 2) + pow((cnd.z-jt.z), 2));
		if (closest == -1 || (distance < shortest)) {
			closest = j;
			shortest = distance;
		}
	}
	return closest;
	//return NULL;
}

bool JointTree::Save(std::string fname) {
	std::ofstream of(fname);
	if (!of.good()) {
		std::cerr << "issue opening ouput file";
		return false;
	}
	of << joints.size() << std::endl;
	for (int i = 0; i < joints.size(); i++) {
		glm::vec3 tpos = joints[i].getPos();
		of << tpos.x << " " << tpos.y << " " << tpos.z << " " << i << std::endl;
	}
	of << "\n";
	std::queue<int> bfs;
	bfs.push(root);
	while (!bfs.empty()) {
		int current = bfs.front();
		bfs.pop();
		of << current << " ";
		Joint* working_joint = &joints[current];
		std::cout << "number of children: " << working_joint->numChildren() << std::endl;
		for (int i = 0; i < working_joint->numChildren(); i++) {
			bfs.push(working_joint->getChild(i));
			of << working_joint->getChild(i) << " ";
		}
		of << "\n";
	}
	return true;
}

bool JointTree::Parallel_load(std::string filename) {
	std::ifstream rigfile(filename);
	if (!rigfile.good()) {
		std::cerr << "ERROR: could not open rigging file" << std::endl;
		return false;
	}
	int num_joints;
	rigfile >> num_joints;
	joints = std::vector<Joint>(num_joints);
	for (int i = 0; i < num_joints; i++) {
		float x,y,z;
		int index;
		rigfile >> x >> y >> z >> index;
		Joint temp(index, glm::vec3(x,y,z));	
		joints[index] = temp;

	}
	std::string token;
	while (getline(rigfile, token)) {
		std::stringstream ss(token);
		int root;
		int child;
		ss >> root;
		while (ss >> child) {
			parent(child, root);
		}
	}
	
}
