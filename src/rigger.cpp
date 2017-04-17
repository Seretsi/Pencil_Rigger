#include "rigger.h"
#include "raytracer.h"
#include "joint.h"
#include "hit.h"
#include "vbo_structs.h"
#include "argparser.h"
#include <omp.h>
#include "glCanvas.h"
#include "utils.h"
#include "argparser.h"



void Rigger::initializeVBOs() {
	glGenBuffers(1, &joints_pixels_VBO);
	glGenBuffers(1, &joints_pixels_indices_VBO);
	//render_to_a = true;
}
void Rigger::resetVBOs() {

	joints_pixel.clear();
	//pixels_b.clear();

	joints_pixel_indices.clear();
	//pixels_indices_b.clear();

	//render_to_a = true;
}

void Rigger::setupJoints() {
	float offset = 0.1;
	glm::vec4 selected_color = glm::vec4(1.0, 1.0, 0.0, 1.0);
	glm::vec4 unselected_color = glm::vec4(1.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < 100; i++) {
		glm::vec3 pos = glm::vec3(args->rand(), args->rand(), args->rand());
		Joint* j = new Joint(jt->size(), pos);
		jt->addJoint(*j);

	}

	//run through "tree" and get positions to make cubes to represent joints3
#pragma omp parallel for
	for (int j = 0; j < jt->size(); ++j) {
		//run through each joint and create cube
		//BONUS: make cube camera facing

		Joint joint_node = jt->getJoint(j);
		glm::vec3 p = joint_node.getPos();
		glm::vec3 a = p + glm::vec3(-offset, offset, offset);
		glm::vec3 b = p + glm::vec3(offset, offset, offset);
		glm::vec3 c = p + glm::vec3(offset, offset, -offset);
		glm::vec3 d = p + glm::vec3(-offset, offset, -offset);
		glm::vec3 e = p + glm::vec3(-offset, -offset, offset);
		glm::vec3 f = p + glm::vec3(offset, -offset, offset);
		glm::vec3 g = p + glm::vec3(offset, -offset, -offset);
		glm::vec3 h = p + glm::vec3(-offset, -offset, -offset);

		//normals
		glm::vec3 normal_FF = computeNormal(a, e, b);
		glm::vec3 normal_BF = computeNormal(c, g, d);
		glm::vec3 normal_LF = computeNormal(d, h, a);
		glm::vec3 normal_RF = computeNormal(b, f, c);
		glm::vec3 normal_TF = computeNormal(d, a, c);
		glm::vec3 normal_UF = computeNormal(e, f, h);

		//color based on selection
		glm::vec4 color;
		joint_node.isSelected() ? color = selected_color : color = unselected_color;

		//FF - Front Face
		int start;
		start = joints_pixel.size();
		joints_pixel.push_back(VBOPosNormalColor(a, normal_FF, color));
		joints_pixel.push_back(VBOPosNormalColor(e, normal_FF, color));
		joints_pixel.push_back(VBOPosNormalColor(b, normal_FF, color));
		joints_pixel.push_back(VBOPosNormalColor(f, normal_FF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));
		
		//BF - Back Face
		start += 4;
		joints_pixel.push_back(VBOPosNormalColor(c, normal_BF, color));
		joints_pixel.push_back(VBOPosNormalColor(g, normal_BF, color));
		joints_pixel.push_back(VBOPosNormalColor(d, normal_BF, color));
		joints_pixel.push_back(VBOPosNormalColor(h, normal_BF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));

		//LF - Left Face
		start += 4;
		joints_pixel.push_back(VBOPosNormalColor(d, normal_LF, color));
		joints_pixel.push_back(VBOPosNormalColor(h, normal_LF, color));
		joints_pixel.push_back(VBOPosNormalColor(a, normal_LF, color));
		joints_pixel.push_back(VBOPosNormalColor(e, normal_LF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));

		//RF - Right Face
		start += 4;
		joints_pixel.push_back(VBOPosNormalColor(b, normal_RF, color));
		joints_pixel.push_back(VBOPosNormalColor(f, normal_RF, color));
		joints_pixel.push_back(VBOPosNormalColor(c, normal_RF, color));
		joints_pixel.push_back(VBOPosNormalColor(g, normal_RF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));

		//TF - Top Face
		start += 4;
		joints_pixel.push_back(VBOPosNormalColor(d, normal_TF, color));
		joints_pixel.push_back(VBOPosNormalColor(a, normal_TF, color));
		joints_pixel.push_back(VBOPosNormalColor(c, normal_TF, color));
		joints_pixel.push_back(VBOPosNormalColor(b, normal_TF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));

		//UF - Under Face
		start += 4;
		joints_pixel.push_back(VBOPosNormalColor(e, normal_UF, color));
		joints_pixel.push_back(VBOPosNormalColor(h, normal_UF, color));
		joints_pixel.push_back(VBOPosNormalColor(f, normal_UF, color));
		joints_pixel.push_back(VBOPosNormalColor(g, normal_UF, color));
		joints_pixel_indices.push_back(VBOIndexedTri(start, start + 1, start + 2));
		joints_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 1, start + 3));
	}

	glBindBuffer(GL_ARRAY_BUFFER, joints_pixels_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*joints_pixel.size(), &joints_pixel[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joints_pixels_indices_VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(VBOIndexedTri) * joints_pixel_indices.size(),
		&joints_pixel_indices[0], GL_STATIC_DRAW);
}
void Rigger::drawVBOs() {
	
	// turn off depth buffer
	glDisable(GL_DEPTH_TEST);

	void drawVBOs_joints();

	glEnable(GL_DEPTH_TEST);
}

void Rigger::drawVBOs_joints() {
	if (joints_pixel.size() == 0) return;
	glBindBuffer(GL_ARRAY_BUFFER, joints_pixels_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joints_pixels_indices_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2 + sizeof(glm::vec4)));
	glDrawElements(GL_TRIANGLES,
		joints_pixel_indices.size() * 3,
		GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}


void Rigger::cleanupVBOs() {
	glDeleteBuffers(1, &joints_pixels_VBO);
	//glDeleteBuffers(1, &pixels_b_VBO);
}