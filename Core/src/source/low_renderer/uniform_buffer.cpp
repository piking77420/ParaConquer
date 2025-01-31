#include "low_renderer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

using namespace PC_CORE;

void UniformBuffer::Update(void* _data, size_t _size)
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    if (m_MappedMemory[currentFrame] == nullptr)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(m_UniformBuffers[currentFrame].handle,
                                                           &m_MappedMemory[currentFrame]))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }
    }
    memcpy(m_MappedMemory[currentFrame], _data, _size);
}

UniformBuffer::UniformBuffer(void* _data, size_t _size)
{
    const GPUBufferCreateInfo info =
         {
        .data = _data,
        .dataSize = _size,
        .usage = BufferUsage::UniformBuffer
        };

    for (size_t i = 0 ; i < m_UniformBuffers.size() ; i++)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &m_UniformBuffers[i].handle))
        {
            PC_LOGERROR("Failed to create uniformBuffer buffer");
            return;
        }

        if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(m_UniformBuffers[i].handle, &m_MappedMemory[i]))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }

        memcpy(m_MappedMemory[i], _data, _size);
    
        if (!Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(m_UniformBuffers[i].handle))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }
        m_MappedMemory[i] = nullptr;
    }

    
    
}

UniformBuffer::~UniformBuffer()
{
    for (size_t i = 0 ; i < m_UniformBuffers.size() ; i++)
    {

        if (m_UniformBuffers[i].handle == nullptr)
            return;
    
        if (m_MappedMemory[i] != nullptr)
        {
            Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(m_UniformBuffers[i].handle);
        }
    }
 
}

const std::shared_ptr<GpuHandle>* UniformBuffer::GetHandle() const
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    return &m_UniformBuffers[currentFrame].handle;
}

std::shared_ptr<GpuHandle>* UniformBuffer::GetHandle()
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    return &m_UniformBuffers[currentFrame].handle;
}
