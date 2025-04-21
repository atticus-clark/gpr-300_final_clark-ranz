#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

out vec4 ClipSpace;
out vec2 TexCoords;
out vec3 WorldNormal;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

uniform vec4 _Plane;

const float tiling = 6.0;

void main()
{
	ClipSpace = _ViewProjection * _Model * vec4(vPos, 1.0);

	TexCoords = vec2(vPos.x / 2.0 + 0.5, vPos.y / 2.0 + 0.5) * tiling;

	WorldNormal = vNormal;

	gl_ClipDistance[0] = -1;
	gl_Position = ClipSpace;
}