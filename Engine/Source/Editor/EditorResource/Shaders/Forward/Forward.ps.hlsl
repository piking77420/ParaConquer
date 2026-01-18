struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
};

#define MATERIAL_SET space1
#include "Material.hlsl"

float4 Main(PSInput input) : SV_Target
{
   
    float3 albedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord).rgb;
    
    albedo += Albedo.rgb * Albedo.a;
    
    return float4(albedo, 1);
}