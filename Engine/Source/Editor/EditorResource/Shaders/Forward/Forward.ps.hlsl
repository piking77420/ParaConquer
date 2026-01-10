struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
};


[[vk::combinedImageSampler]]
Texture2D AlbedoTexture : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState AlbedoSampler : register(s0, space1);

float4 Main(PSInput input) : SV_Target
{
   
    float3 albedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord).rgb;
    
    return float4(albedo, 1);
}