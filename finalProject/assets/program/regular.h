#pragma once

#include "object.h"
#include "light.h"
#include <ew/shader.h>
#include <ew/camera.h>

class RegularPass {
public:
	RegularPass() {}
	~RegularPass() {}

	float maxBias = 0.05f, minBias = 0.005f;

	void Render(Object* objs, const int& NUM_OBJS);
	void SetCameraPtr(ew::Camera* newCamera) { pCamera = newCamera; }
	void SetLightPtr(LightSource* newLight) { pLight = newLight; }
	void SetXrayPtr(bool* newXray) { pXray = newXray; }

private:
	ew::Shader mainShader = ew::Shader("assets/shaders/lit.vert", "assets/shaders/lit.frag");
	ew::Camera* pCamera = nullptr;
	LightSource* pLight = nullptr;
	bool* pXray = nullptr;
};
