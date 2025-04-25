#version 450

// Vertex attributes
layout(location = 0) in vec3 vPos; // Vertex position in model space
layout(location = 1) in vec3 vNormal; // Vertex position in model space
layout(location = 2) in vec2 vTexCoord; // Vertex texture coordinate (UV)

out Surface {
	vec4 WorldPos; // Vertex position in world space
	vec3 WorldNormal; // Vertex normal in world space
	vec2 TexCoord;
	vec4 FragPosLightSpace;
} vs_out;

uniform mat4 _Model; // Model->World Matrix
uniform mat4 _ViewProjection; // Combined View->Projection Matrix
uniform mat4 _LightSpaceMatrix;

uniform vec4 _Plane;

void main() {
	// Send texture coords to fragment shader
	vs_out.TexCoord = vTexCoord;

	// Transform vertex position to World Space.
	vs_out.WorldPos = vec4(_Model * vec4(vPos,1.0));

	// Transform vertex normal to world space using Normal Matrix
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

	// Transform vertex world position to light space
	vs_out.FragPosLightSpace = _LightSpaceMatrix * vs_out.WorldPos;

	gl_ClipDistance[0] = dot(vs_out.WorldPos, _Plane);

	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
