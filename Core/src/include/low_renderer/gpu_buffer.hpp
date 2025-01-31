#pragma once

#include "core_header.hpp"
#include "low_renderer/gpu_handle.hpp"

#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

struct GpuBuffer
{

    GpuBuffer(GpuBuffer&& _other) noexcept
    {
        handle = _other.handle;
        _other.handle = nullptr;
    }

    GpuBuffer& operator=(GpuBuffer&& _other) noexcept
    {
        handle = _other.handle;
        _other.handle = nullptr;
        return *this;
    }
    
    GpuBuffer() = default;

    virtual ~GpuBuffer();

    // TO DO REPLACE BY UNIQUE PTR
    std::shared_ptr<GpuHandle> handle;
};

END_PCCORE