#include "rendering/buffer/index_buffer.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::IndexBuffer::IndexBuffer(IndexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    usage = _other.usage;
    m_IndiciesCount = _other.m_IndiciesCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_IndiciesCount = 0;
}

PC_CORE::IndexBuffer& PC_CORE::IndexBuffer::operator=(IndexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    usage = _other.usage;
    m_IndiciesCount = _other.m_IndiciesCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_IndiciesCount = 0;

    return *this;
}

PC_CORE::IndexBuffer::IndexBuffer(CommandPool* _transfertPool, const std::vector<uint32_t>& _indicies)
{
    usage = BUFFER_USAGE_INDEX;
    m_IndiciesCount = _indicies.size();
    size = m_IndiciesCount * sizeof(uint32_t);
    handleId = RHI::GetInstance()->BufferData(_transfertPool, size, _indicies.data(), usage);
}

PC_CORE::IndexBuffer::IndexBuffer()
{
    usage = BUFFER_USAGE_INDEX;
}

PC_CORE::IndexBuffer::~IndexBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    RHI::GetInstance()->DestroyBuffer(handleId);
    handleId = nullptr;
    size = 0;
    m_IndiciesCount = 0;
}
