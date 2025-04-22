#version 450

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 _ViewProjection;

void main()
{
    TexCoords = aPos;
    gl_Position = _ViewProjection * vec4(aPos, 1.0);
} 