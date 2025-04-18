#version 450

in vec4 clipSpace;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D dudvMap;

uniform float moveFactor;

const float waveStrength = 0.02;

void main()
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(TexCoords.x + moveFactor, TexCoords.y)).rg * 2.0 - 1.0) * waveStrength;
	vec2 distortion2 = (texture(dudvMap, vec2(TexCoords.x, TexCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;

	vec2 distortion = distortion1 + distortion2;

	refractTexCoords += distortion;
	reflectTexCoords += distortion;

	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	
	vec4 reflectColor = texture(reflectionTex, reflectTexCoords);
	vec4 refractColor = texture(refractionTex, refractTexCoords);

	FragColor = mix(reflectColor, refractColor, 0.5);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}
