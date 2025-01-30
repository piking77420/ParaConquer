#include "low_renderer/vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::VertexBuffer::VertexBuffer(const Vertex* _vertices, uint32_t _nbr) :  m_NbrOfVerticies(_nbr)
{
    const GPUBufferCreateInfo info =
        {
        .data = _vertices,
        .dataSize = sizeof(Vertex) * _nbr,
        .usage = BufferUsage::VertexBuffer
        };

    if (!Rhi::GetRhiContext()->gpuAllocator->CreateVulkanBuffer(info, &m_GpuHandle))
    {
        PC_LOGERROR("Failed to create vertex buffer");
        return;
    }
    /*

    if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(m_GpuHandle, &m_data))
    {
        PC_LOGERROR("Failed to map vertex buffer");
        return;
    }

    memcpy(m_data, _vertices, info.dataSize);
    
    if (!Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(m_GpuHandle))
    {
        PC_LOGERROR("Failed to map vertex buffer");
        return;
    }*/
}
