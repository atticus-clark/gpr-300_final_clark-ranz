#pragma once

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/transform.h>
#include <ew/mesh.h>
#include <ew/texture.h>
#include <ew/procGen.h>

class OutlinedObjs {
public:
	OutlinedObjs();
	~OutlinedObjs();

	static const unsigned int NUM_OUTLINED_OBJS = 3; // i hate that this has to be static

	ew::Transform transforms[NUM_OUTLINED_OBJS];
	glm::vec3 rotations[NUM_OUTLINED_OBJS]; // this is redundant but makes rotating from the GUI cleaner
	glm::vec4 colors[NUM_OUTLINED_OBJS];

	glm::vec4 outlineColor = glm::vec4(1.0f);
	float outlineScale = 1.05f;

	bool xray = false;

	void Render(const glm::mat4& viewProj);

private:
	ew::Mesh meshes[NUM_OUTLINED_OBJS];
	ew::Shader shader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
	ew::Shader outlineShader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
};
