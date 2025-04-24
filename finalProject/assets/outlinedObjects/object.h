#pragma once

#include <ew/external/glad.h>
#include <ew/model.h>
#include <ew/procGen.h>
#include <ew/transform.h>

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
};

class Object {
public:
	Object() {}
	~Object() { if(model != nullptr) { delete model; } }

	ew::Model* model = nullptr;
	ew::Mesh mesh = ew::createSphere(1.0f, 25);
	ew::Transform transform = ew::Transform();
	glm::vec3 rotation = glm::vec3(); // this is redundant but it makes it easier to edit rotation in the GUI
	GLuint texture = 0;

	Material material = Material();

	void UpdateRotation() { transform.rotation = glm::quat(glm::radians(rotation)); }
};
