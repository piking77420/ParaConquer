#pragma once

#include "core_header.hpp"
#include "low_renderer/gpu_buffer_handle.hpp"

#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE

class GpuBuffer
{
public:

    GpuBuffer(GpuBuffer&& _other) noexcept
    {
        m_GpuHandle = _other.m_GpuHandle;
        _other.m_GpuHandle = nullptr;
    }

    GpuBuffer& operator=(GpuBuffer&& _other) noexcept
    {
        m_GpuHandle = _other.m_GpuHandle;
        _other.m_GpuHandle = nullptr;
        return *this;
    }
    
    GpuBuffer() = default;

    virtual ~GpuBuffer();

    inline std::shared_ptr<GpuBufferHandle> GetHandle() const
    {
        return m_GpuHandle;
    }

protected:
    std::shared_ptr<GpuBufferHandle> m_GpuHandle;
};

END_PCCORE