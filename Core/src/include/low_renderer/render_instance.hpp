#pragma once

#include "core_header.hpp"

#include "rhi_typedef.h"

BEGIN_PCCORE
    struct RenderInstanceCreateInfo
    {
        const char* appName;
        bool gpuDebug;
    };

class RenderInstance
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RenderInstance)

    PC_CORE_API RenderInstance() = default;

    PC_CORE_API RenderInstance(const RenderInstanceCreateInfo& _renderInstanceCreateInfo) {};

    PC_CORE_API virtual ~RenderInstance() = default;

};

END_PCCORE