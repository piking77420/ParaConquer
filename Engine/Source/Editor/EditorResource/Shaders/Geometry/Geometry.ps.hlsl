
[[vk::combinedImageSampler]]
Texture2D AlbedoTexture : register(t0,space1);
[[vk::combinedImageSampler]]
SamplerState AlbedoSampler : register(s0,space1);

// G-Buffer outputs
struct PsOutput
{
    float4 outColor               : SV_Target0;
    float2 outNormal              : SV_Target1;
    float4 outRoughnessMetallicAo : SV_Target2;
    float4 outViewSpacePosition       : SV_Target3;
};

// Pixel shader input from vertex shader
struct PsInput
{
    float3 inNormal       : NORMAL;
    float4 inViewSpacePosition: TEXCOORD0;
    float2 inTexCoord     : TEXCOORD1;
};

// ----------------------------------------------------------------------------
// Octahedral normal encoding
float2 OctWrap(float2 v)
{
    return (1.0 - abs(v.yx)) * sign(v);
}

float2 EncodeNormal(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    if (n.z < 0.0)
        n.xy = OctWrap(n.xy);
    return n.xy * 0.5 + 0.5;
}

// ----------------------------------------------------------------------------
// Pixel shader
PsOutput Main(PsInput input)
{
    PsOutput output;

    static const float gamma = 2.2;

    float3 albedo = pow(AlbedoTexture.Sample(AlbedoSampler, input.inTexCoord).rgb, gamma);
    output.outColor = float4(albedo, 1.0);

    output.outNormal = EncodeNormal(input.inNormal);

    output.outRoughnessMetallicAo = float4(0.0, 0.0, 0.0, 0.0);

    output.outViewSpacePosition = input.inViewSpacePosition;

    return output;
}
