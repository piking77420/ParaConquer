#pragma once

#include "core_header.hpp"
#include "gpu_buffer.hpp"
#include "low_renderer/vertex.hpp"

BEGIN_PCCORE
// Make 
class VertexBuffer : public GpuBuffer
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(VertexBuffer)

    VertexBuffer(const Vertex* vertices, uint32_t size);
    
    VertexBuffer() = default;
    
    ~VertexBuffer() = default;

    inline size_t GetNbrOfVerticies() const
    {
        return m_NbrOfVerticies;
    }

private:
    size_t m_NbrOfVerticies = 0;

    
};

END_PCCORE