
[[vk::combinedImageSampler]]
Texture2D equirectangularMapTexture : register(t0, space0);
[[vk::combinedImageSampler]]
SamplerState equirectangularMapSampler : register(s0, space0);

struct PushConstant
{
    float4x4 ViewProjection;
};

[[vk::push_constant]]
PushConstant pushConstant;

struct PsInput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

struct PsOutput
{
    float4 Color : SV_TARGET;
};

float2 SampleSphericalMap(float3 v)
{
    const float2 invAtan = float2(0.1591, 0.3183);

    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 

    float2 UV = SampleSphericalMap(normalize(input.TexCoord));
    float3 Color = equirectangularMapTexture.Sample(equirectangularMapSampler, UV).rgb;
    outPut.Color = float4(Color, 1.0);

    return outPut;
}