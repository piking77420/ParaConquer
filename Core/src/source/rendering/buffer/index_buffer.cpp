#include "rendering/buffer/index_buffer.hpp"

#include "rendering/render_harware_interface/RHI.hpp"

PC_CORE::IndexBuffer::IndexBuffer(IndexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_IndiciesCount = _other.m_IndiciesCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_IndiciesCount = 0;
}

PC_CORE::IndexBuffer& PC_CORE::IndexBuffer::operator=(IndexBuffer&& _other) noexcept
{
    size = _other.size;
    handleId = _other.handleId;
    m_IndiciesCount = _other.m_IndiciesCount;

    _other.handleId = nullptr;
    _other.size = 0;
    _other.m_IndiciesCount = 0;

    return *this;
}

PC_CORE::IndexBuffer::IndexBuffer(const std::vector<uint32_t>& _indicies)
{
    m_IndiciesCount = static_cast<uint32_t>(_indicies.size());
    size = m_IndiciesCount * sizeof(uint32_t);
    handleId = RHI::GetInstance().BufferData(size, _indicies.data(), GPU_BUFFER_USAGE::INDEX);
}

PC_CORE::IndexBuffer::IndexBuffer()
{
}

PC_CORE::IndexBuffer::~IndexBuffer()
{
    if (handleId == nullptr && size == 0)
        return;

    size = 0;
    m_IndiciesCount = 0;
    RHI::GetInstance().DestroyBuffer(handleId);
    handleId = nullptr;
}
