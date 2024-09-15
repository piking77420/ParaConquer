#pragma once

#include "core_header.hpp"
#include "math/matrix_transformation.hpp"


BEGIN_PCCORE
    struct LowLevelCamera
    {
        Tbx::Vector3f position;
        Tbx::Vector3f front;
        Tbx::Vector3f up;
    
        bool isOrthographic;
    };

    struct RenderingContext
    {
        Tbx::Vector2f screenSize;
        LowLevelCamera lowLevelCamera;
        
    };


END_PCCORE