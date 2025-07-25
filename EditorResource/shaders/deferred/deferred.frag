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

vec3 Decode(vec2 f)
{
    f = f * 2.0 - 1.0; // Remap [0,1] -> [-1,1]
    vec3 n = vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    if (n.z < 0.0)
    {
        float oldX = n.x;
        n.x = (1.0 - abs(n.y)) * sign(oldX);
        n.y = (1.0 - abs(oldX)) * sign(n.y);
    }
    return normalize(n);
}

void main()
{
    vec3 albedo = subpassLoad(inputAlbedo).rgb;
    vec3 normal = Decode(subpassLoad(inputNormal).rg);
    //vec3 roughnessMettalicAo = subpassLoad(inputRoughnessMettalicAo).rgb;
    vec3 worldPositionTexSampler = subpassLoad(inputWorldPosition).rgb;


    outColor = vec4(normal, 1);
    
    

    //outColor = vec4(albedo * dot(lightSceneData.dirlights[0].direction, normal) , 1.0);
}
