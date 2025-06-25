#include "rendering/buffer/vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"


PC_CORE::VertexBuffer::VertexBuffer(Vertex* _vertices, size_t _count, BufferMemoryUsage _usage)
{
    m_RhiBuffer = Rhi::CreateVertexBuffer(_vertices, _count * sizeof(Vertex), _usage);
}

