#ifndef _RIGGER_H
#define _RIGGER_H

#include "raytracer.h"
#include "joint.h"
#include "hit.h"
#include "vbo_structs.h"
#include "argparser.h"
#include <omp.h>

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
};

#endif // !_RIGGER_H
