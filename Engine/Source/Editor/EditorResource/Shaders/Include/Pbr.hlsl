
#ifndef PBR_HEADER
#define PBR_HEADER

#include "Math.hlsl" // Make sure PI / InvPI are defined here

//https://github.com/google/filament
//https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

// D term (GGX / Trowbridge-Reitz)
float D_GGX(float NoH, float roughness)
{
    float a = NoH * roughness;
    float k = roughness / (1.0 - NoH * NoH + a * a);
    return k * k * (1.0 / PI);
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

// Geometry term (Smith GGX)
float G_GGX(float NoV, float NoL, float roughness)
{
    float a = roughness * roughness;
    float GGXV = GGX(NoV, a);
    float GGXL = GGX(NoL, a);
    return GGXV * GGXL;
}

float F_Schlick(float u, float f0, float f90)
{
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float Fd_Burley(float NoV, float NoL, float LoH, float roughness)
{
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(NoL, 1.0, f90);
    float viewScatter = F_Schlick(NoV, 1.0, f90);
    return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert()
{
    return 1.0 / PI;
}

// Full BRDF combining specular + diffuse
float3 BRDF(float3 diffuseColor, float NoV, float NoL, float NoH, float LoH, float roughness)
{
    float D = D_GGX(NoH, roughness);
    float3 F = F_Schlick(LoH, float3(0.04, 0.04, 0.04));
    float V = G_GGX(NoV, NoL, roughness);

    float3 Fr = (D * V * F) / (4.0 * NoV * NoL);

    float3 Fd = diffuseColor * Fd_Burley(NoV, NoL, LoH, roughness) * (float3(1.0, 1.0, 1.0) - F);

    return Fr + Fd;
}
#endif //PBR_HEADER
