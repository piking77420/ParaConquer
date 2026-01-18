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
   
    float3 FragAlbedo = TextureDescriptor[0] == 1 ? AlbedoTexture.Sample(AlbedoSampler, input.TexCoord).rgb : Albedo.rgb;
    
    float3 FragMaterialRougness = TextureDescriptor[1] == 1 ? MetallicRouhnessTexture.Sample(MetallicRouhnessSampler, input.TexCoord).rgb : float3(MetallicRoughnessAnisotropy.rgb);
    float3 FragNormal = TextureDescriptor[2] == 1 ? NormalTexture.Sample(NormalSampler, input.TexCoord).rgb : float3(float3(0, 0, 1));
    float3 FragEmmisive = TextureDescriptor[3] == 1 ? EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb : float3(Emissive);
    float FragAO = TextureDescriptor2[0] == 1 ? AOTexture.Sample(AOSampler, input.TexCoord).r : AO;


    return float4(FragAlbedo + (FragMaterialRougness * 0.001) + (FragNormal * 0.001) + (FragEmmisive * 0.001) + float3(FragAO, FragAO, FragAO) * 0.001, 1);
}