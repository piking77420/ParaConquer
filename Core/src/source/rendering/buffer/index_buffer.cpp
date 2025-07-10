#include "rendering/buffer/index_buffer.hpp"

#include "utils/rhi_vulkan_parser.hpp"
#include "low_renderer/rhi.hpp"


PC_CORE::IndexBuffer::IndexBuffer(uint8_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count ,IndexFormat::Uiunt8, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(uint16_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag) 
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count ,IndexFormat::Uint16, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(uint32_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint32_t) * _count ,IndexFormat::Uint32, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}