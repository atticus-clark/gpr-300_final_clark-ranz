#pragma once

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/transform.h>
#include <ew/mesh.h>
#include <ew/texture.h>
#include <ew/procGen.h>

class Object {
public:
	Object() {}
	~Object() {}

	ew::Mesh mesh = ew::Mesh();
	ew::Transform transform = ew::Transform();
	glm::vec3 rotation = glm::vec3();
	glm::vec4 color = glm::vec4(1.0f);

	void UpdateRotation() { transform.rotation = glm::quat(glm::radians(rotation)); }
};

class OutlinedObjs {
public:
	OutlinedObjs();
	~OutlinedObjs();

	static const unsigned int NUM_OBJS = 3; // i hate that this has to be static

	Object objs[NUM_OBJS];

	glm::vec4 outlineColor = glm::vec4(1.0f);
	float outlineScale = 0.05f;
	bool xray = false;

	void Render(const glm::mat4& viewProj);

private:
	ew::Shader shader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
	ew::Shader outlineShader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
};
