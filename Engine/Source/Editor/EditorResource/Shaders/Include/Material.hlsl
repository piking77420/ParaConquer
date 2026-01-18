#ifndef  MATERIAL_HEADER_HLSL
#define  MATERIAL_HEADER_HLSL



[[vk::combinedImageSampler]]
Texture2D AlbedoTexture : register(t1, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState AlbedoSampler : register(s1, MATERIAL_SET);


[[vk::combinedImageSampler]]
Texture2D MetallicRouhnessTexture : register(t2, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState MetallicRouhnessSampler : register(s2, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D NormalTexture : register(t3, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState NormalSampler : register(s3, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D EmissiveTexture : register(t4, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState EmmissiveSampler : register(s4, MATERIAL_SET);

[[vk::combinedImageSampler]]
Texture2D AOTexture : register(t5, MATERIAL_SET);
[[vk::combinedImageSampler]]
SamplerState AOSampler : register(s5, MATERIAL_SET);


cbuffer DynamicObjectMaterialBuffer : register(b0, MATERIAL_SET)
{
    int4 TextureDescriptor; // bit flag
    int4 TextureDescriptor2;
    float4 Albedo;  
    float4 MetallicRoughnessAnisotropy;
    float3 Emissive;
    float Pad01;
    float AO;
};

#endif // MATERIAL_HEADER_HLSL