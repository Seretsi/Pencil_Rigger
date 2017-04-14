#include "rigger.h"
#include "raytracer.h"
#include "joint.h"
#include "hit.h"
#include "vbo_structs.h"
#include "argparser.h"
#include <omp.h>
#include "glCanvas.h"


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
void Rigger::setupVBOs() {

	glBindBuffer(GL_ARRAY_BUFFER, joints_pixels_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*joints_pixel.size(), &joints_pixel[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, pixels_b_VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(VBOPosNormalColor)*pixels_b.size(), &pixels_b[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, joints_pixels_indices_VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(VBOIndexedTri) * joints_pixel_indices.size(),
		&joints_pixel_indices[0], GL_STATIC_DRAW);
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pixels_indices_b_VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(VBOIndexedTri) * pixels_indices_b.size(),
		&pixels_indices_b[0], GL_STATIC_DRAW);*/

}
void Rigger::drawVBOs() {
	// turn off lighting
	glUniform1i(GLCanvas::colormodeID, 0);
	// turn off depth buffer
	glDisable(GL_DEPTH_TEST);

	void drawVBOs_joints();

	glEnable(GL_DEPTH_TEST);
}

void Rigger::drawVBOs_joints() {
	if (joints_pixel.size() == 0) return;
	glBindBuffer(GL_ARRAY_BUFFER, joints_pixels_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pixels_indices_a_VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3) * 2 + sizeof(glm::vec4)));
	glDrawElements(GL_TRIANGLES,
		pixels_indices_a.size() * 3,
		GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}


void Rigger::cleanupVBOs() {
	glDeleteBuffers(1, &pixels_a_VBO);
	glDeleteBuffers(1, &pixels_b_VBO);
}