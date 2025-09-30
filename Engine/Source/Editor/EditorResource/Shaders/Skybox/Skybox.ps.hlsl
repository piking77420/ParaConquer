
[[vk::combinedImageSampler]]
TextureCube<float4> skyboxTex : register(t0,space1);
[[vk::combinedImageSampler]]
SamplerState skyboxSampler : register(s0, space1); 
//https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type

struct PSInput
{
    float4 Pos : SV_POSITION;
    float3 dir : TEXCOORD0;
};


float4 Main(PSInput input) : SV_TARGET
{
    // careful do not put it in global scope
    //  it will be   compile as a global buffer by dxc
    const float gamma = 2.2f;

    float3 color = pow(skyboxTex.Sample(skyboxSampler, input.dir).rgb, gamma); // linear space to SRGB space // tone mapp comes later
    return float4(color, 1.0f);
}   