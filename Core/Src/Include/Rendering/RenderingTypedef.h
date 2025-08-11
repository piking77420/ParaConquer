#pragma once

#include "CoreHeader.hpp"
#include "Material.hpp"
#include "LowRenderer/FrameBuffer.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Resources/Mesh.hpp"

BEGIN_PCCORE
    struct LowLevelCamera
    {
        Tbx::Vector3d position;
        Tbx::Vector3f front;
        Tbx::Vector3f up;

        float aspect; 
        float fov;
        float near;
        float far;
        bool isOrthographic;
    };

#ifdef WITH_EDITOR
    enum RenderingContextFlag
    {
        DebugDrawGeometry = 1 << 0  
    };
    

#endif

    struct RenderingContext
    {
        LowLevelCamera lowLevelCamera;
        float deltaTime;
        float time;
        
        std::shared_ptr<FrameBuffer> gbufferFrameBuffer;
        std::shared_ptr<FrameBuffer> forwardFrameBuffer;
        std::shared_ptr<FrameBuffer> finalImageFrameBuffer;
        
        PC_CORE::ShaderProgramDescriptorSets* geometryDescritproSet;
        PC_CORE::ShaderProgramDescriptorSets* defferdLightingGbufferSet;
        PC_CORE::ShaderProgramDescriptorSets* defferdLightingLightingCameraSet;
        PC_CORE::ShaderProgramDescriptorSets* forwardDesritptorSet;
        PC_CORE::ShaderProgramDescriptorSets* toneMapDescritptorSet;
        PC_CORE::ShaderProgramDescriptorSets* finalImageDescritptorSet;

        Texture2D* hdrImage;

        Tbx::Vector2ui renderingContextSize;
#ifdef WITH_EDITOR
        size_t renderingContextFlag;
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
    

    struct StaticMeshData
    {
        PC_CORE::MaterialType materialType;
        const PC_CORE::ShaderProgramDescriptorSets* descriptorSet;
        const PC_CORE::Mesh* mesh;

        Tbx::Matrix4x4d worldMatrix;
        // TO DO PASS IT TO MAT3
        Tbx::Matrix4x4d normalInvertMatrix;

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
        Tbx::Vector3f color;
        float intensity;
        Tbx::Vector3f direction;
    };

    struct SpotLightData
    {
        Tbx::Vector3f color;
        float intensity;
        Tbx::Vector3f direction;
        float cutoff;
        Tbx::Vector3d position;
        float outerCutOff;
    };

    struct PointLightData
    {
        Tbx::Vector3f color;
        float intensity;
        Tbx::Vector3d position;
    };

    
    // World Data
    struct LightData
    {
        LightType lightType;
        union Data
        {
            DirectionalLightData directionalLight;
            SpotLightData spotLight;
            PointLightData pointLightData;
        }data;
    };

    struct RenderingWorldData
    {
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldData);

        DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldData);

        void Clear()
        {
            staticMeshData.clear();
            lightData.clear();
        }

        std::vector<StaticMeshData> staticMeshData;
        std::vector<LightData> lightData;
    };


#define MAX_DIRLIGHT 1
#define MAX_POINTLIGHT 10
#define MAX_SPOTLIGHT 10

    struct ALIGNAS_16 DirectionalLightGPU
    {
        Tbx::Vector3f direction;
        float intensity;
        Tbx::Vector3f color;
        float _pad;
    };

    struct ALIGNAS_16 SpotLightGPU
    {
        Tbx::Vector3f position;
        float intensity;
        Tbx::Vector3f direction;
        float cutoff;
        Tbx::Vector3f color;
        float outerCutOff;
        float maxRange;
        float _pad[3];
    };

    struct ALIGNAS_16 PointLightGPU
    {
        Tbx::Vector3f position;
        float maxRange;
        Tbx::Vector3f color;
        float intensity;
    };

    struct ALIGNAS_16 GPUDynamicLightData   
    {
        DirectionalLightGPU directionalLights[MAX_DIRLIGHT];
        SpotLightGPU spothLights[MAX_SPOTLIGHT];
        PointLightGPU pointLights[MAX_POINTLIGHT];
        int dirLightCount;
        int spothLightCount;
        int pointLightCount;    
        int _pad;
    };

END_PCCORE