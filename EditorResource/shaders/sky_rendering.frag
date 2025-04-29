#version 450

layout(set = 1, binding = 2) uniform sampler2D texSampler;

layout(set = 0, binding = 1) uniform LightData
{
    vec3 direction;
    float padding;
    vec3 color;
    float intensity;
    vec3 ambiant;
    float padding3;
} lightData;


layout(location = 0) in vec3 fragpos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = lightData.direction;  
    float diff = max(dot(normal, -lightDir), 0.0);
    vec3 diffuse = diff * lightData.color * lightData.intensity;

    vec3 outColorVec3 = (diffuse + lightData.ambiant) * texture(texSampler, fragTexCoord).xyz;

    outColor = vec4(outColorVec3, 1.0);
}