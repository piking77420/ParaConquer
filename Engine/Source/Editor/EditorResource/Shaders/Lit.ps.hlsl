
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
TextureCube<float4> IrradianceDiffuse : register(t0, IMAGE_BASE_LIGHTING_SPACE);
[[vk::combinedImageSampler]]
SamplerState IrradianceDiffuseSampler : register(s0, IMAGE_BASE_LIGHTING_SPACE); 

/*
[[vk::combinedImageSampler]]
TextureCube<float4> PrefilterMap : register(t0, IMAGE_BASE_LIGHTING_SPACE);
[[vk::combinedImageSampler]]
SamplerState PrefilterMapSampler : register(s0, IMAGE_BASE_LIGHTING_SPACE); 
*/

#include "PBR.hlsl"


float3 EvaluateIBL(float3 N, float3 DiffuseColor)
{
   // float NoV = max(dot(N, V), 0.0);
    //float3 r = reflect(-V, N); // V could be ViewSpacePosition but whatever

    //vec3 indirectSpecular = evaluateSpecularIBL(r, perceptualRoughness);
    //vec2 env = prefilteredDFG_LUT(perceptualRoughness, NoV);
    //vec3 specularColor = f0 * env.x + f90 * env.y;

    float3 irradiance = IrradianceDiffuse.Sample(IrradianceDiffuseSampler, N).rgb;
    float3 indirectDiffuse = max(irradiance, 0.0);

    return DiffuseColor * indirectDiffuse;
}


float4 Main(PSInput input) : SV_Target
{
    float4 FragAlbedo = float4(0, 0, 0, 1);
#if defined(LIT)

    FragAlbedo.xyz = AlbedoFactor.xyz;

    // Normal
    float3 NormalNormlize = normalize(input.Normal);
    float3 Normal_V = NormalNormlize;
    float3 Normal_W = mul((float3x3)ViewInv, NormalNormlize);
    // 
    float Metallic = AORoughnessMetallicEmptyFactors.x;
    float PerceptualRoughness = AORoughnessMetallicEmptyFactors.y;
    float3 Emissive = EmissiveFactor;
    float AO = AORoughnessMetallicEmptyFactors.z;
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
    float Roughness = PerceptualRoughness * PerceptualRoughness; // remap PerceptualRoughness toRoughness ;
    Roughness = saturate(Roughness); // 0..1
    float3 BaseColor = FragAlbedo.xyz;

    float3 N = Normal_V;
    float3 V = -normalize(input.ViewSpacePosition);
    float NoV = saturate(dot(N, V)) + 1e-5;
    
    float keepAlive = Lights[0].PositionType.x;

    // Mix it into output in a way that can�t be optimized away
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
        
            float3 Brdf = BRDF(BaseColor, Metallic, Roughness,  NoV, NoL, NoH, LoH);
            Lo += Brdf * Radiance * NoL;
        }
    }

    // Ambiant
    float3 DiffuseIBLColor = FragAlbedo.xyz * (1.0 - Metallic);
    float3 Ambient = EvaluateIBL(Normal_W, DiffuseIBLColor) * AO;

    
    // Other
    Lo += Emissive * 0.001;
    Lo += Ambient;
#endif

#if defined(USE_COLOR)
    Lo += input.Color;
#endif
        
    return float4(Lo, FragAlbedo.a);
}