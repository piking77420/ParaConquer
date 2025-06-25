#include "rendering/buffer/index_buffer.hpp"

#include "rhi_vulkan_parser.hpp"
#include "low_renderer/rhi.hpp"


PC_CORE::IndexBuffer::IndexBuffer(uint8_t* _indicies, size_t _count, BufferMemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count, IndexFormat::Uiunt8, _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(uint16_t* _indicies, size_t _count, BufferMemoryUsage _memoryUsageFlag) 
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count, IndexFormat::Uint16, _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(uint32_t* _indicies, size_t _count, BufferMemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint32_t) * _count, IndexFormat::Uint32, _memoryUsageFlag);
}