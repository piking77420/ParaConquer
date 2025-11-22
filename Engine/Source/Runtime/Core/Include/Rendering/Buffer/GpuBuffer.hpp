#pragma once

#include "CoreHeader.hpp"
#include "Rendering/GpuResource.hpp"

BEGIN_PCCORE
    class GpuBuffer : public IGpuResource
    {
    public:
        PC_CORE_API std::shared_ptr<RhiResource> GetRhiHandle() const override = 0;

        DEFAULT_COPY_MOVE_OPERATIONS(GpuBuffer)

        PC_CORE_API GpuBuffer() = default;

        PC_CORE_API ~GpuBuffer() override = default;
    };

END_PCCORE
