#pragma once

#include "object.h"
#include "light.h"
#include <ew/shader.h>

class ShadowPass {
public:
	ShadowPass() {}
	~ShadowPass() {}

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthMapFBO = 0, depthMap = 0;

	void Render(Object* objs, const int& NUM_OBJS);
	void SetupDepthMap();
	void SetLightPtr(LightSource* newLight) { pLight = newLight; }
	void SetScreenSizePtrs(int* newWidth, int* newHeight) {
		pScreenWidth = newWidth;
		pScreenHeight = newHeight;
	}

private:
	ew::Shader depthShader = ew::Shader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
	LightSource* pLight = nullptr;
	int* pScreenWidth = nullptr;
	int* pScreenHeight = nullptr;
};
