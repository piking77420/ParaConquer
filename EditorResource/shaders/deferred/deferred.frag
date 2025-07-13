#version 450

#include "camera.glsl"
#include "light.glsl"
#include "pbr.glsl"

layout(set = GBUFFER_SET, binding = G_ALBEDO, input_attachment_index = 0) uniform subpassInput inputAlbedo;
layout(set = GBUFFER_SET, binding = G_NORMAL, input_attachment_index = 1) uniform subpassInput inputNormal;
layout(set = GBUFFER_SET, binding = G_ROUGNESS_METALLIC_AO, input_attachment_index = 2) uniform subpassInput inputRoughnessMettalicAo;
layout(set = GBUFFER_SET, binding = G_WORLD_POSITION, input_attachment_index = 3) uniform subpassInput inputWorldPosition;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 uv;

vec3 GetNormal(vec2 packedNormal)
{
    float f = dot(packedNormal, packedNormal);
    vec3 n;
    n.xy = 2.0 * packedNormal;
    n.z = f - 1.0;
    return normalize(n);
}

void main()
{
    vec3 albedo = subpassLoad(inputAlbedo).rgb;
    vec2 packedNormal = subpassLoad(inputNormal).xy;
    vec3 normal = GetNormal(packedNormal);
    vec3 roughnessMettalicAo = subpassLoad(inputRoughnessMettalicAo).rgb;
    vec3 worldPositionTexSampler = subpassLoad(inputWorldPosition).rgb;
    
    
    
    
    

    outColor = vec4(normal, 1.0);
}
