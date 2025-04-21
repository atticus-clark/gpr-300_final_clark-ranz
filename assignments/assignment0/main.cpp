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
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

	// outlined objects //
	ew::Mesh cubeMesh = ew::createCube(2.0f);
	ew::Transform cubeTransform = ew::Transform();
	//GLuint cubeTexture = ew::loadTexture("assets/brick_color.jpg");

	ew::Mesh cylinderMesh = ew::createCylinder(1.0f, 2.0f, 25);
	ew::Transform cylinderTransform = ew::Transform();
	//GLuint cylinderTexture = ew::loadTexture("assets/brick_color.jpg");

	ew::Mesh sphereMesh = ew::createSphere(1.0f, 25);
	ew::Transform sphereTransform = ew::Transform();
	//GLuint sphereTexture = ew::loadTexture("assets/brick_color.jpg");

	cubeTransform.position = glm::vec3(-3.0f, 0.0f, 0.0f);
	cylinderTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	sphereTransform.position = glm::vec3(3.0f, 0.0f, 0.0f);

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
	ew::Shader waterShader = ew::Shader("assets/water.vert", "assets/water.frag");
	ew::Shader simpleShader = ew::Shader("assets/simple.vert", "assets/simple.frag");

	// textures
	glBindTextureUnit(0, reflectionTex);
	glBindTextureUnit(1, refractionTex);
	GLuint dudvMap = ew::loadTexture("assets/waterDUDV.png");
	glBindTextureUnit(2, dudvMap);

	//glBindTextureUnit(3, cubeTexture);
	//glBindTextureUnit(4, cylinderTexture);
	//glBindTextureUnit(5, sphereTexture);

	// shader calls
	waterShader.use();
	waterShader.setInt("reflectionTex", 0);
	waterShader.setInt("refractionTex", 1);
	waterShader.setInt("dudvMap", 2);

	// outlined object colors
	float outlineScale = 1.05f;
	glm::vec4 outlineColor = glm::vec4(1.0f);

	glm::vec4 cubeColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 cylinderColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 sphereColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);
		float distance = 2 * (camera.position.y - waterHeight);
		glm::mat4 viewProjMatrix = camera.projectionMatrix() * camera.viewMatrix();

		//moveFactor += WAVE_SPEED * deltaTime;
		//moveFactor %= 1;

		// RENDER //
		glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		// using the stencil buffer, gotta remember to clear it every frame!
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_CLIP_DISTANCE0);
		glStencilMask(0x00);

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

		// ------------------------------------ OUTLINED OBJECTS ------------------------------------- //

		/* code ADAPTED from the LearnOpenGL tutorial on Stencil Testing
		* https://learnopengl.com/Advanced-OpenGL/Stencil-testing */

		// shader setup
		simpleShader.use();
		simpleShader.setMat4("_ViewProjection", viewProjMatrix);

		// ------------------------------------------------------------------------------------------- //
		// 1st render pass: Draw objects as normal while writing to the stencil buffer.

		// setup
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		// draw cube
		simpleShader.setMat4("_Model", cubeTransform.modelMatrix());
		simpleShader.setVec4("_Color", cubeColor);
		cubeMesh.draw();

		// draw cylinder
		simpleShader.setMat4("_Model", cylinderTransform.modelMatrix());
		simpleShader.setVec4("_Color", cylinderColor);
		cylinderMesh.draw();

		// draw sphere
		simpleShader.setMat4("_Model", sphereTransform.modelMatrix());
		simpleShader.setVec4("_Color", sphereColor);
		sphereMesh.draw();
		
		// ------------------------------------------------------------------------------------------- //
		// 2nd render pass: Draw objects slightly scaled-up without writing to the stencil buffer.

		/* The stencil buffer was filled with some 1s in the previous step; those parts are not
		*  drawn, thus only drawing the size differences between the objects in the 1st and 2nd
		*  pass, which looks like an outline. */
		
		// setup 
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		simpleShader.use();
		simpleShader.setVec4("_Color", outlineColor);
		
		glm::mat4 scaledModel;
		glm::vec3 scale = glm::vec3(outlineScale, outlineScale, outlineScale);

		// draw cube outline
		scaledModel = glm::scale(cubeTransform.modelMatrix(), scale);
		simpleShader.setMat4("_Model", scaledModel);
		cubeMesh.draw();

		// draw cylinder outline
		scaledModel = glm::scale(cylinderTransform.modelMatrix(), scale);
		simpleShader.setMat4("_Model", scaledModel);
		cylinderMesh.draw();

		// draw sphere outline
		scaledModel = glm::scale(sphereTransform.modelMatrix(), scale);
		simpleShader.setMat4("_Model", scaledModel);
		sphereMesh.draw();
		
		// cleanup
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		// ------------------------------------ END OUTLINED OBJECTS --------------------------------- //

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
