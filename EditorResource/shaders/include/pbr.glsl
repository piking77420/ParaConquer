#include "math.glsl"

//https://github.com/google/filament
//https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

float D_GGX(float NoH, float roughness) 
{
    float a = NoH * roughness;
    float k = roughness / (1.0 - NoH * NoH + a * a);
    return k * k * (1.0 / PI);
}
    

// FO soudld be 0.04 for most dielectric
// F1 should be 1
vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float GGX(float NdotV, float a2)
{
    float num = 2.0 * NdotV;
    float denum = NdotV + sqrt(a2 + (1.0 - a2) * (NdotV * NdotV));
    return num / denum;
}

// NoV dot(Normal , ViewDir)
// Nol dot(Normal, lightIncident)
float G_GGX(float NoV, float NoL, float roughness) {
    float a = roughness * roughness; // aSq
    float GGXV = GGX(NoV, a);
    float GGXL = GGX(NoL, a);
    return GGXV * GGXL;
}


float F_Schlick(float u, float f0, float f90) {
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float Fd_Burley(float NoV, float NoL, float LoH, float roughness) {
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(NoL, 1.0, f90);
    float viewScatter = F_Schlick(NoV, 1.0, f90);
    return lightScatter * viewScatter * (1.0 / PI);
}

float Fd_Lambert()
{
    return 1.0 / PI;
}

vec3 BRDF(vec3 diffuseColor, float NoV, float NoL, float NoH, float LoH, float roughness)
{
    float D = D_GGX(NoH, roughness);
    vec3  F = F_Schlick(LoH, vec3(0.04));
    float V = G_GGX(NoV, NoL, roughness);

    vec3 Fr = (D * V * F) / 4 * NoV * NoL;

    vec3 Fd = diffuseColor * Fd_Burley(NoV, NoL, LoH, roughness) * (vec3(1.0) - F);
    //vec3 Fd = diffuseColor * Fd_Lambert() * (vec3(1.0) - F);

    return Fr + Fd;
}