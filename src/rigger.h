#ifndef _RIGGER_H
#define _RIGGER_H

#include "raytracer.h"
#include "joint.h"
#include "hit.h"
#include "vbo_structs.h"
#include "argparser.h"
#include <omp.h>
#include "glCanvas.h"


class ArgParser;
class RayTracer;
class Hit;
class JointTree;

class Rigger {
public:	
	//constructor
	Rigger () {}
	Rigger(RayTracer *raytracer, JointTree *jointtree, ArgParser *arg) { 
		rt = raytracer; 
		jt = jointtree;
		args = arg;
	}

	// set access to the other modules for hybrid rendering options
	void setRaytracer(RayTracer *raytracer) { rt = raytracer; }
	void setJointTree(JointTree *jointtree) { jt = jointtree; }


	JointTree* getJointTree() {return jt;}

	void initializeVBOs();
	void resetVBOs();
	void setupJoints();
	void setupBones();
	void drawVBOs_joints();
	void drawVBOs_bones();
	void drawVBOs();
	void cleanupVBOs();

private:
	RayTracer* rt;
	JointTree* jt;
	ArgParser* args;

	bool render_to_a;

	std::vector<VBOPosNormalColor> joints_pixel;
	std::vector<VBOPosNormalColor> bones_pixels;
	std::vector<VBOIndexedTri> joints_pixel_indices;
	std::vector<VBOIndexedTri> bones_pixels_indices;

	GLuint joints_pixels_VBO;
	GLuint bones_pixels_VBO;
	GLuint joints_pixels_indices_VBO;
	GLuint bones_pixels_indices_VBO;
};

#endif // !_RIGGER_H
