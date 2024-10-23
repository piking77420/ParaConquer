﻿#pragma once

#include "core_header.hpp"
#include "math/matrix_transformation.hpp"


BEGIN_PCCORE
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
        Tbx::Vector2f renderingContextSize;
        LowLevelCamera lowLevelCamera;
        float deltaTime;
        float time;
    };


END_PCCORE