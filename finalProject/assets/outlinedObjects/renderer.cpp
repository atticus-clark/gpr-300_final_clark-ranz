#include "renderer.h"
#include <iostream>

void ObjectRenderer::Render(Object* objs, const int& NUM_OBJS) {
	ShadowPass(objs, NUM_OBJS);
	RegularPass(objs, NUM_OBJS);
	OutlinePass(objs, NUM_OBJS);
}

/* code ADAPTED from the LearnOpenGL tutorial on Shadow Mapping
 * https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping */
void ObjectRenderer::ShadowPass(Object* objs, const int& NUM_OBJS) {
	// render scene from light's point of view //
	pDepthShader->use();
	pDepthShader->setMat4("_LightSpaceMatrix", pLight->viewProjMatrix());

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// draw objects //
	for(int i = 0; i < NUM_OBJS; i++) {
		pDepthShader->setMat4("_Model", objs[i].transform.modelMatrix());

		// check if object has model or mesh
		if(objs[i].model != nullptr) { objs[i].model->draw(); }
		else { objs[i].mesh.draw(); }
	}

	// reset viewport //
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, *pWidth, *pHeight);
}

void ObjectRenderer::RegularPass(Object* objs, const int& NUM_OBJS) {
	// these slightly change how the stencil buffer is written to
	// refer to xray.md to learn about what exactly this does
	xray ? glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE) : glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF); // makes all fragments pass the stencil test
	glStencilMask(0xFF); // enable writing to stencil buffer

	// camera and light setup //
	pMainShader->use();
	pMainShader->setMat4("_ViewProjection", pCamera->projectionMatrix() * pCamera->viewMatrix());
	pMainShader->setVec3("_EyePos", pCamera->position);

	pMainShader->setMat4("_LightSpaceMatrix", pLight->viewProjMatrix());
	pMainShader->setVec3("_LightPos", pLight->pos);
	pMainShader->setVec4("_LightColor", pLight->color);

	pMainShader->setFloat("_MaxBias", maxBias);
	pMainShader->setFloat("_MinBias", minBias);

	pMainShader->setFloat("_CelLevels", 4.0f); // number of cel shading levels

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	pMainShader->setInt("_ShadowMap", 3);

	// draw objects //
	for(int i = 0; i < NUM_OBJS; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, objs[i].texture);

		pMainShader->setInt("_DiffuseTexture", i);

		pMainShader->setFloat("_Material.Ka", objs[i].material.Ka);
		pMainShader->setFloat("_Material.Kd", objs[i].material.Kd);
		pMainShader->setFloat("_Material.Ks", objs[i].material.Ks);
		pMainShader->setFloat("_Material.Shininess", objs[i].material.Shininess);

		pMainShader->setMat4("_Model", objs[i].transform.modelMatrix());

		// check if object has model or mesh
		if(objs[i].model != nullptr) { objs[i].model->draw(); }
		else { objs[i].mesh.draw(); }
	}
}

/* code ADAPTED from the LearnOpenGL tutorial on Stencil Testing
 * https://learnopengl.com/Advanced-OpenGL/Stencil-testing */
void ObjectRenderer::OutlinePass(Object* objs, const int& NUM_OBJS) {
	/* The stencil buffer was filled with some 1s in the previous step (regular pass); those
	 * parts are not drawn, thus only drawing the size differences between the objects in the
	 * 1st and 2nd pass, which looks like an outline. */

	 // setup //
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // don't draw any fragments that were drawn before
										 // aka, only draw the scaled-up parts of the objects
	glStencilMask(0x00); // disable writing to stencil buffer

	// this is what actually determines if outlines are visible through walls or not
	// refer to xray.md to learn why this only works with the first glStencilOp() setup
	xray ? glDisable(GL_DEPTH_TEST) : false;

	pOutlineShader->use();
	pOutlineShader->setMat4("_ViewProjection", pCamera->projectionMatrix() * pCamera->viewMatrix());
	pOutlineShader->setVec4("_Color", outlineColor);

	glm::vec3 scaleVec = glm::vec3(outlineScale + 1, outlineScale + 1, outlineScale + 1);
	glm::mat4 scaledModel;

	// draw objects //
	for(int i = 0; i < NUM_OBJS; i++) {
		scaledModel = glm::scale(objs[i].transform.modelMatrix(), scaleVec);
		pOutlineShader->setMat4("_Model", scaledModel);
		
		// check if object has model or mesh
		if(objs[i].model != nullptr) { objs[i].model->draw(); }
		else { objs[i].mesh.draw(); }
	}

	// cleanup //
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	// if depth testing was disabled, make sure to reenable it
	xray ? glEnable(GL_DEPTH_TEST) : false;
}

/* code taken from the LearnOpenGL tutorial on Shadow Mapping
* https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping */
void ObjectRenderer::SetupDepthMap() {
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMapTexture);

	// create depth texture
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

