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

PC_CORE::RhiIndexBuffer::RhiIndexBuffer(const uint32_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uint32),
                                                                            m_IndexCount(_size / sizeof(uint32_t))
{
   
}

PC_CORE::RhiIndexBuffer::RhiIndexBuffer(const uint16_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uint16),
                                                                            m_IndexCount(_size / sizeof(uint16_t))
{
   
}

PC_CORE::RhiIndexBuffer::RhiIndexBuffer(const uint8_t* _indices, size_t _size) : m_IndexFormat(IndexFormat::Uiunt8),
                                                                           m_IndexCount(_size / sizeof(uint8_t))
{
    
}
