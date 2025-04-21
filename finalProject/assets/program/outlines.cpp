#include "outlines.h"

OutlinedObjs::OutlinedObjs() {
	meshes[0] = ew::createCube(2.0f);
	meshes[1] = ew::createCylinder(1.0f, 2.0f, 25);
	meshes[2] = ew::createSphere(1.0f, 25);

	transforms[0].position = glm::vec3(-3.0f, 0.0f, 0.0f);
	transforms[1].position = glm::vec3(0.0f, 0.0f, 0.0f);
	transforms[2].position = glm::vec3(3.0f, 0.0f, 0.0f);

	colors[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[1] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	colors[2] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	//GLuint cubeTexture = ew::loadTexture("assets/brick_color.jpg");
	//GLuint cylinderTexture = ew::loadTexture("assets/brick_color.jpg");
	//GLuint sphereTexture = ew::loadTexture("assets/brick_color.jpg");
}

OutlinedObjs::~OutlinedObjs() {}

/* code ADAPTED from the LearnOpenGL tutorial on Stencil Testing
 * https://learnopengl.com/Advanced-OpenGL/Stencil-testing */
void OutlinedObjs::Render(const glm::mat4& viewProj) {
	// shader setup
	outlineShader.use();
	outlineShader.setMat4("_ViewProjection", viewProj);

	shader.use();
	shader.setMat4("_ViewProjection", viewProj);

	// ------------------------------------------------------------------------------------------- //
	// 1st render pass: Draw objects as normal while writing to the stencil buffer.

	// setup
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	// draw objects
	for(int i = 0; i < NUM_OUTLINED_OBJS; i++) {
		shader.setMat4("_Model", transforms[i].modelMatrix());
		shader.setVec4("_Color", colors[i]);
		meshes[i].draw();
	}

	// ------------------------------------------------------------------------------------------- //
	// 2nd render pass: Draw objects slightly scaled-up without writing to the stencil buffer.

	/* The stencil buffer was filled with some 1s in the previous step; those parts are not
	 * drawn, thus only drawing the size differences between the objects in the 1st and 2nd
	 * pass, which looks like an outline. */

	// setup 
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	outlineShader.use();
	outlineShader.setVec4("_Color", outlineColor);

	glm::vec3 scale = glm::vec3(outlineScale, outlineScale, outlineScale);
	glm::mat4 scaledModel;

	// draw objects
	for(int i = 0; i < NUM_OUTLINED_OBJS; i++) {
		scaledModel = glm::scale(transforms[i].modelMatrix(), scale);
		outlineShader.setMat4("_Model", scaledModel);
		meshes[i].draw();
	}

	// cleanup
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
}
