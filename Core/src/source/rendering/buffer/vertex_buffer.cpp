#include "rendering/buffer/vertex_buffer.hpp"

#include "OBJ_Loader.h"
#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::VertexBuffer::VertexBuffer(VertexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_VertexCount = _other.m_VertexCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_VertexCount = 0;

    return;
}

PC_CORE::VertexBuffer& PC_CORE::VertexBuffer::operator=(VertexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_VertexCount = _other.m_VertexCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_VertexCount = 0;

    return *this;
}

PC_CORE::VertexBuffer::VertexBuffer( const std::vector<Vertex>& _vertices)
{
    m_VertexCount = static_cast<uint32_t>(_vertices.size());
    size = m_VertexCount * sizeof(Vertex);
    handleId = RHI::GetInstance().BufferData(size, _vertices.data(), GPU_BUFFER_USAGE::BUFFER_USAGE_VERTEX_BUFFER_BIT);
}



PC_CORE::VertexBuffer::VertexBuffer()
{
}

PC_CORE::VertexBuffer::~VertexBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    size = 0;
    m_VertexCount = 0;
    RHI::GetInstance().DestroyBuffer(handleId);
    handleId = nullptr;
}
