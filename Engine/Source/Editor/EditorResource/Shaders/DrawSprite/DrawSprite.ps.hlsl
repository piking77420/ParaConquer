
[[vk::combinedImageSampler]]
Texture2D texSampler : register(t0, space1); 
[[vk::combinedImageSampler]]
SamplerState texSamplerSampler : register(s0, space1);

struct PS_IN
{
    float2 texCoord : TEXCOORD0;
};

float4 Main(PS_IN input) : SV_TARGET
{
    return texSampler.Sample(texSamplerSampler, input.texCoord);
}