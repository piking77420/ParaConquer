#pragma once

#include "CoreHeader.hpp"
#include "RhiTypedef.h"

BEGIN_PCCORE

    class RhiDevice;

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

        PC_CORE_API RenderInstance(const RenderInstanceCreateInfo& _renderInstanceCreateInfo)
        {
        };

        PC_CORE_API virtual ~RenderInstance() = default;
    };

END_PCCORE
