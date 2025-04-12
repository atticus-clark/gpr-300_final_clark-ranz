#include "notmain.h"

int main()
{
	GLFWwindow* window = initWindow("Final Project", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	// camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	glm::vec3 lightPos(0.0, -1.0, 0.0);

	// objects
	ew::Mesh planeMesh(ew::createPlane(10.0f, 10.0f, 20));
	ew::Transform planeTransform;
	planeTransform.position = glm::vec3(0, -2.0, 0);

	// shaders
	ew::Shader waterShader = ew::Shader("assets/water.vert", "assets/water.frag");

	// framebuffers
	// refraction framebuffer
	unsigned int refractionFramebuffer;
	glGenFramebuffers(1, &refractionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
	GLuint refractionTex = createTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	GLuint refractionDepthTex = createDepthTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	unbindFramebuffer();

	// reflection framebuffer
	unsigned int reflectionFramebuffer;
	glGenFramebuffers(1, &reflectionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
	GLuint reflectionTex = createTexture(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	GLuint reflectionDepthTex = createDepthBuffer(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	unbindFramebuffer();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);

		//RENDER
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shader calls
		waterShader.use();
		waterShader.setMat4("_Model", planeTransform.modelMatrix());
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		planeMesh.draw();

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
