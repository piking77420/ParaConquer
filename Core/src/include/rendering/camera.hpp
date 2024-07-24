#pragma once

#include "core_header.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
struct Camera
{
    // in degress
    float fov = 90.f;
    
    float near = 0.1f;
    float far = 1000.f;
    
    Tbx::Vector3f front = Tbx::Vector3f::UnitZ();
    Tbx::Vector3f up = Tbx::Vector3f::UnitY();

    // tmpr
    Tbx::Vector3f position = {2.0f, 2.0f, 2.0};
};

END_PCCORE