#pragma once

#include "GpuBuffer.hpp"

BEGIN_PCCORE
    class IndexBuffer final : public GpuBuffer
    {
    public:
        RhiBuffer::IndexFormat GetIndexFormat() const
        {
            return m_RhiBufferFormat;
        }

        size_t GetIndexCount() const
        {
            return m_IndiciesCount;
        }

        DEFAULT_COPY_MOVE_OPERATIONS(IndexBuffer)
    
        explicit IndexBuffer(Rhi& rhi, const std::string& _name, size_t _indexCount,  RhiBuffer::IndexFormat _indexFormat, PC_CORE::RhiResource::MemoryUsage _memoryUsage);
    
        IndexBuffer() = default;

        ~IndexBuffer() override = default;
    private:
        RhiBuffer::IndexFormat m_RhiBufferFormat{};
    
        size_t m_IndiciesCount = 0;
    };

END_PCCORE
