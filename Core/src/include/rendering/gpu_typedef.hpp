#pragma once


#include "core_header.hpp"
#include <math/toolbox_typedef.hpp>

static constexpr size_t MAX_MATERIAL_COUNT = 100; 
static constexpr size_t MAX_POINTLIGHT_COUNT = 10; 
static constexpr size_t MAX_SPOTLIGHT_COUNT = 10; 
static constexpr size_t MAX_DIRLIGHT_COUNT = 1;


enum RenderPass
{
    COLORONLY,
    FORWARD,
    GBUFFER,
    COUNT
};


struct alignas(16) CameraBuffer
{
    Tbx::Matrix4x4f view = Tbx::Matrix4x4f::Identity();
    Tbx::Matrix4x4f proj = Tbx::Matrix4x4f::Identity();
    Tbx::Vector3f cameraPos = {};
};


struct alignas(16) MatrixMeshes
{
    Tbx::Matrix4x4f model = Tbx::Matrix4x4f::Identity();
    Tbx::Matrix4x4f modelNormalMatrix = Tbx::Matrix4x4f::Identity();
};

struct alignas(16) GpuDirLight
{
    Tbx::Vector3f direction;
    float _pad0;
    Tbx::Vector3f color;
    float intensity;

};
struct alignas(16) GpuPointLight
{
    Tbx::Vector3f position;
    float _pad0;
    Tbx::Vector3f color;
    float intensity;
};
struct alignas(16) GpuSpothLight
{
    Tbx::Vector3f direction;
    float _pad0;
    Tbx::Vector3f position;
    float _pad1;
    Tbx::Vector3f color;
    float intensity;
};

struct alignas(16) GPUMaterial
{
    Tbx::Vector4f albedo;
    Tbx::Vector4f metallicSpecularRoughessAnisotropy;
    Tbx::Vector4f emmisiveColorAo;
};


struct alignas(16) GpuLight
{
    int32_t nbrOfDirLight = -1;
    int32_t nbrOfPointLight = -1;
    int32_t nbrOfSpotLight = -1;
    int32_t _pad0;

    GpuDirLight gpuDirLights[MAX_DIRLIGHT_COUNT];
    GpuPointLight gpuPointLights[MAX_POINTLIGHT_COUNT];
    GpuSpothLight gpuSpotLight[MAX_SPOTLIGHT_COUNT];
};
