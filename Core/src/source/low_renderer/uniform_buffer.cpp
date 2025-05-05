#include "low_renderer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

using namespace PC_CORE;

void UniformBuffer::Update(void* _data, size_t _size)
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    if (m_MappedMemory[currentFrame] == nullptr)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(bufferHandles[currentFrame],
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

    int i = 0;
    for (auto& b : bufferHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &b))
        {
            PC_LOGERROR("Failed to create uniformBuffer buffer");
            return;
        }

        if (!Rhi::GetRhiContext()->gpuAllocator->MapBuffer(b, &m_MappedMemory[i]))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }

        memcpy(m_MappedMemory[i], _data, _size);
    
        if (!Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(b))
        {
            PC_LOGERROR("Failed to MapBuffer unfiorm buffer");
            return;
        }
        m_MappedMemory[i] = nullptr;
        i++;
    }
    
    
}

UniformBuffer::~UniformBuffer()
{
    for (size_t i = 0 ; i < bufferHandles.size() ; i++)
    {

        if (bufferHandles[i] == nullptr)
            continue;
    
        if (m_MappedMemory[i] != nullptr)
        {
            Rhi::GetRhiContext()->gpuAllocator->UnMapBuffer(bufferHandles[i]);
        }
    }
 
}

const std::shared_ptr<GpuHandle>* UniformBuffer::GetHandle() const
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    return &bufferHandles[currentFrame];
}

std::shared_ptr<GpuHandle>* UniformBuffer::GetHandle()
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    return &bufferHandles[currentFrame];
}
