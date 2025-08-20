#include "Rendering/Buffer/IndexBuffer.hpp"

#include "Utils/RhiVulkanParser.hpp"
#include "LowRenderer/Rhi.hpp"


PC_CORE::IndexBuffer::IndexBuffer(const uint8_t* _indicies, size_t _count, PC_CORE::MemoryLocalisation _memoryLocalisation, MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count ,IndexFormat::Uiunt8, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(const uint16_t* _indicies, size_t _count, PC_CORE::MemoryLocalisation _memoryLocalisation, MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count ,IndexFormat::Uint16, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(const uint32_t* _indicies, size_t _count, PC_CORE::MemoryLocalisation _memoryLocalisation, MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint32_t) * _count ,IndexFormat::Uint32, PC_CORE::MemoryLocalisation::GPU_Only,  _memoryUsageFlag);
}