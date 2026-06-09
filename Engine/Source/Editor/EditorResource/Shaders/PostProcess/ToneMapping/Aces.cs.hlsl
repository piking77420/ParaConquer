
#include "Func.hlsl"

Texture2D<float4> HdrImage : register(t0, space0);


[[vk::image_format("rgba8")]]
RWTexture2D<float4> RgbImage : register(u1, space0);

// ACES RRT + ODT fit function
float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

static const float3x3 ACESInputMat = float3x3(
    0.59719, 0.35458, 0.04823,
    0.07600, 0.90834, 0.01566,
    0.02840, 0.13383, 0.83777
);

static const float3x3 ACESOutputMat = float3x3(
    1.60475, -0.53108, -0.07367,
   -0.10208, 1.10813, -0.00605,
   -0.00327, -0.07276, 1.07602
);



[numthreads(16, 16, 1)]
void Main(uint3 DTid : SV_DispatchThreadID)
{
    const float exposure = 1.f; 

    uint2 gid = DTid.xy;
    uint2 size;
    HdrImage.GetDimensions(size.x, size.y);

    if (gid.x >= size.x || gid.y >= size.y) // we assume thant HdrImage.GetDimensions == RgbImage
        return;

    // Read HDR color (linear)
    float4 hdr = HdrImage[gid]  * exposure;

    float3 color = mul(ACESInputMat, hdr.rgb); // Linear sRGB -> AP1
    color = RRTAndODTFit(color); // RRT+ODT in AP1 space
    color = mul(ACESOutputMat, color); // AP1 -> Linear sRGB
    color = ApplyGammaCorrection(color, 2.2);

    RgbImage[gid] = float4(color, hdr.a);
}
