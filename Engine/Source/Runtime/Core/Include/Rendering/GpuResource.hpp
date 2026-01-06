#pragma once

#include <Memory>

#include "CoreHeader.hpp"

#include "LowRenderer/RhiResource.hpp"


BEGIN_PCCORE
    class IGpuResource
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(IGpuResource)

        PC_CORE_API IGpuResource() = default;

        PC_CORE_API virtual ~IGpuResource() = default;

protected:
    };

END_PCCORE
