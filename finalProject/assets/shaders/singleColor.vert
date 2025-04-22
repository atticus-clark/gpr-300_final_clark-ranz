#version 450

// Vertex attributes
layout(location = 0) in vec3 vPos; // Vertex position in model space
layout(location = 1) in vec3 vNormal; // Vertex normal in model space

uniform mat4 _Model; // Model->World Matrix
uniform mat4 _ViewProjection; // Combined View->Projection Matrix

void main() {
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
