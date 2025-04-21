#version 450

out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D _MainTex;
uniform vec4 _Color;

void main() {
	//FragColor = texture(_MainTex, TexCoords);
	FragColor = _Color;
}
