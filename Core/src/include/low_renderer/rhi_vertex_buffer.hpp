#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi_buffer.h"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE

class RhiVertexBuffer : public RhiBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiVertexBuffer)

    PC_CORE_API RhiVertexBuffer(size_t _sizeInByte, BufferMemoryUsage _memoryUsage);
    
    PC_CORE_API RhiVertexBuffer() : RhiBuffer(0, PC_CORE::BufferMemoryUsage::Count)
    {
        
    }
    
    ~RhiVertexBuffer() override = default;
private:
};

END_PCCORE