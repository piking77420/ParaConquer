#pragma once

#include <array>

#include "CoreHeader.hpp"
#include "Material.hpp"
#include "LowRenderer/RhiFrameBuffer.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Resources/StaticMesh.hpp"

#define GPU_ALIGN alignas(16)

namespace PC_CORE::Rendering
{

using mat4 = std::array<float, 16>;
using mat3 = std::array<float, 9>;

using vec4 = std::array<float, 4>;
using vec3 = std::array<float, 3>;
using vec2 = std::array<float, 2>;


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

struct RenderingWorldData
{
    DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldData);

    DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldData);

    void Clear()
    {
        StaticMeshComponentData.clear();
        LightData.clear();
    }

    std::vector<Rendering::StaticMeshComponentData> StaticMeshComponentData;
    std::vector<Rendering::LightData> LightData;
};

}
