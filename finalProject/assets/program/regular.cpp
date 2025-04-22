#include "regular.h"

void RegularPass::Render(Object* objs, const int& NUM_OBJS) {
	mainShader.use();

	// these slightly change how the stencil buffer is written to
	// refer to xray.md to learn about what exactly this does
	if(pXray != nullptr) { *pXray ? glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE) : glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); }

	glStencilFunc(GL_ALWAYS, 1, 0xFF); // makes all fragments pass the stencil test
	glStencilMask(0xFF); // enable writing to stencil buffer

	// camera and light setup //
	mainShader.setMat4("_ViewProjection", pCamera->projectionMatrix() * pCamera->viewMatrix());
	mainShader.setVec3("_EyePos", pCamera->position);

	mainShader.setMat4("_LightSpaceMatrix", pLight->viewProjMatrix());
	mainShader.setVec3("_LightPos", pLight->pos);
	mainShader.setVec4("_LightColor", pLight->color);

	mainShader.setFloat("_MaxBias", maxBias);
	mainShader.setFloat("_MinBias", minBias);

	// draw objects //
	for(int i = 0; i < NUM_OBJS; i++) {
		mainShader.setInt("_DiffuseTexture", 0);
		mainShader.setInt("_ShadowMap", 1);

		mainShader.setFloat("_Material.Ka", objs[i].material.Ka);
		mainShader.setFloat("_Material.Kd", objs[i].material.Kd);
		mainShader.setFloat("_Material.Ks", objs[i].material.Ks);
		mainShader.setFloat("_Material.Shininess", objs[i].material.Shininess);

		mainShader.setMat4("_Model", objs[i].transform.modelMatrix());
	}
}
