
[[vk::combinedImageSampler]]
Texture2D equirectangularMapTexture : register(t0, space0);
[[vk::combinedImageSampler]]
SamplerState equirectangularMapSampler : register(s0, space0);

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
    const float2 invAtan = float2(
        0.15915494309189535f,
        0.3183098861837907f
    );

    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;


// Best solution so far
#if Y_UP_NDC == 0
    uv.y = 1.0f - uv.y;
#endif

    return uv;  
}

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 

    float2 UV = SampleSphericalMap(normalize(input.TexCoord));
    float3 Color = equirectangularMapTexture.Sample(equirectangularMapSampler, UV).rgb;
#if 0
    float4 Truc = float4(Color, 1.0);

    float3 dir = normalize(input.TexCoord);
    float3 adir = abs(dir);

    if (adir.x > adir.y && adir.x > adir.z)
    {
        outPut.Color = dir.x > 0.0 ? float4(1, 0, 0, 1) : float4(0, 1, 1 , 1); // +X red, -X cyan
    }
    else if (adir.y > adir.z)
    {
        outPut.Color = dir.y > 0.0 ? float4(0, 1, 0 , 1) : float4(1, 0, 1 , 1); // +Y green, -Y magenta
    }
    else
    {
        outPut.Color = dir.z > 0.0 ? float4(0, 0, 1 , 1) : float4(1, 1, 0 , 1); // +Z blue, -Z yellow
    }

    outPut.Color.xyz += Truc * 0.000001f;
    #else 
     outPut.Color = float4(Color, 1.0);
    #endif

    return outPut;
}