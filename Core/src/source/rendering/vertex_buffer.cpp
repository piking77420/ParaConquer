#include "rendering/vulkan_vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::VertexBuffer::VertexBuffer(const Vertex* _vertices, uint32_t _nbr) : m_NbrOfVerticies(_nbr) 
{
    const GPUBufferCreateInfo bufferCreateInfo =
        {
        .data = _vertices,
        .dataSize = sizeof(Vertex) * _nbr,
        .usage = BufferUsage::VertexBuffer
        };

    if (!Rhi::GetRhiContext()->gpuAllocator->CreateVulkanBuffer(bufferCreateInfo, &m_GpuHandle))
    {
        PC_LOGERROR("Failed to create Vulkan buffer");
    }
}
