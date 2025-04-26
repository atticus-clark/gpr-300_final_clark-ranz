#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec4 ClipSpace;
out vec2 TexCoords;
out vec3 ToCamera;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _CameraPos;

const float tiling = 4.0;

void main()
{
	vec4 worldPos = _Model * vec4(vPos, 1.0);

	ClipSpace = _ViewProjection * worldPos;

	TexCoords = vTexCoord * tiling;

	ToCamera = _CameraPos - worldPos.xyz;

	gl_Position = ClipSpace;
}