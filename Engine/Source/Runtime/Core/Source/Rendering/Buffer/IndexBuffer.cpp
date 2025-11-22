#include "Rendering/Buffer/IndexBuffer.hpp"

#include "Utils/RhiToVulkan.hpp"
#include "LowRenderer/Rhi.hpp"


PC_CORE::IndexBuffer::IndexBuffer(const uint8_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                                  MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count, IndexFormat::Uiunt8,
                                         MemoryLocalisation::GpuOnly, _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(const uint16_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                                  MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint8_t) * _count, IndexFormat::Uint16,
                                         MemoryLocalisation::GpuOnly, _memoryUsageFlag);
}

PC_CORE::IndexBuffer::IndexBuffer(const uint32_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                                  MemoryUsage _memoryUsageFlag)
{
    m_RhiBuffer = Rhi::CreateIndexBuffer(_indicies, sizeof(uint32_t) * _count, IndexFormat::Uint32,
                                         MemoryLocalisation::GpuOnly, _memoryUsageFlag);
}
