#include "rendering/buffer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

void PC_CORE::UniformBuffer::Update(void* _data, size_t _size)
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

PC_CORE::UniformBuffer::UniformBuffer(void* _data, size_t _size, MemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateUniformBuffer(_data, _size, PC_CORE::MemoryLocalisation::CPU_Only, _usage);
}
