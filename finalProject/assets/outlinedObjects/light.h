#pragma once

#include <ew/transform.h>

class Light {
public:
	Light() {}
	~Light() {}

	glm::vec3 pos = glm::vec3(0.0f, 30.0f, 20.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float nearPlane = 0.0f, farPlane = 50.0f;

	/* code ADAPTED from the LearnOpenGL tutorial on Shadow Mapping
	* https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping */
	glm::mat4 projMatrix() { return glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, nearPlane, farPlane); }
	glm::mat4 viewMatrix() { return glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::mat4 viewProjMatrix() { return projMatrix() * viewMatrix(); }
};
