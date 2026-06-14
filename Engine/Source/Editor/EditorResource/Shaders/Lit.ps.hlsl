
#include "Func.hlsl"

struct PSInput
{
    float4 Position : SV_POSITION;
#if defined(LIT) || defined(VIEWPOS)
    #if defined(VIEWPOS)
    float3 ViewSpacePosition : TEXCOORD0;
    #elif defined(LIT)
    float3 ViewSpacePosition : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Tangent : TEXCOORD2;
    #endif
#endif

#if defined(USE_UV)
    float2 TexCoord : TEXCOORD3;
#endif

#if defined(USE_COLOR)
    nointerpolation float3 Color : COLOR0;
#endif
};

#define CAMERA_BINDING b0
#define CAMERA_SET space0
#include "Camera.hlsl"

#define LIGHT_BUFFER_BINDING t2
#define LIGHT_BUFFER_SPACE space0

#define LIGHT_HEADER_BUFFER_BINDING b3
#define LIGHT_HEADER_BUFFER_SPACE space0
#include "Light.hlsl"

#define MATERIAL_SET space1
#include "Material.hlsl"

#define IMAGE_BASE_LIGHTING_SPACE space3

[[vk::combinedImageSampler]]
TextureCube<float4> IrradianceMap : register(t0, IMAGE_BASE_LIGHTING_SPACE);
[[vk::combinedImageSampler]]
SamplerState IrradianceMapSampler : register(s0, IMAGE_BASE_LIGHTING_SPACE); 

[[vk::combinedImageSampler]]
TextureCube<float4> PrefilterMap : register(t1, IMAGE_BASE_LIGHTING_SPACE);
[[vk::combinedImageSampler]]
SamplerState PrefilterMapSampler : register(s1, IMAGE_BASE_LIGHTING_SPACE); 

[[vk::combinedImageSampler]]
Texture2D<float4> BRDFLUTTexture : register(t2, IMAGE_BASE_LIGHTING_SPACE);
[[vk::combinedImageSampler]]
SamplerState BRDFLUTSampler : register(s2, IMAGE_BASE_LIGHTING_SPACE); 


#include "PBR.hlsl"


float3 PrefilteredReflection(float3 R, float PerceptualRoughness)
{
    uint Width;
    uint Height;
    uint MipCount;
    PrefilterMap.GetDimensions(0, Width, Height, MipCount);

    float LOD = PerceptualRoughness * MipCount;
    float LODF = floor(LOD);
	float LODC = ceil(LOD);
    float3 a = PrefilterMap.SampleLevel(PrefilterMapSampler, R, LODF).rgb;
	float3 b = PrefilterMap.SampleLevel(PrefilterMapSampler, R, LODC).rgb;
    return lerp(a, b, LOD - LODF);
}

float3 EvaluateIBL(float3 N, float3 V, float NoV, float3 DiffuseColor, float PerceptualRoughness, float3 F0)
{
    float3 R_W = reflect(-V, N);   

    // Reflection
    float3 PFR = PrefilteredReflection(R_W, PerceptualRoughness);

    // Diffuse
    float3 Irradiance = IrradianceMap.Sample(IrradianceMapSampler, N).rgb;
    float3 Diffuse = DiffuseColor * Irradiance;

    // Specular
    float2 BRDF = BRDFLUTTexture.Sample(BRDFLUTSampler, float2(saturate(NoV), saturate(PerceptualRoughness))).rg;
    float3 F = F_SchlickR(max(NoV, 0.0), F0, PerceptualRoughness);
	float3 Specular = PFR * (F * BRDF.x + BRDF.y);

    // Component
    // KS => 1
    float3 kD = 1.0 - F;

    return (kD * Diffuse) + Specular;
}


