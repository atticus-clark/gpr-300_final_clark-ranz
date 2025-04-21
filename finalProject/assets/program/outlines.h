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

	static const unsigned int NUM_OUTLINED_OBJS = 3; // this is dumb but whatever

	ew::Transform transforms[NUM_OUTLINED_OBJS];
	glm::vec4 colors[NUM_OUTLINED_OBJS];

	glm::vec4 outlineColor = glm::vec4(1.0f);
	float outlineScale = 1.05f;

	void Render(const glm::mat4& viewProj);

private:
	ew::Mesh meshes[NUM_OUTLINED_OBJS];
	ew::Shader shader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
	ew::Shader outlineShader = ew::Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
};
