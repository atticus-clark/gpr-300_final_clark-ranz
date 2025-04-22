#include "compositeRenderer.h"

void CompositeRenderer::Render(Object* objs, const unsigned int& NUM_OBJS, const glm::mat4& viewProj) {
	shadows.Render(objs, NUM_OBJS);
	regular.Render(objs, NUM_OBJS);
	outlines.Render(objs, NUM_OBJS, viewProj);
}

void CompositeRenderer::Setup(ew::Camera* cameraPtr, LightSource* lightPtr, int* WIDTH, int* HEIGHT) {
	shadows.SetLightPtr(lightPtr);
	shadows.SetScreenSizePtrs(WIDTH, HEIGHT);

	regular.SetCameraPtr(cameraPtr);
	regular.SetLightPtr(lightPtr);
	regular.SetXrayPtr(&xray);

	outlines.SetXrayPtr(&xray);
}
