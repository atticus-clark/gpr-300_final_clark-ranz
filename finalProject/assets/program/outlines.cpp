#include "outlines.h"

/* code ADAPTED from the LearnOpenGL tutorial on Stencil Testing
* https://learnopengl.com/Advanced-OpenGL/Stencil-testing */
void OutlinePass::Render(Object* objs, const int& NUM_OBJS, const glm::mat4& viewProj) {
	/* The stencil buffer was filled with some 1s in the previous step (regular pass); those
	 * parts are not drawn, thus only drawing the size differences between the objects in the
	 * 1st and 2nd pass, which looks like an outline. */

	// setup //
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // don't draw any fragments that were drawn before
										 // aka, only draw the scaled-up parts of the objects
	glStencilMask(0x00); // disable writing to stencil buffer

	// this is what actually determines if outlines are visible through walls or not
	// refer to xray.md to learn why this only works with the first glStencilOp() setup
	if(pXray != nullptr) { *pXray ? glDisable(GL_DEPTH_TEST) : false; }

	outlineShader.use();
	outlineShader.setMat4("_ViewProjection", viewProj);
	outlineShader.setVec4("_Color", outlineColor);

	glm::vec3 scaleVec = glm::vec3(outlineScale, outlineScale, outlineScale);
	glm::mat4 scaledModel;

	// draw objects //
	for(int i = 0; i < NUM_OBJS; i++) {
		scaledModel = glm::scale(objs[i].transform.modelMatrix(), scaleVec);
		outlineShader.setMat4("_Model", scaledModel);
		objs[i].mesh.draw();
	}

	// cleanup //
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	// if depth testing was disabled, make sure to reenable it
	if(pXray != nullptr) { *pXray ? glEnable(GL_DEPTH_TEST) : false; }
}
