#include "Func.hlsl"

[[vk::combinedImageSampler]]
TextureCube<float4> skyboxTex : register(t0, space0);
[[vk::combinedImageSampler]]
SamplerState skyboxSampler : register(s0, space0); 
//https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type

struct PSInput
{
    float4 Position : SV_POSITION;
#if defined(USE_UV)
    float3 Dir : TEXCOORD0;
#endif
};

#include "Func.hlsl"

float4 Main(PSInput input) : SV_TARGET
{
    float3 dir = normalize(input.Dir);  

    float3 color = skyboxTex.Sample(skyboxSampler, dir).rgb;

    return float4(color, 1.0f);
}