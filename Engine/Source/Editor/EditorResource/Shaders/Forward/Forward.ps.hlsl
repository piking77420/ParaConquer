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
    
    float FragMetallic = TextureDescriptor[1] == 1 ? MetallicTexture.Sample(MetallicSampler, input.TexCoord).r : MetallicRoughnessAnisotropy.r;
    float FragRougness = TextureDescriptor[2] == 1 ? RoughnessTexture.Sample(RoughnessSampler, input.TexCoord).r : MetallicRoughnessAnisotropy.g;
        
    float3 FragNormal = TextureDescriptor[3] == 1 ? NormalTexture.Sample(NormalSampler, input.TexCoord).rgb : float3(float3(0, 0, 1));
    float3 FragEmmisive = TextureDescriptor2[0] == 1 ? EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb : float3(Emissive);
    float FragAO = TextureDescriptor2[1] == 1 ? AOTexture.Sample(AOSampler, input.TexCoord).r : AO;


    return float4(FragAlbedo + float3((FragMetallic * 0.001), (FragRougness * 0.001), 0) + (FragNormal * 0.001) + (FragEmmisive * 0.001) + float3(FragAO, FragAO, FragAO) * 0.001, 1);
}