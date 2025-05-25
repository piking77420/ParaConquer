#include "low_renderer/vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::VertexBuffer::VertexBuffer(const Vertex* _vertices, uint32_t _size) :  m_NbrOfVerticies(_size / sizeof(Vertex))
{
    const GPUBufferCreateInfo info =
        {
        .data = _vertices,
        .dataSize = _size,
        .usage = BufferUsage::VertexBuffer
        };

    for (auto& b : bufferHandles)
    {
        b = Rhi::CreateBuffer(info);
    }

  
}