float4 Main(PSInput input) : SV_Target
{
    float4 FragAlbedo = float4(0, 0, 0, 1);
#if defined(LIT)

    FragAlbedo.xyz = AlbedoFactor.xyz;

    // Normal
    float3 NormalNormlize = normalize(input.Normal);
    float3 Normal_V = NormalNormlize;
    // 
    float AO = AORoughnessMetallicEmptyFactors.x;
    float PerceptualRoughness = AORoughnessMetallicEmptyFactors.y;
    float Metallic = AORoughnessMetallicEmptyFactors.z;
    float3 Emissive = EmissiveFactor;

    float3 V = -normalize(input.ViewSpacePosition);
    float3 V_W = mul((float3x3)ViewInv, V);
#endif

#if defined(LIT) && defined(USE_UV)
    if (AlbedoNormalEmissiveDescriptor[ALBEDO_KEY] == 1)
    {
        FragAlbedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord);
        if (FragAlbedo.a < 0.5)
            discard;
        
        FragAlbedo.xyz = SRGBToLinear(FragAlbedo.xyz);
    }
#endif   

#if defined(LIT) && defined(USE_UV) && defined(USE_NORMAL_MAP)
    if (AlbedoNormalEmissiveDescriptor[NORMAL_KEY] == 1)
    {
        float3 T = normalize(input.Tangent.xyz);
        float3 N = Normal_V;
        T = normalize(T - dot(T, N) * N);
        float tangentSign = input.Tangent.w;

        float3 B = normalize(cross(N, T)) * tangentSign;;

        float3 NormalTS = NormalTexture.Sample(NormalSampler, input.TexCoord).rgb; // TODO be careful with BC textures
        NormalTS = NormalTS * 2.0 - 1.0; // 0...1 to -1 ... 1
        float3x3 TBN = float3x3(
            T.x, B.x, N.x,
            T.y, B.y, N.y,
            T.z, B.z, N.z
        );
        Normal_V = normalize(mul(TBN, NormalTS));    
    }
#endif
    
    
#if defined(LIT) && defined(USE_UV)
    if (AlbedoNormalEmissiveDescriptor[EMMISIVE_KEY] == 1)
    {
        Emissive *= SRGBToLinear(EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb);
    }
#endif
    
#if defined(LIT) && defined(USE_UV)
    if (ORMTextureDescriptor[METALLIC_ROUGNESS_AO_ANI_KEY] == 1)
    {
        float3 ORM = ORMTexture.Sample(ORMTextureSampler, input.TexCoord).rgb;

        AO = ORM.r;
        PerceptualRoughness = ORM.g;
        Metallic = ORM.b;
    }
#endif

    float3 Lo = float3(0, 0, 0);
#if defined(LIT)


    float Roughness = PerceptualRoughness; // remap PerceptualRoughness toRoughness ;
    Roughness = saturate(Roughness); // 0..1
    float3 BaseColor = FragAlbedo.xyz;
    float3 F0 = lerp(DIELECTRIC_F0, BaseColor, Metallic);

    // Normal Computing
    float3 N = Normal_V;
    float NoV = max(saturate(dot(N, V)), 1e-5);;
    float3 Normal_W = mul((float3x3)ViewInv, Normal_V);

    // Mix it into output in a way that can�t be optimized away
    float keepAlive = Lights[0].PositionType.x;
    Lo.r += (asuint(keepAlive) & 1) * 1e-6;
 
    // Dir Light 
    {
        float3 L = normalize(-DirLight.Direction);
        float NoL = saturate(dot(N, L));
        if (NoL > 0.0)
        {
            float3 H = normalize(V + L);
            float NoH = saturate(dot(N, H));
            float LoH = saturate(dot(L, H));

            float3 Radiance = SRGBToLinear(DirLight.ColorIntensity.xyz) * DirLight.ColorIntensity.w;
        
            float3 Brdf = BRDF(BaseColor, Metallic, Roughness, NoV, NoL, NoH, LoH, F0);
            Lo += Brdf * Radiance * NoL;
        }
    }

    // Ambiant
    float3 DiffuseIBLColorIBL = FragAlbedo.xyz * (1.0 - Metallic);
    float NoV_W = max(dot(Normal_W, V_W), 1e-5);
    float3 Ambient = EvaluateIBL(Normal_W, V_W, NoV_W, DiffuseIBLColorIBL, PerceptualRoughness, F0) * AO;

    
    // Other
    Lo += Emissive;
    Lo += Ambient;
#endif

#if defined(USE_COLOR)
    Lo += input.Color;
#endif
        
    return float4(Lo, FragAlbedo.a);
}