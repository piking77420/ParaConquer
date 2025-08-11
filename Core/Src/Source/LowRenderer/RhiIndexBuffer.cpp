#include "LowRenderer/RhiIndexBuffer.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::IndexFormat PC_CORE::RhiIndexBuffer::GetIndexFormat() const
{
    return m_IndexFormat;
}

uint32_t PC_CORE::RhiIndexBuffer::GetIndexCount() const
{
    return m_IndexCount;
}
