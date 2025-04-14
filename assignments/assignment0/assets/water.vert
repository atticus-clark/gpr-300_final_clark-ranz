#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

uniform vec4 _Plane;

void main()
{
	gl_ClipDistance[0] = -1;
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
