#pragma once

#include "rhi_buffer.h"
#include "low_renderer/rhi_uniform_buffer.hpp"

BEGIN_PCCORE

class RhiUniformBuffer : public RhiBuffer
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(RhiUniformBuffer)

    PC_CORE_API virtual void Update(void* _data, size_t _size) = 0;

    PC_CORE_API RhiUniformBuffer(const GPUBufferCreateInfo& _createInfo);
    
    PC_CORE_API RhiUniformBuffer() = default;
    
    PC_CORE_API virtual ~RhiUniformBuffer() = default;
private:
    size_t m_Size = 0;
};

END_PCCORE