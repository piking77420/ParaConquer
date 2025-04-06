#pragma once


#include "low_renderer/gpu_handle.hpp"

BEGIN_PCCORE

struct GpuBuffer
{
public:
    PC_CORE_API GpuBuffer(GpuBuffer&& _other) noexcept;

    PC_CORE_API GpuBuffer& operator=(GpuBuffer&& _other) noexcept;
    
    PC_CORE_API GpuBuffer() = default;

    PC_CORE_API virtual ~GpuBuffer();

    std::array<std::shared_ptr<GpuHandle>, MAX_FRAMES_IN_FLIGHT> bufferHandles;
};

END_PCCORE