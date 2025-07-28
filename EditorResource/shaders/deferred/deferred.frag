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
    outColor = vec4(0);
    
    vec3 albedo = subpassLoad(inputAlbedo).rgb;
    vec3 roughnessMettalicAo = subpassLoad(inputRoughnessMettalicAo).rgb;
    vec3 viewSpacePos = subpassLoad(inputWorldPosition).rgb;

    vec3 N = Decode(subpassLoad(inputNormal).rg);
    vec3 V = normalize(viewSpacePos);
    float NoV = abs(dot(N, V)) + 1e-5;
    
    float roughness = roughnessMettalicAo.x * roughnessMettalicAo.x;
    float metallic = roughnessMettalicAo.y;

    // iterate over each light
    
    // for each dirlight
    for(int i = 0; i < lightSceneData.dirLightCount; i++)
    {
        DirectionalData dirlight = lightSceneData.dirlights[i];
        vec3 L = normalize(dirlight.direction);
        vec3 H = normalize(V + L);
        float NoL = max(dot(N, L), 1.0);
        float NoH = max(dot(H, V), 1.0);
        float LoH = max(dot(L, H), 1.0);

        vec3 material = BRDF(albedo, NoV, NoL, NoH, LoH, roughness, metallic);
        vec3 radiance = dirlight.color * dirlight.intensity;
    }
    // for each spothlight
    // TODO
    
    // for each pointLight
    for(int i = 0; i < lightSceneData.pointLightCount; i++)
    {
        PointLightData pointLight = lightSceneData.pointLights[i];
        
        vec3 fragmentViewPos = viewSpacePos;
        vec3 pointLightViewPos = pointLight.position;
        
        vec3 fragmentViewPosToLight = ( pointLightViewPos - fragmentViewPos );
        float fragmentToLightNorm = length(fragmentViewPosToLight);


        if (fragmentToLightNorm >= pointLight.maxRange * pointLight.maxRange)
                continue;

    
        vec3 L = fragmentViewPosToLight / fragmentToLightNorm; // position are in view space
        vec3 H = normalize(V + L);
        float NoL = max(dot(N, L), 1.0);
        float NoH = max(dot(H, V), 1.0);
        float LoH = max(dot(L, H), 1.0);
        vec3 material = BRDF(albedo, NoV, NoL, NoH, LoH, 1, 0);


        float distanceFrag = fragmentToLightNorm;
        float attenuation = GetSquareFalloffAttenuation(-fragmentViewPosToLight, 1.0F / pointLight.maxRange);
        vec3 radiance = pointLight.color * attenuation * pointLight.intensity;
        outColor += vec4(radiance, 1);

    }
    //outColor = vec4(lightSceneData.pointLights[0].color * lightSceneData.pointLights[0].intensity, 1);

}
