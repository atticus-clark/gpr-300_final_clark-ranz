#pragma once

#include "object.h"
#include "light.h"

#include <ew/shader.h>
#include <ew/camera.h>

class ObjectRenderer {
public:
	ObjectRenderer() {}
	~ObjectRenderer() {}

	// shadows
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO = 0, depthMapTexture = 0;
	float maxBias = 0.05f, minBias = 0.005f;

	glm::vec4 outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float outlineScale = 0.05f;
	bool xray = false;

	void Render(Object* objs, const int& NUM_OBJS);
	void SetupDepthMap();

	void SetCameraPtr(ew::Camera* camera) { pCamera = camera; }
	void SetLightPtr(Light* light) { pLight = light; }
	void SetScreenSizePtrs(int* width, int* height) { pWidth = width; pHeight = height; }
	void SetDepthShaderPtr(ew::Shader* depthShader) { pDepthShader = depthShader; }
	void SetMainShaderPtr(ew::Shader* mainShader) { pMainShader = mainShader; }
	void SetOutlineShaderPtr(ew::Shader* outlineShader) { pOutlineShader = outlineShader; }

private:
	ew::Camera* pCamera = nullptr;
	Light* pLight = nullptr;

	int* pWidth = nullptr; // ptr to screen width
	int* pHeight = nullptr; // ptr to screen height

	// shaders
	ew::Shader* pDepthShader = nullptr;
	ew::Shader* pMainShader = nullptr;
	ew::Shader* pOutlineShader = nullptr;

	// render passes
	void ShadowPass(Object* objs, const int& NUM_OBJS);
	void RegularPass(Object* objs, const int& NUM_OBJS);
	void OutlinePass(Object* objs, const int& NUM_OBJS);
};
