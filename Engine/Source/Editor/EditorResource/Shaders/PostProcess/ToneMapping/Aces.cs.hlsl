#include "Func.hlsl"

Texture2D<float4> HdrImage : register(t0, space0);

[[vk::image_format("rgba16f")]]
RWTexture2D<float4> RgbImage : register(u1, space0);

// From http://filmicgames.com/archives/75
float3 Uncharted2Tonemap(float3 x)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;

    return ((x * (A * x + C * B) + D * E) /
            (x * (A * x + B)     + D * F)) - E / F;
}

[numthreads(16, 16, 1)]
void Main(uint3 DTid : SV_DispatchThreadID)
{
    const float exposure = 1.f;
    const float whitePoint = 11.2f;

    uint2 gid = DTid.xy;

    uint width, height;
    HdrImage.GetDimensions(width, height);

    if (gid.x >= width || gid.y >= height)
        return;

    float4 hdr = HdrImage[gid];

    // HDR linear RGB
    float3 rgb = hdr.rgb * exposure;
    rgb = max(rgb, float3(0.0, 0.0, 0.0));

    // Filmic tonemap
    float3 color = Uncharted2Tonemap(rgb);

    // White point normalization
    float3 whiteScale = 1.0 / Uncharted2Tonemap(float3(whitePoint, whitePoint, whitePoint));
    color *= whiteScale;

    // Clamp before writing to rgba8
    color = saturate(color);

    // If RgbImage is rgba8 UNORM and you expect display output, convert to sRGB.
    //color = LinearToSRGB(color);

    RgbImage[gid] = float4(color, saturate(hdr.a));
}