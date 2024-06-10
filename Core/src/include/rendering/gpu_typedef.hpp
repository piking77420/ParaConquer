#pragma once


#include "core_header.hpp"
#include "math/toolbox_typedef.hpp"

static constexpr size_t MAX_MATERIAL_COUNT = 100; 
static constexpr size_t MAX_POINTLIGHT_COUNT = 10; 
static constexpr size_t MAX_SPOTLIGHT_COUNT = 10; 
static constexpr size_t MAX_DIRLIGHT_COUNT = 1; 


struct alignas(16) CameraBuffer
{
    Matrix4x4f view = Matrix4x4f::Identity();
    Matrix4x4f proj = Matrix4x4f::Identity();
};


struct alignas(16) MatrixMeshes
{
    Matrix4x4f model = Matrix4x4f::Identity();
    Matrix4x4f modelNormalMatrix = Matrix4x4f::Identity();
};

struct alignas(16) GpuDirLight
{
    Vector3f direction;
    float _pad0;
    Vector3f color;
    float intensity;
    
};
struct alignas(16) GpuPointLight
{
    Vector3f position;
    float _pad0;
    Vector3f color;
    float intensity;
};
struct alignas(16) GpuSpothLight
{
    Vector3f direction;
    float _pad0;
    Vector3f position;
    float _pad1;
    Vector3f color;
    float intensity;
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
