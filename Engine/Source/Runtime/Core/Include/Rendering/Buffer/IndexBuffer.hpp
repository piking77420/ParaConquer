#pragma once

#include "GpuBuffer.hpp"

BEGIN_PCCORE
    class IndexBuffer final : public GpuBuffer
    {
    public:

        IndexBuffer() = default;

        explicit IndexBuffer(Rhi& rhi);

        ~IndexBuffer() override = default;

        IndexBuffer(const IndexBuffer&) = delete;

        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&&) noexcept = default;

        IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

        IndexBuffer& SetIndexFormat(RhiBuffer::IndexFormat _IndexFormat)
        {
            m_RhiBufferFormat = _IndexFormat;
            return *this;
        }

        IndexBuffer& SetIndexCount(size_t _IndexCount)
        {
            m_IndiciesCount = _IndexCount;
            return *this;
        }


        RhiBuffer::IndexFormat GetIndexFormat() const
        {
            return m_RhiBufferFormat;
        }

        size_t GetIndexCount() const
        {
            return m_IndiciesCount;
        }

    private:
        RhiBuffer::IndexFormat m_RhiBufferFormat{};
    
        size_t m_IndiciesCount = 0;
    };

END_PCCORE
