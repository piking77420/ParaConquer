#pragma once

#include <Memory>

#include "CoreHeader.hpp"

#include "LowRenderer/RhiResource.hpp"


BEGIN_PCCORE
    class IGpuResource
    {
    public:
        PC_CORE_API virtual std::shared_ptr<RhiResource> GetRhiHandle() const = 0;

        DEFAULT_COPY_MOVE_OPERATIONS(IGpuResource)

        PC_CORE_API IGpuResource() = default;

        PC_CORE_API virtual ~IGpuResource() = default;
    };

END_PCCORE
