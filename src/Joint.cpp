#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "joint.h"
#include <fstream>

void JointTree::clearJoints() {
	//use omp to enter jointtree and delete tree from root
}

bool JointTree::Parallel_load(std::string filename, ArgParser* args) {
	std::ifstream rigfile(args->path+'/'+filename);
	if (!rigfile.good()) {
		std::cerr << "ERROR: could not open rigging file" << std::endl;
		return false;
	}
	std::string token;
	getline(rigfile, token);
	int num_joints = atoi(token.c_str());
	this->joints = std::vector<Joint>(num_joints);
	std::cout << "# joints: " << num_joints << std::endl;
	int num_threads = 16;
	int num_lines_per_thread = std::max(num_joints/num_threads, 1);
	int last_thread_extra = num_joints%num_threads;
#pragma omp parallel for 
	{
		int startline = 1 + omp_get_thread_num()*num_lines_per_thread;
		std::ifstream threadstream(args->path+'/'+filename);
		if (!rigfile.good()) {
			exit(0);
		}
		std::string threadtoken;
		int x = num_lines_per_thread;
		if (omp_get_thread_num() == num_threads -1) {
			x += last_thread_extra;
		}
		int i = 0;
		while (i < startline) {
			std::getline(threadstream, threadtoken);
			i++;
		}
		for (i = 0; i < x; i++) {
			std::getline(threadstream, threadtoken);
			std::stringstream ss(threadtoken);
			float x, y, z;
			int ind;
			ss >> x >> y >> z >> ind;
			glm::vec3 temp = glm::vec3(x,y,z);
			this->joints[ind] = Joint(ind, temp);
		}
		threadstream.close();
	}
	for (int i = 0; i < joints.size(); i++) {
		std::cout << joints[i].getID() << std::endl;
	}
	for (int i = 0; i < num_joints; i++) {
		getline(rigfile, token);
	}
	getline(rigfile, token); // should just be whitespace
	while (getline(rigfile, token)) {
		std::stringstream ss(token);
		std::string t;
		while (ss >> t) {

		}
	}

}