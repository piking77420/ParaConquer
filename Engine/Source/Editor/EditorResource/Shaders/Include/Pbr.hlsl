
#ifndef PBR_HEADER
#define PBR_HEADER

#include "Math.hlsl" // Make sure PI / InvPI are defined here

//https://github.com/google/filament
//https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

static const float3 DIELECTRIC_F0 = float3(0.04, 0.04, 0.04);

// D term (GGX / Trowbridge-Reitz)
float D_GGX(float NoH, float Roughness)
{
    float alpha = Roughness * Roughness;
    float a2 = alpha * alpha;
    float denom = NoH * NoH * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

// Fresnel term (Schlick approximation)
float3 F_Schlick(float u, float3 f0)
{
    return f0 + (float3(1.0, 1.0, 1.0) - f0) * pow(1.0 - u, 5.0);
}

// Geometry term helper (GGX)
float GGX(float NdotV, float a2)
{
    float num = 2.0 * NdotV;
    float denum = NdotV + sqrt(a2 + (1.0 - a2) * (NdotV * NdotV));
    return num / denum;
}


float V_SmithGGXCorrelated(float NoV, float NoL, float Roughness)
{
    float alpha = Roughness * Roughness;
    float a2 = alpha * alpha;
    // Geometry term (Smith GGX)
    float GV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);

    return 0.5 / (GV + GL);
}

float3 F_SchlickR(float CosTheta, float3 F0, float Roughness)
{
    float minus1Roughness = 1.0 - Roughness;
	return F0 + (max(float3(minus1Roughness, minus1Roughness, minus1Roughness), F0) - F0) * pow(1.0 - CosTheta, 5.0);
}

float F_Schlick(float u, float f0, float f90)
{
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float Fd_Burley(float NoV, float NoL, float LoH, float Roughness)
{
    float f90 = 0.5 + 2.0 * Roughness * LoH * LoH;
    float lightScatter = F_Schlick(NoL, 1.0, f90);
    float viewScatter = F_Schlick(NoV, 1.0, f90);
    return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert()
{
    return 1.0 / PI;
}

// Full BRDF combining specular + diffuse
float3 BRDF(float3 BaseColor, float Metallic, float Roughness, float NoV, float NoL, float NoH, float LoH, float3 F0)
{
    // Specular F0 from metallic workflow

    // Normal Distrubution Function 
    // Approximate the amout of surface microfacet that are alligned with H
    float D = D_GGX(NoH, Roughness);
    // Self shadowing property of the microfacets 
    // how other microfacets shadow themselft
    float V = V_SmithGGXCorrelated(NoV, NoL, Roughness); // G / (4 * NoV * NoL)
    float3 F = F_Schlick(LoH, F0);

    float3 Fr = D * V * F; 

    float3 kD = (1.0 - F) * (1.0 - Metallic); // diffuse coefficient kf is equal to F and F varies to 0 to 1
    float3 Fd = kD * BaseColor * Fd_Burley(NoV, NoL, LoH, Roughness);

    return Fr + Fd;
}


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

#endif //PBR_HEADER
