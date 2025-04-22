#pragma once

#include "regular.h"
#include "shadows.h"
#include "outlines.h"

class CompositeRenderer {
public:
	CompositeRenderer() {}
	~CompositeRenderer() {}

	bool xray = false;

	void Render(Object* objs, const unsigned int& NUM_OBJS, const glm::mat4& viewProj);
	void Setup(ew::Camera* cameraPtr, LightSource* lightPtr, int* WIDTH, int* HEIGHT);

private:
	ShadowPass shadows = ShadowPass(); 
	RegularPass regular = RegularPass();
	OutlinePass outlines = OutlinePass();
};
