#include "Rendering/Buffer/VertexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"


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

PC_CORE::VertexBuffer::VertexBuffer(Vertex* _vertices, size_t _count, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage)
{
	m_RhiBuffer = Rhi::CreateVertexBuffer(_vertices, _count * sizeof(Vertex), _localisation, _usage);
}

PC_CORE::VertexBuffer::VertexBuffer(const void* _data, size_t _vertexCount, size_t _vertexSize,
	PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage)
	: m_Count(_vertexCount)
{
	m_RhiBuffer = Rhi::CreateVertexBuffer(_data, _vertexCount * _vertexSize, _localisation, _usage);
}

PC_CORE::VertexBuffer::VertexBuffer(size_t _vertexCout, size_t _verticiesSize, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage) : m_Count(_vertexCout)
{
	m_RhiBuffer = Rhi::CreateVertexBuffer(static_cast<uint32_t>(_vertexCout * _verticiesSize), _localisation, _usage);
}

PC_CORE::VertexBuffer::VertexBuffer(size_t _size, PC_CORE::MemoryLocalisation _localisation, MemoryUsage _usage) : m_Count(std::numeric_limits<decltype(m_Count)>::max())
{
	m_RhiBuffer = Rhi::CreateVertexBuffer(_size, _localisation, _usage);
}

