[[vk::combinedImageSampler]]
Texture2D<float4> texture : register(t0,space0);
[[vk::combinedImageSampler]]
SamplerState texSampler : register(s0, space0); 

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

float4 Main(PSInput input) : SV_TARGET
{
    return float4(texture.Sample(texSampler, input.texCoord).rgb, 1);
}