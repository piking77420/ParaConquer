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
    
    Vector3f front = Vector3f::UnitY();
    Vector3f up = Vector3f::UnitZ();

    // tmpr
    Vector3f position = {2.0f, 2.0f, 2.0};
};

END_PCCORE