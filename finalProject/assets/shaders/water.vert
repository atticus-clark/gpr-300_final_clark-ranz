#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec4 ClipSpace;
out vec2 TexCoords;
out vec3 WorldNormal;
//out vec4 FragPosLightSpace;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform mat4 lightSpaceMatrix;

//uniform vec4 _Plane;

const float tiling = 6.0;

void main()
{
	vec4 worldPos = _Model * vec4(vPos, 1.0);

	ClipSpace = _ViewProjection * _Model * vec4(vPos, 1.0);

	TexCoords = vTexCoord;//vec2(vPos.x / 2.0 + 0.5, vPos.y / 2.0 + 0.5) * tiling;

	WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

	//FragPosLightSpace = lightSpaceMatrix * vec4(vec3(_Model * vec4(vPos, 1.0)), 1.0);

	//gl_ClipDistance[0] = dot(worldPos, _Plane);
	gl_Position = ClipSpace;
}