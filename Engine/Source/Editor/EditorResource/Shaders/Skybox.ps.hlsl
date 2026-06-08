

[[vk::combinedImageSampler]]
TextureCube<float4> SkyboxTex : register(t0,space1);
[[vk::combinedImageSampler]]
SamplerState SkyboxSampler : register(s0, space1); 

struct PushConstant
{
    float4x4 ViewProjection;
};

[[vk::push_constant]]
PushConstant pushConstant;

struct PsInput
{
    float4 Position : SV_POSITION;
    #if defined(USE_UV)
    float3 TexCoord : TEXCOORD0;
    #endif
};



float4 Main(PsInput input) : SV_TARGET
{
    float3 Color = SkyboxSampler.Sample(SkyboxTex, TexCoord).rgb;
    
    return vec4(color, 1.0);
}