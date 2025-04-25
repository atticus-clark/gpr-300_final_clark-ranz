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

	// camera //
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	// objects //
	ew::Mesh waterMesh(ew::createPlane(10.0f, 10.0f, 20));
	ew::Transform waterTransform;
	int waterHeight = -2;
	waterTransform.position = glm::vec3(0, waterHeight, 0);

	glm::vec4 reflectionClipPlane = glm::vec4(0, 1, 0, -waterHeight);
	glm::vec4 refractionClipPlane = glm::vec4(0, -1, 0, waterHeight);

	// skybox //
	// skybox textures
	std::vector<std::string> faces
	{
		"assets/textures/right.jpg",
		"assets/textures/left.jpg",
		"assets/textures/top.jpg",
		"assets/textures/bottom.jpg",
		"assets/textures/front.jpg",
		"assets/textures/back.jpg"
	};
	GLuint cubemapTexture = loadCubemap(faces);

	// skybox VAO & VBO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// framebuffers //
	// refraction framebuffer
	unsigned int refractionFramebuffer;
	glGenFramebuffers(1, &refractionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	/*GLuint*/ refractionTex = createTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);
	GLuint refractionDepthTex = createDepthTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionDepthTex, 0);
	unbindFramebuffer();

	// reflection framebuffer
	unsigned int reflectionFramebuffer;
	glGenFramebuffers(1, &reflectionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	/*GLuint*/ reflectionTex = createTexture(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex , 0);
	GLuint reflectionDepthBuf = createDepthBuffer(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuf);
	unbindFramebuffer();

	GLuint dudvMap = ew::loadTexture("assets/textures/waterDUDV.png");

	// shaders //
	ew::Shader waterShader = ew::Shader("assets/shaders/water.vert", "assets/shaders/water.frag");
	ew::Shader skyboxShader = ew::Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	ew::Shader depthShader = ew::Shader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
	ew::Shader mainShader = ew::Shader("assets/shaders/lit.vert", "assets/shaders/lit.frag");
	ew::Shader outlineShader = ew::Shader("assets/shaders/singleColor.vert", "assets/shaders/singleColor.frag");

	// setup outlined objects & renderer //
	SetupOutlinedObjs();

	objRend.SetupDepthMap();
	objRend.SetCameraPtr(&camera);
	objRend.SetLightPtr(&light);
	objRend.SetScreenSizePtrs(&screenWidth, &screenHeight);

	objRend.SetDepthShaderPtr(&depthShader);
	objRend.SetMainShaderPtr(&mainShader);
	objRend.SetOutlineShaderPtr(&outlineShader);

	// textures //
	glBindTextureUnit(0, reflectionTex);
	glBindTextureUnit(1, refractionTex);
	glBindTextureUnit(2, dudvMap);

	glBindTextureUnit(3, objRend.depthMapTexture);
	glBindTextureUnit(4, aObjs[0].texture);
	glBindTextureUnit(5, aObjs[1].texture);
	glBindTextureUnit(6, aObjs[2].texture);

	// shader calls //
	skyboxShader.use();
	skyboxShader.setInt("skybox", cubemapTexture);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);
		glm::mat4 viewProj = camera.projectionMatrix() * camera.viewMatrix();

		moveFactor += WAVE_SPEED * deltaTime;
		if (moveFactor >= 1) { moveFactor = 0; }

		// RENDER //
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		// using the stencil buffer, gotta remember to clear it every frame!
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glStencilMask(0x00); // disable stencil buffer writing for non-outlined objects

		glEnable(GL_CLIP_DISTANCE0);

		// shader calls
		waterShader.use();
		waterShader.setInt("dudvMap", dudvMap);
		waterShader.setMat4("_Model", waterTransform.modelMatrix());
		waterShader.setFloat("moveFactor", moveFactor);

		// reflection
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		waterShader.setVec4("_Plane", reflectionClipPlane);
		bindFramebuffer(reflectionFramebuffer, REFLECTION_HEIGHT, REFLECTION_WIDTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.position.y = -camera.position.y;
		cameraController.pitch = -cameraController.pitch;
		
		waterMesh.draw();

		for (int i = 0; i < NUM_OBJS; i++) { aObjs[i].UpdateRotation(); }
		objRend.Render(aObjs, NUM_OBJS); // render outlined objects

		camera.position.y = -camera.position.y;
		cameraController.pitch = -cameraController.pitch;

		// refraction
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		waterShader.setVec4("_Plane", refractionClipPlane);
		bindFramebuffer(refractionFramebuffer, REFRACTION_HEIGHT, REFRACTION_WIDTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		waterMesh.draw();

		for (int i = 0; i < NUM_OBJS; i++) { aObjs[i].UpdateRotation(); }
		objRend.Render(aObjs, NUM_OBJS); // render outlined objects

		// scene
		glDisable(GL_CLIP_DISTANCE0);
		unbindFramebuffer();
		waterShader.use();
		waterShader.setInt("reflectionTex", reflectionTex);
		waterShader.setInt("refractionTex", refractionTex);
		waterMesh.draw();

		for (int i = 0; i < NUM_OBJS; i++) { aObjs[i].UpdateRotation(); }
		objRend.Render(aObjs, NUM_OBJS); // render outlined objects

		// skybox
		glDepthFunc(GL_LEQUAL); // depth
		skyboxShader.use();
		skyboxShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // depth

		drawUI(deltaTime);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");

	delete[] aObjs;
}
