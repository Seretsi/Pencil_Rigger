#ifndef _RIGGER_H
#define _RIGGER_H

#include "raytracer.h"
#include "joint.h"
#include "hit.h"
#include "vbo_structs.h"
#include "argparser.h"
#include <omp.h>
<<<<<<< HEAD
#include "glCanvas.h"
=======
>>>>>>> refs/remotes/origin/dev_br

class ArgParser;
class RayTracer;
class Hit;
class JointTree;

class Rigger {
public:	
	//constructor
	Rigger(RayTracer *raytracer, JointTree *jointtree) { 
		rt = raytracer; 
		jt = jointtree;
	}

	// set access to the other modules for hybrid rendering options
	void setRaytracer(RayTracer *raytracer) { rt = raytracer; }
	void setJointTree(JointTree *jointtree) { jt = jointtree; }

	void initializeVBOs();
	void resetVBOs();
	void setupVBOs();
	void drawVBOs();
	void cleanupVBOs();

private:
	RayTracer* rt;
	JointTree* jt;

public:

	bool render_to_a;

	std::vector<VBOPosNormalColor> joints_pixel;
	//std::vector<VBOPosNormalColor> pixels_b;
	std::vector<VBOIndexedTri> joints_pixel_indices;
	//std::vector<VBOIndexedTri> pixels_indices_b;


private:
<<<<<<< HEAD
	void drawVBOs_joints();

	GLuint joints_pixels_VBO;
	//GLuint pixels_b_VBO;
	GLuint joints_pixels_indices_VBO;
=======

	GLuint joints_pixels_a_VBO;
	//GLuint pixels_b_VBO;
	GLuint joints_pixels_indices_a_VBO;
>>>>>>> refs/remotes/origin/dev_br
	//GLuint pixels_indices_b_VBO;
};

#endif // !_RIGGER_H
