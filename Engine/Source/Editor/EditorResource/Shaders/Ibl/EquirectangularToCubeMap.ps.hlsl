
[[vk::combinedImageSampler]]
Texture2D equirectangularMapTexture : register(t0, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState equirectangularMapSampler : register(s0, MATERIAL_SET);

struct PushConstant
{
    float4x4 ViewProjection;
};

[[vk::push_constant]]
PushConstant pushConstant;

struct PsInput
{
    float4 Position : SV_POSITION;
};

float2 SampleSphericalMap(vec3 v)
{
    const float2 invAtan = vec2(0.1591, 0.3183);

    float2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 Main(PsInput input) : SV_TARGET
{
    float2 uv = SampleSphericalMap(normalize(input.Position)); // make sure to normalize localPos
    float3 color = equirectangularMapSampler.Sample(equirectangularMapTexture, uv).rgb;
    
    return vec4(color, 1.0);
}