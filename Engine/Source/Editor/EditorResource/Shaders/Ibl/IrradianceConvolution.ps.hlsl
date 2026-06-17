#include "Math.hlsl"
#include "Pbr.hlsl"

[[vk::combinedImageSampler]]
TextureCube<float4> EnvironementMap : register(t0, space0);
[[vk::combinedImageSampler]]
SamplerState EnvironementMapSampler : register(s0, space0); 
//https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type


struct PsInput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

struct PsOutput
{
    float4 Color : SV_TARGET;
};

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 
    float3 Irradiance = float3(0.0, 0.0, 0.0);  

    float3 Normal = normalize(input.TexCoord);
    float3 Up = abs(Normal.y) > 0.999 ? float3(1, 0, 0) : float3(0, 1, 0);
    float3 Right = normalize(cross(Up, Normal));
    Up = normalize(cross(Normal, Right));

    float DeltaSample = 0.025;
    int NumbOfSampleZenipth = ((2.f * PI) / DeltaSample) + 1;
    int NumbOfSampleAzimuth = ((0.5f * PI) / DeltaSample) + 1;

    float phi = 0.f;
    for (int x = 0; x < NumbOfSampleZenipth; x++)
    {   
        float theta = 0.f;
        for (int y = 0; y < NumbOfSampleAzimuth; y++)
        {
            float3 TangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 SampleVec = TangentSample.x * Right + TangentSample.y * Up + TangentSample.z * Normal;

            // when Tangent is along the normal aka (cos(theta) == 1)
            // then SampleVec == Normal so cos(theta) is actually the dot product between L and N
            float NdotL = cos(theta);
            float sinTheta = sin(theta); // from jacobian area scaling factor
            float3 Li = EnvironementMap.Sample(EnvironementMapSampler, SampleVec).rgb;

            Irradiance += Li * NdotL * sinTheta;

            theta += DeltaSample;
        }

        phi += DeltaSample;
    }

    // PI because we integrate over (2.f * PI) and (0.5f * PI) => PI
    Irradiance *= PI / float(NumbOfSampleZenipth * NumbOfSampleAzimuth);
    outPut.Color = float4(Irradiance, 1.0);

    return outPut;
}