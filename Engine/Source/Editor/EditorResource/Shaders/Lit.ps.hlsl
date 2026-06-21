
    #include "Func.hlsl"

    struct PSInput
    {
        float4 Position : SV_POSITION;
    #if defined(LIT) || defined(VIEWPOS)
        #if defined(VIEWPOS)
        float3 WorldPosition : TEXCOORD0;
        #elif defined(LIT)
        float3 WorldPosition : TEXCOORD0;
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

    #define IMAGE_BASE_LIGHTING_SPACE space2

    #define IMAGE_BASE_LIGHTING_SPACE 2

    [[vk::combinedImageSampler]]
    [[vk::binding(0, IMAGE_BASE_LIGHTING_SPACE)]]
    TextureCube<float4> IrradianceMap : register(t0, space2);

    [[vk::combinedImageSampler]]
    [[vk::binding(0, IMAGE_BASE_LIGHTING_SPACE)]]
    SamplerState IrradianceMapSampler : register(s0, space2);

    [[vk::combinedImageSampler]]
    [[vk::binding(1, IMAGE_BASE_LIGHTING_SPACE)]]
    TextureCube<float4> PrefilterMap : register(t1, space2);

    [[vk::combinedImageSampler]]
    [[vk::binding(1, IMAGE_BASE_LIGHTING_SPACE)]]
    SamplerState PrefilterMapSampler : register(s1, space2);

    [[vk::combinedImageSampler]]
    [[vk::binding(2, IMAGE_BASE_LIGHTING_SPACE)]]
    Texture2D<float4> BRDFLUTTexture : register(t2, space2);

    [[vk::combinedImageSampler]]
    [[vk::binding(2, IMAGE_BASE_LIGHTING_SPACE)]]
    SamplerState BRDFLUTSampler : register(s2, space2);

    #include "PBR.hlsl"


    float3 PrefilteredReflection(float3 R, float Roughness)
    {
        uint Width;
        uint Height;
        uint MipCount;
        PrefilterMap.GetDimensions(0, Width, Height, MipCount);

        float MaxMip = float(MipCount - 1);
        float LOD = saturate(Roughness) * MaxMip;

        float LODF = floor(LOD);
        float LODC = min(ceil(LOD), MaxMip);

        float3 a = PrefilterMap.SampleLevel(PrefilterMapSampler, R, LODF).rgb;
        float3 b = PrefilterMap.SampleLevel(PrefilterMapSampler, R, LODC).rgb;

        return lerp(a, b, LOD - LODF);
    }

    float3 EvaluateIBL(float3 N, float3 V, float NoV, float3 BaseColor, float Metallic, float PerceptualRoughness, float AO, float3 F0)
    {
        float3 R_W = reflect(-V, N);   
        
        float3 PFR = PrefilteredReflection(R_W, PerceptualRoughness);
        float3 Irradiance = IrradianceMap.Sample(IrradianceMapSampler, N).rgb;
        float2 BRDF = BRDFLUTTexture.Sample(BRDFLUTSampler, float2(saturate(NoV), saturate(PerceptualRoughness))).rg;

        // Specular
        float3 F = F_SchlickR(max(NoV, 0.0), F0, PerceptualRoughness);
        // Component
        // KS => 1
        float3 kD = (1.0 - F) * (1.0 - Metallic);

        // Diffuse
        float3 Diffuse = kD * BaseColor * Irradiance;
        // Specular
        float3 Specular = PFR * (F * BRDF.x + BRDF.y);

        return (Diffuse + Specular) * AO;
    }


    float4 Main(PSInput input) : SV_Target
    {
        float4 FragAlbedo = float4(0, 0, 0, 1);
    #if defined(LIT)

        FragAlbedo.xyz = AlbedoFactor.xyz;

        // Normal
        float3 NormalNormlize = normalize(input.Normal);
        float3 Normal_W = NormalNormlize;
        // 
        float AO = AORoughnessMetallicEmptyFactors.x;
        float Roughness = AORoughnessMetallicEmptyFactors.y;
        float Metallic = AORoughnessMetallicEmptyFactors.z;
        float3 Emissive = EmissiveFactor;

        float3 V_W = normalize(CameraPos.xyz - input.WorldPosition);
    #endif

    #if defined(LIT) && defined(USE_UV)
        if (AlbedoNormalEmissiveDescriptor[ALBEDO_KEY] == 1)
        {
            float4 TexAlbedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord);
            if (TexAlbedo.a < 0.5)
                discard;
            
            TexAlbedo.xyz = SRGBToLinear(TexAlbedo.xyz);
            FragAlbedo = AlbedoFactor * TexAlbedo;
        }
    #endif   

    #if defined(LIT) && defined(USE_UV) && defined(USE_NORMAL_MAP) || (defined(USE_UV) && defined(USE_NORMAL_MAP) && defined(NORMALW))
        if (AlbedoNormalEmissiveDescriptor[NORMAL_KEY] == 1 || false)
        {
            float3 T = normalize(input.Tangent.xyz);
            float3 N = Normal_W;
            T = normalize(T - dot(T, N) * N);
            float tangentSign = input.Tangent.w;

            float3 B = normalize(cross(N, T)) * tangentSign;

            float3 NormalTS = NormalTexture.Sample(NormalSampler, input.TexCoord).rgb; // TODO be careful with BC textures
            NormalTS = NormalTS * 2.0 - 1.0; // 0...1 to -1 ... 1
            float3x3 TBN = float3x3(
                T.x, B.x, N.x,
                T.y, B.y, N.y,
                T.z, B.z, N.z
            );
            Normal_W = normalize(mul(TBN, NormalTS));    
        }
        
    #endif
        
        
    #if defined(LIT) && defined(USE_UV)
        if (AlbedoNormalEmissiveDescriptor[EMMISIVE_KEY] == 1)
        {
            Emissive *= SRGBToLinear(EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb);
        }
    #endif
        

        float3 Lo = float3(0, 0, 0);
    #if defined(LIT) && defined(USE_UV)
        if (AlbedoNormalEmissiveDescriptor[AO_ROUGNESS_METALLIC_KEY] == 1)
        {
            float3 ORM = ORMTexture.Sample(ORMTextureSampler, input.TexCoord).rgb;

            //AO = ORM.r * AORoughnessMetallicEmptyFactors.x;
            Roughness = ORM.g * AORoughnessMetallicEmptyFactors.y;
            Metallic = ORM.b * AORoughnessMetallicEmptyFactors.z;
        }
    #endif

    #if defined(LIT) && defined(USE_UV)
        if (AoEmptyEmptyEmptyDescriptor[AO_KEY] == 1)
        {
            float AoSampled = AoTexture.Sample(AoTextureSampler, input.TexCoord).r;
            AO = AORoughnessMetallicEmptyFactors.x * AoSampled;
        }
    #endif 

    #if defined(LIT)
        float PerceptualRoughness  = Roughness; // remap PerceptualRoughness toRoughness ;
        float AlphaRoughness = max(PerceptualRoughness * PerceptualRoughness, 0.045); // 0..1

        float3 BaseColor = FragAlbedo.xyz;
        float3 DiffuseColor = BaseColor * (float3(1, 1, 1) - float3(DIELECTRIC_F0));
        DiffuseColor *= 1.f - Metallic;

        float3 SpecularColor = lerp(DIELECTRIC_F0, BaseColor, Metallic);
        float Reflectance = max(max(SpecularColor.r, SpecularColor.g), SpecularColor.b);

        float3 F0 = SpecularColor;
	    float3 F90 = clamp(Reflectance * 25.0, 0.0, 1.0);
        float3 SpecularEnvironmentR90 = float3(1.0, 1.0, 1.0) * F90;

        // Normal Computing
        float3 N = Normal_W;
        float NoV = saturate(dot(N, V_W));

        // Mix it into output in a way that cant be optimized away
        float keepAlive = Lights[0].PositionType.x;
        Lo.r += (asuint(keepAlive) & 1) * 1e-6;
    
        // Dir Light 
        {
            float3 L = normalize(-DirLightW.Direction);
            float NoL = saturate(dot(N, L));
            if (NoL > 0.0)
            {
                float3 H = normalize(V_W + L);
                float NoH = saturate(dot(N, H));
                float LoH = saturate(dot(L, H));

                float3 Radiance = DirLightW.ColorIntensity.xyz * DirLightW.ColorIntensity.w;
            
                float3 Brdf = BRDF(BaseColor, Metallic, AlphaRoughness, NoV, NoL, NoH, LoH, F0, F90);
                Lo += Brdf * Radiance * NoL;
            }
        }

        // Ambiant
        float3 Ambient = EvaluateIBL(Normal_W, V_W, NoV, BaseColor, Metallic, PerceptualRoughness, AO, F0);

        
        // Other
        Lo += Emissive;
        Lo += Ambient;

    #endif

    #if defined(USE_COLOR)
        Lo += input.Color;
    #endif
            
        return float4(Lo, FragAlbedo.a);
    }