#pragma once

#include "render_harware_interface/rhi_typedef.h"
#include "math/matrix_transformation.hpp"

BEGIN_PCCORE

enum class GraphicAPI
{
    NONE,
    VULKAN,
    DX3D12,
    COUNT
};

enum class GBufferType
{
    COLOR,
    POSITION,
    NORMAL,
    TEXTCOORD,
    DEPTH,
    
    COUT,
};



    struct LowLevelCamera
    {
        Tbx::Vector3f position;
        Tbx::Vector3f front;
        Tbx::Vector3f up;

        float aspect; 
        float fov;
        float near;
        float far;
        bool isOrthographic;
    };

    struct RenderingContext
    {
        LowLevelCamera lowLevelCamera;
        float deltaTime;
        float time;
        
        FrameBufferHandle frameBufferHandle;
        Tbx::Vector2f renderingContextSize;
    };




END_PCCORE