#version 450

in vec4 ClipSpace;
in vec2 TexCoords;
in vec3 ToCamera;
in vec3 ToLight;

out vec4 FragColor;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);

uniform vec3 _View;

uniform float moveFactor;

const float waveStrength = 0.02;
const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main()
{
	// projective texture mapping
	vec2 ndc = (ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoords.x + moveFactor, TexCoords.y)).rg*0.1;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	refractTexCoords += totalDistortion;
	reflectTexCoords += totalDistortion;

	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	
	vec4 reflectColor = texture(reflectionTex, reflectTexCoords);
	vec4 refractColor = texture(refractionTex, refractTexCoords);

	vec3 cameraVec = normalize(ToCamera);
	float refractiveFactor = dot(cameraVec, vec3(0, 1, 0));
	refractiveFactor = pow(refractiveFactor, 2);

	vec4 normalColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b, normalColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(ToLight), normal);
	float specular = max(dot(reflectedLight, _View), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = _LightColor * specular * reflectivity;

	FragColor = mix(reflectColor, refractColor, refractiveFactor);
	// make it more blue
	FragColor = mix(FragColor, vec4(0.0, 0.1, 0.3, 1.0), 0.1 + vec4(specularHighlights, 0.0));
}