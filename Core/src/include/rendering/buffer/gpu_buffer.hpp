#pragma once

#include "core_header.hpp"
#include "rendering/gpu_resource.hpp"

BEGIN_PCCORE
class GpuBuffer : public IGpuResource
{
public:

    PC_CORE_API std::shared_ptr<RhiResource> GetRhiHandle() const = 0;

    DEFAULT_COPY_MOVE_OPERATIONS(GpuBuffer)
    
    PC_CORE_API GpuBuffer() = default;
    
    PC_CORE_API ~GpuBuffer() override = default;
protected:

};

END_PCCORE