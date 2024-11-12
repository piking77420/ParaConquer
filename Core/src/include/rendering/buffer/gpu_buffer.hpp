#pragma once
#include "rendering/render_harware_interface/rhi_typedef.h"

BEGIN_PCCORE

class CommandPool;

struct GPUBuffer
{
    size_t size = 0;
    PC_CORE::GPUBufferHandle handleId = nullptr;

    GPUBuffer() = default;

    ~GPUBuffer() = default;

    GPUBuffer(GPUBuffer&& _other) noexcept
    {
        handleId = _other.handleId;
        size = _other.size;
        _other.handleId = nullptr;
        _other.size = 0;
    }

    GPUBuffer& operator=(GPUBuffer&& _other) noexcept
    {
        handleId = _other.handleId;
        size = _other.size;
        _other.handleId = nullptr;
        _other.size = 0;

        return *this;
    }
};

END_PCCORE