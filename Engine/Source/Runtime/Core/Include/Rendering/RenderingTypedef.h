#pragma once

#include "CoreHeader.hpp"
#include "Material.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Resources/StaticMesh.hpp"

BEGIN_PCCORE
    struct LowLevelCamera
    {
        Tbx::Vector3d Position;
        Tbx::Vector3f Front;
        Tbx::Vector3f Up;

        float Aspect;
        float Fov;
        float Near;
        float Far;
        bool IsOrthographic;
    };

#ifdef WITH_EDITOR
    enum RenderingContextFlag
    {
        DebugDrawGeometry = 1 << 0
    };


#endif

    struct RenderingContext
    {
        LowLevelCamera LowLevelCamera;
        float DeltaTime;
        float Time;

        std::shared_ptr<RhiFrameBuffer> GbufferFrameBuffer;
        std::shared_ptr<RhiFrameBuffer> ForwardFrameBuffer;
        std::shared_ptr<RhiFrameBuffer> FinalImageFrameBuffer;

        ShaderProgramDescriptorSets* GeometryDescritproSet;
        ShaderProgramDescriptorSets* DefferdLightingGbufferSet;
        ShaderProgramDescriptorSets* DefferdLightingLightingCameraSet;
        ShaderProgramDescriptorSets* ForwardDesritptorSet;
        ShaderProgramDescriptorSets* ToneMapDescritptorSet;
        ShaderProgramDescriptorSets* FinalImageDescritptorSet;

        RhiTexture* HdrImage;

        Tbx::Vector2ui RenderingContextSize;
#ifdef WITH_EDITOR
        size_t RenderingContextFlag;
#endif

        float gamma;
        float exposure;
    };

    enum struct GbufferType : std::uint8_t
    {
        Albedo,
        Normal,
        RoughnessMetallicAo,
        WorldPosition,

        Count
    };

    inline std::string GbufferTypeToString(GbufferType _gbufferType)
    {
        switch (_gbufferType) 
        {
        case GbufferType::Albedo:
            return "Albedo";
        case GbufferType::Normal:
            return "Normal";
        case GbufferType::RoughnessMetallicAo:
            return "RoughnessMetallicAo";
        case GbufferType::WorldPosition:
            return "WorldPosition";
        case GbufferType::Count:
            return "";
        }
        return "";
    }


    struct StaticMeshComponentData
    {
        MaterialType MaterialType;
        const ShaderProgramDescriptorSets* DescriptorSet;
        const StaticMesh* StaticMesh;

        Tbx::Matrix4x4d WorldMatrix;
        // TO DO PASS IT TO MAT3
        Tbx::Matrix4x4d NormalInvertMatrix;
    };


    enum class LightType : uint8_t
    {
        Directional,
        Spotlight,
        Point,
        Area,
        Count,
    };

    struct DirectionalLightData
    {
        Tbx::Vector3f Color;
        float Intensity;
        Tbx::Vector3f Direction;
    };

    struct SpotLightData
    {
        Tbx::Vector3f Color;
        float Intensity;
        Tbx::Vector3f Direction;
        float Cutoff;
        Tbx::Vector3d Position;
        float OuterCutOff;
    };

    struct PointLightData
    {
        Tbx::Vector3f Color;
        float Intensity;
        Tbx::Vector3d Position;
    };


    // World Data
    struct LightData
    {
        LightType LightType;

        union Data
        {
            DirectionalLightData DirectionalLight;
            SpotLightData SpotLight;
            PointLightData PointLightData;
        } Data;
    };

    struct RenderingWorldData
    {
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldData);

        DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldData);

        void Clear()
        {
            StaticMeshComponentData.clear();
            LightData.clear();
        }

        std::vector<StaticMeshComponentData> StaticMeshComponentData;
        std::vector<LightData> LightData;
    };


#define MAX_DIRLIGHT 1
#define MAX_POINTLIGHT 10
#define MAX_SPOTLIGHT 10

    struct ALIGNAS_16 DirectionalLightGpu
    {
        Tbx::Vector3f Direction;
        float Intensity;
        Tbx::Vector3f Color;
        float Pad;
    };

    struct ALIGNAS_16 SpotLightGpu
    {
        Tbx::Vector3f Position;
        float Intensity;
        Tbx::Vector3f Direction;
        float Cutoff;
        Tbx::Vector3f Color;
        float OuterCutOff;
        float MaxRange;
        float Pad[3];
    };

    struct ALIGNAS_16 PointLightGpu
    {
        Tbx::Vector3f Position;
        float MaxRange;
        Tbx::Vector3f Color;
        float Intensity;
    };

    struct ALIGNAS_16 GPUDynamicLightData
    {
        DirectionalLightGpu DirectionalLights[MAX_DIRLIGHT];
        SpotLightGpu SpothLights[MAX_SPOTLIGHT];
        PointLightGpu PointLights[MAX_POINTLIGHT];
        int DirLightCount;
        int SpothLightCount;
        int PointLightCount;
        int Pad;
    };

END_PCCORE
