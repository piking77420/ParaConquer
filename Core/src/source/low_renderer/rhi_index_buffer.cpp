#include "low_renderer/rhi_index_buffer.hpp"

#include "low_renderer/rhi.hpp"

PC_CORE::IndexFormat PC_CORE::RhiIndexBuffer::GetIndexFormat() const
{
    return m_IndexFormat;
}

uint32_t PC_CORE::RhiIndexBuffer::GetIndexCount() const
{
    return m_IndexCount;
}
