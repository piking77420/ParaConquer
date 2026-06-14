#include "Math.hlsl"
#include "Pbr.hlsl"


struct PsInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PsOutput
{
    float2 Color : SV_TARGET;
};

float2 IntegrateBRDF(float NoV, float Roughness)
{
    float3 V;
    V.x = sqrt(1.0 - NoV*NoV);
    V.y = 0.0;
    V.z = NoV;

    float A = 0.0;
    float B = 0.0;
    const uint SAMPLE_COUNT = 1024u;
    float3 N = float3(0.0, 0.0, 1.0);

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley2d(i, SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, N, Roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NoL = max(L.z, 0.0);
        float NoH = max(H.z, 0.0);
        float VoH = max(dot(V, H), 0.0);

        if(NoL > 0.0)
        {
            float Vis = V_SmithGGXCorrelated(NoV, NoL, Roughness);
            float G_Vis = 4.0 * Vis * NoL * VoH / max(NoH, 1e-5);
            float Fc = pow(1.0 - VoH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return float2(A, B);
}

PsOutput Main(PsInput input) : SV_TARGET
{
    PsOutput outPut; 
    outPut.Color = IntegrateBRDF(input.TexCoord.x, input.TexCoord.y);
    //outPut.Color = float2(255,0);
    return outPut;
}