#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

out vec4 clipSpace;
out vec2 TexCoords;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

uniform vec4 _Plane;

const float tiling = 6.0;

void main()
{
	clipSpace = _ViewProjection * _Model * vec4(vPos, 1.0);

	gl_ClipDistance[0] = -1;
	gl_Position = clipSpace;
	TexCoords = vec2(vPos.x / 2.0 + 0.5, vPos.y / 2.0 + 0.5) * tiling;
}
