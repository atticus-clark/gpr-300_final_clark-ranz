#pragma once

#include "notmain.h"

int main() {
	GLFWwindow* window = initWindow("Final Project", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	/* code taken from the LearnOpenGL tutorial on Stencil Testing
	 * https://learnopengl.com/Advanced-OpenGL/Stencil-testing */
	glEnable(GL_STENCIL_TEST); // enable stencil testing
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // tells OpenGL how to determine if it should discard a fragment

	// camera //
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	// shaders //
	ew::Shader depthShader = ew::Shader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
	ew::Shader mainShader = ew::Shader("assets/shaders/lit.vert", "assets/shaders/lit.frag");
	ew::Shader outlineShader = ew::Shader("assets/shaders/outline.vert", "assets/shaders/outline.frag");

	// setup objects & renderer //
	SetupOutlinedObjs();

	objRend.SetupDepthMap();
	objRend.SetCameraPtr(&camera);
	objRend.SetLightPtr(&light);
	objRend.SetScreenSizePtrs(&screenWidth, &screenHeight);

	objRend.SetDepthShaderPtr(&depthShader);
	objRend.SetMainShaderPtr(&mainShader);
	objRend.SetOutlineShaderPtr(&outlineShader);

	// texture units //
	glBindTextureUnit(0, aObjs[0].texture);
	glBindTextureUnit(1, aObjs[1].texture);
	glBindTextureUnit(2, aObjs[2].texture);
	glBindTextureUnit(3, objRend.depthMapTexture);

	// render loop //
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);
		glm::mat4 viewProj = camera.projectionMatrix() * camera.viewMatrix();

		// RENDER //
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		// using the stencil buffer, gotta remember to clear it every frame!
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00); // disable stencil buffer writing for non-outlined objects

		objRend.Render(aObjs, NUM_OBJS); // render outlined objects

		drawUI(deltaTime);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");

	delete[] aObjs;
}
