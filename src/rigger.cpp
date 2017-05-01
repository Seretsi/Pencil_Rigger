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
	glGenBuffers(1, &bones_pixels_VBO);
	glGenBuffers(1, &bones_pixels_indices_VBO);
	glGenBuffers(1, &sketch_pixels_VBO);
	glGenBuffers(1, &sketch_pixels_indices_VBO);
}

void Rigger::resetVBOs() {

	joints_pixel.clear();
	bones_pixels.clear();

	joints_pixel_indices.clear();
	bones_pixels_indices.clear();
}

void Rigger::setupJoints() {
	float offset = 0.05f; //controls the size of rendered joint nodes
	glm::vec4 selected_color = glm::vec4(1.0, 1.0, 0.0, 1.0);
	glm::vec4 unselected_color = glm::vec4(1.0, 0.0, 0.0, 1.0);
	std::cout << "setting up\n";
//#pragma omp parallel for
	//run through "tree" and get positions to make cubes to represent joints3
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
		//std::cout << "selected?" << joint_node.isSelected() << std::endl;

		if (joint_node.isSelected()) {
			std::cout << "not selected\n";
			color = unselected_color;
		} 
		else {
			std::cout << "selected\n";
			color = selected_color;
		}
		//joint_node.isSelected() ? color = unselected_color : color = selected_color;

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

	if (joints_pixel.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, joints_pixels_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*joints_pixel.size(), &joints_pixel[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joints_pixels_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(VBOIndexedTri) * joints_pixel_indices.size(),
			&joints_pixel_indices[0], GL_STATIC_DRAW);
	}
}

void Rigger::setupBones() {
	float line_thickness = 0.01f; //controls the size of rendered joint nodes
	glm::vec4 selected_color = glm::vec4(1.0, 1.0, 0.0, 1.0);
	glm::vec4 unselected_color = glm::vec4(1.0, 0.0, 0.0, 1.0);

	//run through "tree" and get positions to make cubes to represent joints3
//#pragma omp parallel for
	for (int j = 0; j < jt->size(); ++j) {
		Joint joint_node = jt->getJoint(j);
		if (joint_node.getParent() < 0) { continue; }

		int parent_id = joint_node.getParent();
		Joint parent_joint = jt->getJoint(parent_id);

		//color based on selection
		glm::vec4 color;
		joint_node.isSelected() ? color = unselected_color : color = selected_color;

		addEdgeGeometry(bones_pixels, bones_pixels_indices, parent_joint.getPos(), joint_node.getPos(), color, color, line_thickness, line_thickness);
	}

	if (bones_pixels.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, bones_pixels_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*bones_pixels.size(), &bones_pixels[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bones_pixels_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(VBOIndexedTri) * bones_pixels_indices.size(),
			&bones_pixels_indices[0], GL_STATIC_DRAW);
	}
}

void Rigger::setupsketch() {
	//std::cout << "setting up sketch\n" << sketch_strokes.size() << std::endl;
	//normals
	glm::vec3 normal;
	if (sketch_strokes_coords.size() > 0) {
		normal = computeNormal(sketch_strokes[0], sketch_strokes[2], sketch_strokes[1]);
	}

	//color based on selection
	glm::vec4 color = glm::vec4(0.0, 0.0, 1.0, 1.0);
	sketch_pixel.empty();
	sketch_pixel_indices.empty();

//#pragma omp parallel for
	for (int s = 0; s < sketch_strokes_coords.size(); s++) {
		//FF - Front Face
		int start = s*4;
		start = sketch_pixel.size();
		sketch_pixel.push_back(VBOPosNormalColor(sketch_strokes[start], normal, color));
		sketch_pixel.push_back(VBOPosNormalColor(sketch_strokes[start+1], normal, color));
		sketch_pixel.push_back(VBOPosNormalColor(sketch_strokes[start+2], normal, color));
		sketch_pixel.push_back(VBOPosNormalColor(sketch_strokes[start+3], normal, color));
		sketch_pixel_indices.push_back(VBOIndexedTri(start, start + 2, start + 1));
		sketch_pixel_indices.push_back(VBOIndexedTri(start + 2, start + 3, start + 1));
	}

	if (sketch_pixel.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, sketch_pixels_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*sketch_pixel.size(), &sketch_pixel[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sketch_pixels_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(VBOIndexedTri) * sketch_pixel_indices.size(),
			&sketch_pixel_indices[0], GL_STATIC_DRAW);
	}
}

void Rigger::drawVBOs() {
	
	// turn off depth buffer
	//glDisable(GL_DEPTH_TEST);

	void drawVBOs_joints();
	void drawVBOs_bones();

	//glEnable(GL_DEPTH_TEST);
}

void Rigger::drawVBOs_joints() {
	if (joints_pixel.size() == 0) return;
	HandleGLError("enter draw joints");
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
	HandleGLError("exit draw joints");
}

void Rigger::drawVBOs_bones() {
	if (joints_pixel.size() == 0) return;
	HandleGLError("enter draw joints");
	glBindBuffer(GL_ARRAY_BUFFER, bones_pixels_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bones_pixels_indices_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2 + sizeof(glm::vec4)));
	glDrawElements(GL_TRIANGLES,
		bones_pixels_indices.size() * 3,
		GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	HandleGLError("exit draw joints");
}

void Rigger::drawVBOs_sketch(){
	if (sketch_pixel.size() == 0) return;
	HandleGLError("enter draw joints");
	glBindBuffer(GL_ARRAY_BUFFER, sketch_pixels_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sketch_pixels_indices_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2 + sizeof(glm::vec4)));
	glDrawElements(GL_TRIANGLES,
		sketch_pixel_indices.size() * 3,
		GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	HandleGLError("exit draw joints");
}

void Rigger::cleanupVBOs() {
	glDeleteBuffers(1, &joints_pixels_VBO);
	glDeleteBuffers(1, &bones_pixels_VBO);
	glDeleteBuffers(1, &sketch_pixels_VBO);
}

//add one "pixel" of line stroke using square vertex coordinates
void Rigger::sketch(glm::vec3 center, glm::vec3 upRight, glm::vec3 upLeft, glm::vec3 lowRight, glm::vec3 lowLeft){
	//printf("%f | %f | %f\n", upRight.x, upRight.y, upRight.z);
	sketch_strokes_coords.push_back(center);
	sketch_strokes.push_back(upLeft);
	sketch_strokes.push_back(upRight);
	sketch_strokes.push_back(lowLeft);
	sketch_strokes.push_back(lowRight);

}	







