
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
        uint Width, Height, MipCount;
        PrefilterMap.GetDimensions(0, Width, Height, MipCount);

        float MaxMip = float(MipCount - 1);
        float LOD = saturate(Roughness) * MaxMip;

        return PrefilterMap.SampleLevel(PrefilterMapSampler, normalize(R), LOD).rgb;

    }

    float3 EvaluateIBL(float3 N, float3 V, float NoV, float3 DiffuseColor, float Metallic, float PerceptualRoughness, float3 F0, float3 F90, float A0)
    {
        float3 R_W = reflect(-V, N);   
        
        // Specular indirect
        float3 IndirectSpecular = PrefilteredReflection(R_W, PerceptualRoughness);
        float2 Env  = BRDFLUTTexture.Sample(BRDFLUTSampler, float2(saturate(NoV), saturate(PerceptualRoughness))).rg;
        float3 SpecularColor = F0 * Env.x + F90 * Env.y;

        // Diffuse indirect
        float3 IndirectDiffuse = IrradianceMap.Sample(IrradianceMapSampler, N).rgb;
        float3 F = F_SchlickR(max(NoV, 0.0), F0, PerceptualRoughness);
        float3 kD = (1.0 - F) * (1.0 - Metallic);

        float3 DiffuseIbl = DiffuseColor * IndirectDiffuse;
        float3 SpecularIbl = IndirectSpecular * SpecularColor;

        return (DiffuseIbl * A0) + SpecularIbl;
    }


    float4 GetAlbedo(PSInput input)
    {
        float4 FragAlbedo = float4(0, 0, 0, 1);
#if defined(LIT) && defined(USE_UV)
        FragAlbedo.xyz = AlbedoFactor.xyz;

        if (AlbedoNormalEmissiveDescriptor[ALBEDO_KEY] == 1)
        {
            float4 TexAlbedo = AlbedoTexture.Sample(AlbedoSampler, input.TexCoord);
            if (TexAlbedo.a < 0.5)
                discard;
            
            TexAlbedo.xyz = SRGBToLinear(TexAlbedo.xyz);
            FragAlbedo = AlbedoFactor * TexAlbedo;
        }
#endif

        return FragAlbedo;
    }

    float3 GetNormal(PSInput input)
    {
        float3 N = float3(0, 0, 0);
#if defined(LIT) && defined(USE_UV)
        N = normalize(input.Normal); 

#if defined(USE_NORMAL_MAP)
        if (AlbedoNormalEmissiveDescriptor[NORMAL_KEY] == 1)
        {            
            float3 T = normalize(input.Tangent.xyz);
            float3 B = normalize(cross(N, T)) * input.Tangent.w;
            float3x3 TBN = float3x3(
                T.x, B.x, N.x,
                T.y, B.y, N.y,
                T.z, B.z, N.z
            );
            float3 NormalTS = NormalTexture.Sample(NormalSampler, input.TexCoord).rgb; // TODO be careful with BC textures
            NormalTS = NormalTS * 2.0 - 1.0;
            N = normalize(mul(TBN, NormalTS));
        }
#endif

#endif
        return N;
    }

    float3 GetEmmisive(PSInput input)
    {
        float3 Emissive = float3(0, 0, 0);
#if defined(LIT) && defined(USE_UV)
        Emissive = EmissiveFactor;

        if (AlbedoNormalEmissiveDescriptor[EMMISIVE_KEY] == 1)
        {
            Emissive *= SRGBToLinear(EmissiveTexture.Sample(EmmissiveSampler, input.TexCoord).rgb);
        }
#endif
        return Emissive;
    }

    float3 GetORM(PSInput input)
    {
        float3 ORM = float3(0, 0, 0);
#if defined(LIT) && defined(USE_UV)
        ORM = float3(AORoughnessMetallicEmptyFactors.x, AORoughnessMetallicEmptyFactors.y, AORoughnessMetallicEmptyFactors.z); 

        if (AlbedoNormalEmissiveDescriptor[AO_ROUGNESS_METALLIC_KEY] == 1)
        {
            float3 ORMSampled = ORMTexture.Sample(ORMTextureSampler, input.TexCoord).rgb;
            ORM *= ORMSampled;
        }
#endif
        return ORM;
    }

    float GetAO(PSInput input)
    {
        float AoSampled = 1.f;
#if defined(LIT) && defined(USE_UV)
        if (AoEmptyEmptyEmptyDescriptor[AO_KEY] == 1)
        {
            AoSampled = AoTexture.Sample(AoTextureSampler, input.TexCoord).r;
        }
#endif

        return AoSampled;
    }

    float4 Main(PSInput input) : SV_Target
    {
        float4 OutColor = float4(0, 0, 0, 1);
#if defined(LIT)
    float3 Lo = float3(0, 0, 0);

#if defined(LIT) && defined(USE_UV)
    float4 FragAlbedo = GetAlbedo(input);
    float3 Normal_W = GetNormal(input);        
    float3 Emissive = GetEmmisive(input);        
    float3 ORM = GetORM(input);
    float AO = ORM.x * GetAO(input);
    float Roughness = ORM.y;
    float Metallic = ORM.z;
#endif

    float3 V_W = normalize(CameraPos.xyz - input.WorldPosition);
    float PerceptualRoughness = Roughness; 
    float AlphaRoughness = max(PerceptualRoughness * PerceptualRoughness, 0.045); // remap PerceptualRoughess

    float3 BaseColor = FragAlbedo.xyz;
    float3 DiffuseColor = BaseColor * (float3(1, 1, 1) - float3(DIELECTRIC_F0));
    DiffuseColor *= 1.f - Metallic;

    float3 F0 = lerp(DIELECTRIC_F0, BaseColor, Metallic);
    float Reflectance = max(max(F0.r, F0.g), F0.b);
    float3 F90 = clamp(Reflectance * 25.0, 0.0, 1.0);

    // Normal Computing
    float NoV = max(dot(Normal_W, V_W), 1e-4);

    // Mix it into output in a way that cant be optimized away
    float keepAlive = Lights[0].PositionType.x;
    Lo.r += (asuint(keepAlive) & 1) * 1e-6;

    // Dir Light 
    {
        float3 L = normalize(-DirLightW.Direction);
        float NoL = saturate(dot(Normal_W, L));
        if (NoL > 0.0)
        {
            float3 H = normalize(V_W + L);
            float NoH = saturate(dot(Normal_W, H));
            float LoH = saturate(dot(L, H));

            float3 Radiance = DirLightW.ColorIntensity.xyz * DirLightW.ColorIntensity.w;
        
            float3 Brdf = BRDF(BaseColor, Metallic, AlphaRoughness, NoV, NoL, NoH, LoH, F0, F90);
            Lo += Brdf * Radiance * NoL;
        }
    }

    // Ambiant
    float3 Ambient = EvaluateIBL(Normal_W, V_W, NoV, DiffuseColor, Metallic, PerceptualRoughness, F0, F90, AO);

    // Other
    Lo += Emissive;
    Lo += Ambient;

    OutColor = float4(Lo, FragAlbedo.a);
#endif

#if defined(USE_COLOR)
    OutColor.xyz += input.Color;
#endif
            
        return OutColor;
    }