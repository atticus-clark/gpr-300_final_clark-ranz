#pragma once

#include <stdio.h>
#include <math.h>
#include <string>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/camera.h>
#include <ew/cameraController.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/texture.h>

#include "../finalProject/assets/program/compositeRenderer.h"

// Global state //
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

// lower resolution due to expense
const int REFLECTION_HEIGHT = 180;
const int REFLECTION_WIDTH = 320;
const int REFRACTION_HEIGHT = 360;
const int REFRACTION_WIDTH = 640;

const float WAVE_SPEED = 0.03f;

ew::Camera camera;
ew::CameraController cameraController;

float moveFactor = 0;

const int NUM_OBJS = 3;
Object* aObjs;
CompositeRenderer* pCompRend;

// creates a color buffer texture for a framebuffer- mandatory
GLuint createTexture(int height, int width)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	return texture;
}

// creates a depth buffer texture for a framebuffer- optional
GLuint createDepthTexture(int height, int width)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	return texture;
}

// creates a depth buffer attachment
GLuint createDepthBuffer(int height, int width)
{
	GLuint depthBuffer = GL_RENDERBUFFER;
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

void bindFramebuffer(GLuint framebuffer, int height, int width)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}

void unbindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void drawUI(float dt) {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings"); // begin window: Settings

	//std::string fpsCounter = "FPS: " + std::to_string(1.0f / dt);
	//ImGui::Text(fpsCounter.data());

	if(ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}

	ImGui::Checkbox("Xray", &(pCompRend->xray));

	if(ImGui::CollapsingHeader("Outlined Objects")) {
		for(int i = 0; i < NUM_OBJS; i++) {
			ImGui::PushID(i);

			std::string header = "Object " + std::to_string(i+1);
			ImGui::Text(header.data());

			ImGui::DragFloat3("Position", &(aObjs[i].transform.position.x), 0.05f);
			ImGui::DragFloat3("Rotation", &(aObjs[i].rotation.x), 0.05f, -360.0f, 360.0f);
			ImGui::DragFloat3("Scale", &(aObjs[i].transform.scale.x), 0.05f);

			ImGui::PopID();
		}
	}

	ImGui::End(); // end window: Settings

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void SetupOutlinedObjs(Object* objs) {
	aObjs = objs; // this is a stupid idea

	objs[0].mesh = ew::createCube(2.0f);
	objs[1].mesh = ew::createCylinder(1.0f, 2.0f, 25);
	objs[2].mesh = ew::createSphere(1.0f, 25);

	objs[0].transform.position = glm::vec3(-3.0f, 0.0f, 0.0f);
	objs[1].transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	objs[2].transform.position = glm::vec3(3.0f, 0.0f, 0.0f);

	objs[0].texture = ew::loadTexture("assets/textures/brick_color.jpg");
	objs[1].texture = ew::loadTexture("assets/textures/brick_color.jpg");
	objs[2].texture = ew::loadTexture("assets/textures/brick_color.jpg");
}
