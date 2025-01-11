#pragma once

#include "math/matrix_transformation.hpp"

BEGIN_PCCORE


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
        // Need this to change the final aspect Image either shader read optimal for viewPort or swaipchain image present optimal
        bool isSwapChainImage;
        
        //FrameBufferHandle frameBufferHandle;
        Tbx::Vector2f renderingContextSize;
    };




END_PCCORE