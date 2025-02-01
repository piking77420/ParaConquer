#pragma once

#include "core_header.hpp"
#include "low_renderer/gpu_handle.hpp"

#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

struct GpuBuffer
{

    PC_CORE_API GpuBuffer(GpuBuffer&& _other) noexcept
    {
        for (size_t i = 0; i < bufferHandles.size(); ++i)
        {
            bufferHandles[i] = _other.bufferHandles[i];
            _other.bufferHandles[i] = nullptr;
        }
        
    }

    PC_CORE_API GpuBuffer& operator=(GpuBuffer&& _other) noexcept
    {
        for (size_t i = 0; i < bufferHandles.size(); ++i)
        {
            bufferHandles[i] = _other.bufferHandles[i];
            _other.bufferHandles[i] = nullptr;
        }
        
        return *this;
    }
    
    PC_CORE_API GpuBuffer() = default;

    PC_CORE_API virtual ~GpuBuffer();

    std::array<std::shared_ptr<GpuHandle>, MAX_FRAMES_IN_FLIGHT> bufferHandles;
};

END_PCCORE