#version 450

out vec4 FragColor; // The color of this fragment

in Surface {
	vec4 WorldPos; // Vertex position in world space
	vec3 WorldNormal; // Vertex normal in world space
	vec2 TexCoord;
	vec4 FragPosLightSpace;
} fs_in;

struct Material {
	float Ka; // Ambient coefficient (0-1)
	float Kd; // Diffuse coefficient (0-1)
	float Ks; // Specular coefficient (0-1)
	float Shininess; // Affects size of specular highlight
};
uniform Material _Material;

uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
uniform vec4 _LightColor;
uniform vec3 _LightPos;
uniform vec3 _EyePos;

uniform sampler2D _DiffuseTexture;
uniform sampler2D _ShadowMap;

uniform float _MaxBias;
uniform float _MinBias;

uniform float _CelLevels;

/* code taken from the LearnOpenGL tutorial on Shadow Mapping
* https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping */
float ShadowCalculation(vec4 fragPosLightSpace, float normalDotLightDir)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(_ShadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // apply slope-scale bias to eliminate shadow acne
    float bias = max(_MaxBias * (1.0 - normalDotLightDir), _MinBias);

    // check whether current frag pos is in shadow (with PCF)
    float shadow = 0;
    vec2 texelSize = 1.0 / textureSize(_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // delete shadows for objects past the light far plane (eliminate oversampling)
    if(projCoords.z > 1.0) { shadow = 0.0f; }

    return shadow;
}

/* code taken from the LearnOpenGL tutorial on Shadow Mapping
* https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping */
void main() {
	vec3 color = texture(_DiffuseTexture, fs_in.TexCoord).rgb;
    vec3 normal = normalize(fs_in.WorldNormal);

    // ambient
    vec3 ambient = _Material.Ka * _AmbientColor;

    // diffuse
    vec3 lightDir = normalize(_LightPos - fs_in.WorldPos.rgb);
    float diff = max(dot(lightDir, normal), 0.0);
    diff = floor(diff * _CelLevels) / _CelLevels; // cel shading
    vec3 diffuse = _Material.Kd * diff * _LightColor.rgb;

    // specular
    vec3 viewDir = normalize(_EyePos - fs_in.WorldPos.rgb);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), _Material.Shininess);
    spec = floor(spec * _CelLevels) / _CelLevels; // cel shading
    vec3 specular = _Material.Ks * spec * _LightColor.rgb;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, dot(normal, lightDir));
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    FragColor = vec4(lighting, 1.0);
}

/* cel shading code taken from "OpenGL 3D Game Tutorial 30: Cel Shading" by Thin Matrix
 * https://www.youtube.com/watch?v=dzItGHyteng
 * "# = floor(# * _CelLevels) / _CelLevels;" is the only line of code, used twice */
