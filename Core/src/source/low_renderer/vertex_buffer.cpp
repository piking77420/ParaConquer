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

    if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &m_GpuHandle))
    {
        PC_LOGERROR("Failed to create vertex buffer");
        return;
    }

}
