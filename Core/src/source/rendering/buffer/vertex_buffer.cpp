#include "rendering/buffer/vertex_buffer.hpp"

#include "OBJ_Loader.h"
#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::VertexBuffer::VertexBuffer(VertexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    usage = _other.usage;

    _other.handleId = nullptr;
    _other.size = 0;

    return;
}

PC_CORE::VertexBuffer& PC_CORE::VertexBuffer::operator=(VertexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    usage = _other.usage;

    _other.handleId = nullptr;
    _other.size = 0;
    
    return *this;
}

PC_CORE::VertexBuffer::VertexBuffer(CommandPool* _transfertPool, const std::vector<Vertex>& _vertices)
{
    usage = GPU_BUFFER_USAGE::VERTEX;
    size = _vertices.size() * sizeof(Vertex);
    handleId = RHI::GetInstance()->BufferData(_transfertPool, _vertices.size(), _vertices.data(), GPU_BUFFER_USAGE::VERTEX);
}



PC_CORE::VertexBuffer::VertexBuffer()
{
    usage = GPU_BUFFER_USAGE::VERTEX;
}

PC_CORE::VertexBuffer::~VertexBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    RHI::GetInstance()->DestroyBuffer(handleId);
    handleId = nullptr;
    size = 0;
}
