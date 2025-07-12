#version 450

#include "camera.glsl"

layout(set = SCENE_DESCRIPTOR_SET, binding = LIGHTDATA_BINDING) uniform LightData
{
    vec3 direction;
    float padding;
    vec3 color;
    float intensity;
    vec3 ambiant;
    float padding3;
} lightData;


layout(set = GBUFFER_SET, binding = G_ALBEDO) uniform sampler2D albedoTexSampler;
layout(set = GBUFFER_SET, binding = G_NORMAL) uniform sampler2D normalTexSampler;
layout(set = GBUFFER_SET, binding = G_ROUGNESS_METALLIC_AO) uniform sampler2D rougnessMetallicAoTexSampler;
layout(set = GBUFFER_SET, binding = G_WORLD_POSITION) uniform sampler2D worldPositionTexSampler;


layout(location = 0) out vec4 outColor;


layout(location = 0) in vec2 uv;

vec3 GetNormal(vec2 packedNormal)
{
    float nZ = length(packedNormal.xy) * (2 - 1);
    vec2 nXY = normalize(packedNormal) * sqrt(1 - nZ * nZ);
    
    return vec3(nXY.x, nXY.y, nZ);
}

void main() 
{
    // Get Data From Gbuffer
    vec3 albedo = texture(albedoTexSampler, uv).xyz;
    vec3 normal = GetNormal(texture(normalTexSampler, uv).xy);
    vec3 roughnessMettalicAo = vec3(texture(rougnessMetallicAoTexSampler, uv).xyz);
    vec3 worldPositionTexSampler = vec3(texture(worldPositionTexSampler, uv).xyz);
    
    outColor = vec4(normal, 1.0);
}