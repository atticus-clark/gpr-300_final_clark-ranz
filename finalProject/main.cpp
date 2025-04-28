#pragma once

// creator credits:
// water & skybox - River
// cel shading and object outlines - Atticus

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
	camera.position = glm::vec3(0.0f, 0.0f, 8.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	// code adapted for C++ from ThinMatrix's water tutorial
	// https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh
	
	// objects //
	ew::Mesh waterMesh(ew::createPlane(20.0f, 20.0f, 20));
	ew::Transform waterTransform;
	int waterHeight = -2;
	waterTransform.position = glm::vec3(0, waterHeight, 0);

	glm::vec4 reflectionClipPlane = glm::vec4(0, 1, 0, -waterHeight);
	glm::vec4 refractionClipPlane = glm::vec4(0, -1, 0, waterHeight);

	// code taken from LearnOpenGL's skybox tutorial
	// https://learnopengl.com/Advanced-OpenGL/Cubemaps
	
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
	GLuint refractionTex = createTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTex, 0);
	GLuint refractionDepthTex = createDepthTexture(REFRACTION_HEIGHT, REFRACTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTex, 0);
	unbindFramebuffer();

	// reflection framebuffer
	unsigned int reflectionFramebuffer;
	glGenFramebuffers(1, &reflectionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	GLuint reflectionTex = createTexture(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);
	GLuint reflectionDepthBuf = createDepthBuffer(REFLECTION_HEIGHT, REFLECTION_WIDTH);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuf);
	unbindFramebuffer();

	GLuint dudvMap = ew::loadTexture("assets/textures/waterDUDV.png");
	GLuint waterNormal = ew::loadTexture("assets/textures/waterNormalMap.png");

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
	glBindTextureUnit(7, reflectionTex);
	glBindTextureUnit(8, refractionTex);
	glBindTextureUnit(9, dudvMap);
	glBindTextureUnit(10, waterNormal);

	glBindTextureUnit(3, objRend.depthMapTexture);
	glBindTextureUnit(4, aObjs[0].texture);
	glBindTextureUnit(5, aObjs[1].texture);
	glBindTextureUnit(6, aObjs[2].texture);

	// shader calls //
	waterShader.use();
	waterShader.setInt("reflectionTex", 7);
	waterShader.setInt("refractionTex", 8);
	waterShader.setInt("dudvMap", 9);
	waterShader.setInt("normalMap", 10);

	skyboxShader.use();
	skyboxShader.setInt("skybox", cubemapTexture);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);
		glm::mat4 viewProj = camera.projectionMatrix() * camera.viewMatrix();
		glm::mat4 lightSpaceMatrix = light.viewProjMatrix();

		float distance = 2 * camera.position.y;

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
		waterShader.setFloat("moveFactor", moveFactor);


		// reflection //
		bindFramebuffer(reflectionFramebuffer, REFLECTION_HEIGHT, REFLECTION_WIDTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.position.y = -camera.position.y;
		cameraController.pitch = -cameraController.pitch;
		
		// skybox
		glDepthFunc(GL_LEQUAL); // depth
		skyboxShader.use();
		skyboxShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		renderSkybox(skyboxVAO, cubemapTexture);
		glDepthFunc(GL_LESS); // depth

		// depth pass
		depthShader.use();
		depthShader.setMat4("_LightSpaceMatrix", lightSpaceMatrix);
		depthObjRender(depthShader);

		// regular pass
		mainShader.use();
		mainShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		mainShader.setVec4("_Plane", reflectionClipPlane);
		mainShader.setVec4("_LightColor", light.color);
		mainShader.setMat4("_LightSpaceMatrix", lightSpaceMatrix);
		objRender(mainShader);

		glBindTexture(GL_TEXTURE_2D, reflectionTex);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 7);

		camera.position.y = -camera.position.y;
		cameraController.pitch = -cameraController.pitch;

		// refraction //
		bindFramebuffer(refractionFramebuffer, REFRACTION_HEIGHT, REFRACTION_WIDTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// skybox
		glDepthFunc(GL_LEQUAL); // depth
		skyboxShader.use();
		skyboxShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		renderSkybox(skyboxVAO, cubemapTexture);
		glDepthFunc(GL_LESS); // depth

		// depth pass
		depthShader.use();
		depthShader.setMat4("_LightSpaceMatrix", lightSpaceMatrix);
		depthObjRender(depthShader);

		// regular pass
		mainShader.use();
		mainShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		mainShader.setVec4("_Plane", refractionClipPlane);
		mainShader.setVec4("_LightColor", light.color);
		objRender(mainShader);

		glBindTexture(GL_TEXTURE_2D, refractionTex);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 8);


		// scene //
		glDisable(GL_CLIP_DISTANCE0);
		unbindFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// shader calls
		waterShader.use();
		waterShader.setMat4("_Model", waterTransform.modelMatrix());
		waterShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		waterShader.setVec3("_CameraPos", camera.position);
		waterShader.setVec3("_LightPos", light.pos);
		waterShader.setVec4("_LightColor", light.color);
		waterShader.setVec3("_View", glm::normalize(camera.target - camera.position));
		waterShader.setFloat("_Tiling", waterTiling);
		waterShader.setInt("reflectionTex", 7);
		waterShader.setInt("refractionTex", 8);

		// draw scene
		waterMesh.draw();
		
		for (int i = 0; i < NUM_OBJS; i++) { aObjs[i].UpdateRotation(); }
		objRend.Render(aObjs, NUM_OBJS); // render outlined objects

		// skybox
		glDepthFunc(GL_LEQUAL); // depth
		skyboxShader.use();
		skyboxShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		renderSkybox(skyboxVAO, cubemapTexture);
		glDepthFunc(GL_LESS); // depth

		drawUI(deltaTime);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");

	delete[] aObjs;
}
