
#include "Func.hlsl"

Texture2D<float4> HdrImage : register(t0, space0);


[[vk::image_format("rgba8")]]
RWTexture2D<float4> RgbImage : register(u1, space0);

// From http://filmicgames.com/archives/75
float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

[numthreads(16, 16, 1)]
void Main(uint3 DTid : SV_DispatchThreadID)
{
    const float exposure = 4.5f; 

    uint2 gid = DTid.xy;
    uint2 size;
    HdrImage.GetDimensions(size.x, size.y);

    if (gid.x >= size.x || gid.y >= size.y) // we assume thant HdrImage.GetDimensions == RgbImage
        return;

    // Read HDR color (linear)
    float4 hdr = HdrImage[gid]  * exposure;

    float3 color = Uncharted2Tonemap(hdr.rgb);
    color = ApplyGammaCorrection(color, 2.2);

    RgbImage[gid] = float4(color, hdr.a);
}
