#version 450

// code adapted for C++ from ThinMatrix's water tutorial
// https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec4 ClipSpace;
out vec2 TexCoords;
out vec3 ToCamera;
out vec3 ToLight;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _CameraPos;
uniform vec3 _LightPos;

uniform float _Tiling;

void main()
{
	vec4 worldPos = _Model * vec4(vPos, 1.0); // world space

	ClipSpace = _ViewProjection * worldPos; // homogenous clip space

	TexCoords = vTexCoord * _Tiling;

	ToCamera = _CameraPos - worldPos.xyz;

	ToLight = worldPos.xyz - _LightPos;

	gl_Position = ClipSpace;
}