#pragma once

#include "core_header.hpp"

BEGIN_PCCORE
    struct RenderInstanceCreateInfo
    {
        const char* appName;
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