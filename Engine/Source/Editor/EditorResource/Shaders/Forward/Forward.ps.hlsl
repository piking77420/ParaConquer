struct PSInput
{
    float4 Position : SV_POSITION;
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD1;
};

/*
#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"*/

#define LIGHT_BUFFER_BINDING t1
#define LIGHT_BUFFER_SPACE space0

#define LIGHT_HEADER_BUFFER_BINDING b2
#define LIGHT_HEADER_BUFFER_SPACE space0
#include "Light.hlsl"

#define MATERIAL_SET space1
#include "Material.hlsl"


#include "PBR.hlsl"


float4 Main(PSInput input) : SV_Target
{
    float4 FragAlbedo = AlbedoFactor;
    float3 Normal = normalize(input.Normal);
    float Metallic = AORoughnessMetallicEmptyFactors.x;
    float Roughness = AORoughnessMetallicEmptyFactors.y;
    float3 Emissive = EmissiveFactor;
    float AO = AORoughnessMetallicEmptyFactors.z;

    if (AlbedoNormalEmissiveDescriptor[ALBEDO_KEY] == 1)
    {
        FragAlbedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord);
        if (FragAlbedo.a < 0.5)
            discard;
    }
        
    if (AlbedoNormalEmissiveDescriptor[NORMAL_KEY] == 1)
    {
        float3 T = normalize(input.Tangent);
        float3 N = Normal;

        T = normalize(T - dot(T, N) * N);
        float3 B = normalize(cross(N, T));

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

        AO = ORM.r;
        Roughness = ORM.g;
        Metallic = ORM .b;
    }
    
    float3 Lo = float3(0, 0, 0);
    
    Roughness = Roughness * Roughness; // map to perspectual Rougness
    float3 N = Normal;
    float3 V = -normalize(input.ViewSpacePosition);
    float NoV = saturate(dot(N, V)) + 1e-5;
    
    float keepAlive = Lights[0].PositionType.x;

    // Mix it into output in a way that can’t be optimized away
    Lo.r += (asuint(keepAlive) & 1) * 1e-6;
 
    // Dir Light 
    {
        float3 L = normalize(DirLight.Direction);
        float NoL = saturate(dot(N, L));
        if (NoL > 0.0)
        {
            float3 H = normalize(V + L);
            float NoH = saturate(dot(N, H));
            float LoH = saturate(dot(L, H));
                    
            float3 Radiance = DirLight.ColorIntensity.xyz * DirLight.ColorIntensity.w;
            float3 DiffuseColor = (1.0 - Metallic) * FragAlbedo.xyz;
        
            float3 Brdf = BRDF(DiffuseColor, NoV, NoL, NoH, LoH, Roughness);
            Lo += Brdf * Radiance * NoL;
        }
    }
    
    Lo += Emissive;
    

        
    return float4(Lo, FragAlbedo.a);
}