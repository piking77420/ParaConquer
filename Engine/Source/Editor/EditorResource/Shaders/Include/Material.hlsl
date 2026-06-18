#ifndef  MATERIAL_HEADER_HLSL
#define  MATERIAL_HEADER_HLSL



[[vk::combinedImageSampler]]
Texture2D AlbedoTexture : register(t1, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState AlbedoSampler : register(s1, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D NormalTexture : register(t2, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState NormalSampler : register(s2, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D EmissiveTexture : register(t3, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState EmmissiveSampler : register(s3, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D ORMTexture : register(t4, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState ORMTextureSampler : register(s4, MATERIAL_SET);


#define ALBEDO_KEY 0
#define NORMAL_KEY 1
#define EMMISIVE_KEY 2
#define AO_ROUGNESS_METALLIC_KEY 3

cbuffer DynamicObjectMaterialBuffer : register(b0, MATERIAL_SET)
{
    int4 AlbedoNormalEmissiveDescriptor; // AlbedoTexture // Metallic // Emmisive // ORM
    float4 AlbedoFactor;  
    float4 AORoughnessMetallicEmptyFactors;
    float3 EmissiveFactor;
    int UseAlpha;
};

#endif // MATERIAL_HEADER_HLSL