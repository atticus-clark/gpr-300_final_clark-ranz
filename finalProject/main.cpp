#pragma once

#include "notmain.h"

int main()
{
	GLFWwindow* window = initWindow("Final Project", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	/* code taken from the LearnOpenGL tutorial on Stencil Testing
	* https://learnopengl.com/Advanced-OpenGL/Stencil-testing */
	glEnable(GL_STENCIL_TEST); // enable stencil testing
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // tells OpenGL how to determine if it should discard a fragment

	// camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	glm::vec3 lightPos(0.0, -1.0, 0.0);

	// objects
	ew::Mesh waterMesh(ew::createPlane(10.0f, 10.0f, 20));
	ew::Transform waterTransform;
	int waterHeight = -2;
	waterTransform.position = glm::vec3(0, waterHeight, 0);

	glm::vec4 reflectionClipPlane = glm::vec4(0, 1, 0, -waterHeight);
	glm::vec4 refractionClipPlane = glm::vec4(0, -1, 0, waterHeight);

	// framebuffers //
	// refraction framebuffer
	unsigned int refractionFramebuffer;
	glGenFramebuffers(1, &refractionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLuint refractionTex = createTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	GLuint refractionDepthTex = createDepthTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	unbindFramebuffer();

	// reflection framebuffer
	unsigned int reflectionFramebuffer;
	glGenFramebuffers(1, &reflectionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLuint reflectionTex = createTexture(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	GLuint reflectionDepthBuf = createDepthBuffer(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	unbindFramebuffer();

	// shaders
	ew::Shader waterShader = ew::Shader("assets/shaders/water.vert", "assets/shaders/water.frag");

	// textures
	glBindTextureUnit(0, reflectionTex);
	glBindTextureUnit(1, refractionTex);
	GLuint dudvMap = ew::loadTexture("assets/textures/waterDUDV.png");
	glBindTextureUnit(2, dudvMap);

	//glBindTextureUnit(3, cubeTexture);
	//glBindTextureUnit(4, cylinderTexture);
	//glBindTextureUnit(5, sphereTexture);

	// shader calls
	waterShader.use();
	waterShader.setInt("reflectionTex", 0);
	waterShader.setInt("refractionTex", 1);
	waterShader.setInt("dudvMap", 2);

	OutlinedObjs outlined;
	pOutlined = &outlined;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);
		float distance = 2 * (camera.position.y - waterHeight);
		glm::mat4 viewProj = camera.projectionMatrix() * camera.viewMatrix();

		//moveFactor += WAVE_SPEED * deltaTime;
		//moveFactor %= 1;

		// RENDER //
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		// using the stencil buffer, gotta remember to clear it every frame!
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glStencilMask(0x00); // disable stencil buffer writing for non-outlined objects
		glEnable(GL_CLIP_DISTANCE0);

		// shader calls
		waterShader.use();
		waterShader.setMat4("_Model", waterTransform.modelMatrix());
		waterShader.setFloat("moveFactor", moveFactor);
		
		// reflection
		bindFramebuffer(reflectionFramebuffer, REFLECTION_HEIGHT, REFLECTION_WIDTH);
		camera.position.y -= distance;
		cameraController.pitch = -cameraController.pitch;
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		waterShader.setVec4("_Plane", reflectionClipPlane);
		waterMesh.draw();

		camera.position.y += distance;
		cameraController.pitch = -cameraController.pitch;

		// refraction
		bindFramebuffer(refractionFramebuffer, REFRACTION_HEIGHT, REFRACTION_WIDTH);
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		waterShader.setVec4("_Plane", refractionClipPlane);
		waterMesh.draw();

		glDisable(GL_CLIP_DISTANCE0);
		unbindFramebuffer();
		waterMesh.draw();

		outlined.Render(viewProj); // render outlined objects

		drawUI(deltaTime);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
