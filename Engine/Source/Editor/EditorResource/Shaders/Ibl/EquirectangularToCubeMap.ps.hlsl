
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
    const float invTwoPi = 0.15915494309189535; // 1 / (2*pi)
    const float invPi    = 0.3183098861837907;  // 1 / pi

    float2 uv;
    uv.x = atan2(v.z, v.x) * invTwoPi + 0.5;
    uv.y = 0.5 - asin(clamp(v.y, -1.0, 1.0)) * invPi;

    return uv;
}

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 

    float2 UV = SampleSphericalMap(normalize(input.TexCoord));
    float3 Color = equirectangularMapTexture.Sample(equirectangularMapSampler   , UV).rgb;
    outPut.Color = float4(Color, 1.0);

    return outPut;
}