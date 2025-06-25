#pragma once

#include "core_header.hpp"
#include "low_renderer/rhi_buffer.h"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE

class RhiVertexBuffer : public RhiBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(RhiVertexBuffer)

    RhiVertexBuffer(const Vertex* _vertices, uint32_t _size, BufferMemoryUsage _memoryUsage);
    
    RhiVertexBuffer() = default;
    
    ~RhiVertexBuffer() override = default;
private:
};

END_PCCORE