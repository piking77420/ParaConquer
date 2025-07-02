#include "rendering/buffer/vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"


void PC_CORE::VertexBuffer::Update(void* _data, size_t _size) const
{
    void* mappedData = nullptr;
    m_RhiBuffer->MapData(&mappedData);

    if (mappedData == nullptr)
    {
        PC_LOGERROR("Failed to map vertexBuffer buffer data");
        return;
    }
    std::memcpy(mappedData, _data, _size);

    m_RhiBuffer->UnmapData();
}

PC_CORE::VertexBuffer::VertexBuffer(Vertex* _vertices, size_t _count,PC_CORE::MemoryLocalisation _localisation,  MemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateVertexBuffer(_vertices, _count * sizeof(Vertex), _localisation, _usage);
}

PC_CORE::VertexBuffer::VertexBuffer(size_t _sizeInBytes, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateVertexBuffer(static_cast<uint32_t>(_sizeInBytes), _localisation, _usage);
}

