struct PSInput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD1;
};
#define MATERIAL_SET space1
#include "Material.hlsl"

float4 Main(PSInput input) : SV_Target
{
    float4 FragAlbedo = AlbedoFactor;
    float Alpha = 1.0f;
    float3 Normal = normalize(input.Normal);
    float Metallic = AORoughnessMetallicEmptyFactors.x;
    float Roughness = AORoughnessMetallicEmptyFactors.y;
    float3 Emissive = EmissiveFactor;
    float AO = AORoughnessMetallicEmptyFactors.z;

    if (AlbedoNormalEmissiveDescriptor[ALBEDO_KEY] == 1)
    {
        FragAlbedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord);
        Alpha = UseAlpha ? FragAlbedo.a : 1.0f;
        if (Alpha < 0.5)
            discard;
    }
    
    float3 Color = FragAlbedo.xyz * Alpha;
    
    
    if (AlbedoNormalEmissiveDescriptor[NORMAL_KEY] == 1)
    {
        float3 T = normalize(input.Tangent);
        float3 N = Normal;

        T = normalize(T - dot(T, N) * N);
        float3 B = cross(N, T);

        float3x3 TBN = float3x3(T, B, N);

        float3 NormalTS = NormalTexture.Sample(NormalSampler, input.TexCoord).rgb;
        NormalTS = normalize(NormalTS * 2.0 - 1.0);

        Normal = normalize(mul(NormalTS, TBN));
    }
    
    
    if (AlbedoNormalEmissiveDescriptor[EMMISIVE_KEY] == 1)
    {
        Emissive += EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb;
    }
    
    if (ORMTextureDescriptor[METALLIC_ROUGNESS_AO_ANI_KEY] == 1)
    {
        float3 ORM = ORMTexture.Sample(ORMTextureSampler, input.TexCoord).rgb;

        AO += ORM.r;
        Roughness += ORM.g;
        Metallic += ORM .b;
    }
    
    float keepAlive = 0.0;
    keepAlive += dot(Normal, Normal) * 1e-6;
    keepAlive += Metallic * 1e-6;
    keepAlive += Roughness * 1e-6;
    keepAlive += AO * 1e-6;
    keepAlive += FragAlbedo.x;
    keepAlive += dot(Emissive, float3(1, 1, 1)) * 1e-6;
 
        
    return float4(Color + float3(keepAlive, keepAlive, keepAlive) * 1e-6, Alpha);
}