#include "low_renderer/uniform_buffer.hpp"

#include "low_renderer/rhi.hpp"

using namespace PC_CORE;

void UniformBuffer::Update(void* _data, size_t _size)
{
    const size_t currentFrame = Rhi::GetFrameIndex();

    if (m_MappedMemory[currentFrame] == nullptr)
    {
        Rhi::MapBuffer(bufferHandles[currentFrame], &m_MappedMemory[currentFrame]);
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
        b = Rhi::CreateBuffer(info);
        Rhi::MapBuffer(b, &m_MappedMemory[i]);

        if (m_MappedMemory[i] == nullptr)
            continue;

        memcpy(m_MappedMemory[i], _data, _size);
        Rhi::UnMapBuffer(b);
        m_MappedMemory[i] = nullptr;
        i++;
    }
    
    
}

UniformBuffer::~UniformBuffer()
{
    for (size_t i = 0 ; i < bufferHandles.size() ; i++)
    {

        if (bufferHandles[i] == GPU_INVALID_ID)
            continue;
    
        if (m_MappedMemory[i] != nullptr)
        {
            Rhi::UnMapBuffer(bufferHandles[i]);
        }
    }
 
}


