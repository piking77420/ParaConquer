#pragma once


#include "core_header.hpp"
#include "math/toolbox_typedef.hpp"


struct alignas(16) UniformBufferObject
{
    Matrix4x4f model = Matrix4x4f::Identity();
    Matrix4x4f view = Matrix4x4f::Identity();
    Matrix4x4f proj = Matrix4x4f::Identity();
};

