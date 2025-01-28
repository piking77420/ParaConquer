#include "rendering/vulkan_vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::VertexBuffer::VertexBuffer(Vertex* _vertices, uint32_t _nbr) : m_NbrOfVerticies(_nbr) 
{
    const GPUBufferCreateInfo buffercInfof =
        {
        .data = _vertices,
        .dataSize = sizeof(Vertex) * _nbr,
        .usage = BufferUsage::VertexBuffer
        };

    
}
