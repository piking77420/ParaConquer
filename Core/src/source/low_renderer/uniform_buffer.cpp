#include "low_renderer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

using namespace PC_CORE;

void UniformBuffer::Update(void* _data, size_t _size)
{
    if (m_MappedMemory == nullptr)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(m_GpuHandle, &m_MappedMemory))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }
    }

    memcpy(m_MappedMemory, _data, _size);
}

UniformBuffer::UniformBuffer(void* _data, size_t _size)
{
    const GPUBufferCreateInfo info =
         {
        .data = _data,
        .dataSize = _size,
        .usage = BufferUsage::UniformBuffer
        };

    
    if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &m_GpuHandle))
    {
        PC_LOGERROR("Failed to create uniformBuffer buffer");
        return;
    }

    if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(m_GpuHandle, &m_MappedMemory))
    {
        PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
        return;
    }

    memcpy(m_MappedMemory, _data, _size);
    
    if (!Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(m_GpuHandle))
    {
        PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
        return;
    }
    m_MappedMemory = nullptr;
}

UniformBuffer::~UniformBuffer()
{
    if (m_GpuHandle == nullptr)
        return;
    
    if (m_MappedMemory != nullptr)
    {
        Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(m_GpuHandle);
    }
}
