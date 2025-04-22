#pragma once

#include "object.h"
#include <ew/shader.h>

class OutlinePass {
public:
	OutlinePass() {}
	~OutlinePass() {}

	glm::vec4 outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float outlineScale = 0.05f;

	void Render(Object* objs, const int& NUM_OBJS, const glm::mat4& viewProj);
	void SetXrayPtr(bool* newXray) { pXray = newXray; }

private:
	ew::Shader outlineShader = ew::Shader("assets/shaders/outline.vert", "assets/shaders/outline.frag");
	bool* pXray = nullptr;
};
