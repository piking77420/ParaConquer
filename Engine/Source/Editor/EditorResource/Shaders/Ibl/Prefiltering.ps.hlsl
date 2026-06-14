#include "Math.hlsl"
#include "Pbr.hlsl"

[[vk::combinedImageSampler]]
TextureCube<float4> EnvironementMap : register(t0, space0);
[[vk::combinedImageSampler]]
SamplerState EnvironementMapSampler : register(s0, space0); 
//https://github.com/microsoft/DirectXShaderCompiler/wiki/Vulkan-combined-image-sampler-type

struct PushConstant
{
    float4x4 ViewProjection;
    float Metadata; // roughness here
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

float2 Hammersley2d(uint i, uint N) 
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10;
	return float2(float(i) /float(N), rdi);
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float Roughness)
{
    float A = Roughness * Roughness;
	
    float Phi = 2.0 * PI * Xi.x;
    float CosTheta = sqrt((1.0 - Xi.y) / (1.0 + (A * A - 1.0) * Xi.y));
    float SinTheta = sqrt(1.0 - CosTheta* CosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(Phi) * SinTheta;
    H.y = sin(Phi) * SinTheta;
    H.z = CosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 Up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 Tangent   = normalize(cross(Up, N));
    float3 Bitangent = cross(N, Tangent);
	
    float3 SampleVec = Tangent * H.x + Bitangent * H.y + N * H.z;
    return normalize(SampleVec);
}  

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 
    float3 Irradiance = float3(0.0, 0.0, 0.0);  

    const uint SAMPLE_COUNT = 4096u;
    float3 N = normalize(input.TexCoord);    
    float3 R = N;
    float3 V = R;

    float TotalWeight = 0.0;
    float3 PrefilteredColor = float3(0.0, 0.0, 0.0);    
    float Roughness = pushConstant.Metadata;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley2d(i, SAMPLE_COUNT);
        float3 H  = ImportanceSampleGGX(Xi, N, Roughness);
        float3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {   
            float NoH = max(dot(N, H), 0.0);
			float HoV = max(dot(V, H), 0.0);

            float D = D_GGX(NoH, Roughness);
            float Pdf = (D * NoH / (4.0 * HoV)) + 0.0001;

            uint Width;
            uint Height;
            uint MipCount;
            EnvironementMap.GetDimensions(0, Width, Height, MipCount);

            // Solid angle of 1 pixel across all cube faces
            float SaTexel  = 4.0 * PI / (6.0 * Width * Height);
            // Biased (+1.0) mip level for better result
            float SaSample = 1.0 / (float(SAMPLE_COUNT) * Pdf + 0.0001);
            float MipLevel = Roughness == 0.0 ? 0.0 : 0.5 * log2(SaSample / SaTexel); 
            
            PrefilteredColor += EnvironementMap.SampleLevel(EnvironementMapSampler, L, MipLevel).rgb * NdotL;
            TotalWeight += NdotL;
        }
    }

    outPut.Color = float4(PrefilteredColor / TotalWeight, 1.0);
    return outPut;
}