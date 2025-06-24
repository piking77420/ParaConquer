#include "low_renderer/rhi_vertex_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::RhiVertexBuffer::RhiVertexBuffer(const Vertex* _vertices, uint32_t _size) :  m_NbrOfVerticies(_size / sizeof(Vertex))
{
    
}
