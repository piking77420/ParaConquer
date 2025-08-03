#include "rendering/buffer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

void PC_CORE::UniformBuffer::Update(const void* _data, size_t _size)
{

    void* mappedData = nullptr;
    m_RhiBuffer->MapData(&mappedData);

    if (mappedData == nullptr)
    {
        PC_LOGERROR("Failed to map uniform buffer data");
        return;
    }
    std::memcpy(mappedData, _data, _size);

    m_RhiBuffer->UnmapData();
}

PC_CORE::UniformBuffer::UniformBuffer(void* _data, size_t _size, MemoryLocalisation _memoryLocalisation, MemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateUniformBuffer(_data, _size, _memoryLocalisation, _usage);
}

PC_CORE::UniformBuffer::UniformBuffer(size_t _size, MemoryLocalisation _memoryLocalisation, MemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateUniformBuffer(nullptr, _size, _memoryLocalisation, _usage);
}
