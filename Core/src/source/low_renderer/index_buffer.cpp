#include "low_renderer/index_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::IndexFormat PC_CORE::IndexBuffer::GetIndexFormat() const
{
    return m_IndexFormat;
}

uint32_t PC_CORE::IndexBuffer::GetIndexCount() const
{
    return m_IndexCount;
}

PC_CORE::IndexBuffer::IndexBuffer(const uint32_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uint32),
                                                                            m_IndexCount(_size / sizeof(uint32_t))
{
    const GPUBufferCreateInfo info =
    {
        .data = _indices,
        .dataSize = _size,
        .usage = BufferUsage::IndexBuffer
    };

    for (auto& b : bufferHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &b))
        {
            PC_LOGERROR("Failed to create vertex buffer");
            return;
        }
    }
}

PC_CORE::IndexBuffer::IndexBuffer(const uint16_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uint16),
                                                                            m_IndexCount(_size / sizeof(uint16_t))
{
    const GPUBufferCreateInfo info =
    {
        .data = _indices,
        .dataSize = _size,
        .usage = BufferUsage::IndexBuffer
    };
    for (auto& b : bufferHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &b))
        {
            PC_LOGERROR("Failed to create vertex buffer");
            return;
        }
    }
}

PC_CORE::IndexBuffer::IndexBuffer(const uint8_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uiunt8),
                                                                           m_IndexCount(_size / sizeof(uint8_t))
{
    const GPUBufferCreateInfo info =
    {
        .data = _indices,
        .dataSize = _size,
        .usage = BufferUsage::IndexBuffer
    };

    for (auto& b : bufferHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateGPUBuffer(info, &b))
        {
            PC_LOGERROR("Failed to create vertex buffer");
            return;
        }
    }
}
