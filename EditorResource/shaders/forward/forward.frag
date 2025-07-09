#version 450

layout(set = MATERIAL_DESCRIPTOR_SET, binding = ALBEDO_BINDING) uniform sampler2D texSampler;

layout(set = SCENE_DESCRIPTOR_SET, binding = LIGHTDATA_BINDING) uniform LightData
{
    vec3 direction;
    float padding;
    vec3 color;
    float intensity;
    vec3 ambiant;
    float padding3;
} lightData;

layout(set = SCENE_DESCRIPTOR_SET, binding = FORWARD_SKYBOX_CUBEMAP) uniform samplerCube cubeMap;

layout(location = 0) in vec3 fragpos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightData.direction); // Ajout de normalize
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightData.color * lightData.intensity;

    vec3 baseColor = texture(texSampler, fragTexCoord).rgb;
    vec3 lighting = (diffuse + lightData.ambiant) * baseColor;

    vec3 viewDir = normalize(fragpos);
    vec3 reflectedDir = reflect(viewDir, normal);

    vec3 skyBoxColor = texture(cubeMap, reflectedDir).rgb;

    vec3 finalColor = lighting + skyBoxColor * 0.2; 

    outColor = vec4(skyBoxColor * 0.2, 1.0);

}