#pragma once

#include "gpu_buffer.hpp"
#include "low_renderer/rhi_index_buffer.hpp"
#include "low_renderer/rhi_typedef.h"

BEGIN_PCCORE
    class RhiBuffer;

    class IndexBuffer final : public PC_CORE::GpuBuffer
    {
    public:
        std::shared_ptr<RhiResource> GetRhiHandle() const override
        {
            return m_RhiBuffer;
        }

        IndexFormat GetIndexFormat() const
        {
            return m_RhiBuffer->GetIndexFormat();
        }

        size_t GetIndexCount() const
        {
            return m_RhiBuffer->GetIndexCount();
        }

        std::shared_ptr<RhiIndexBuffer> GetRhiBuffer() const
        {
            return m_RhiBuffer;
        }

        DEFAULT_COPY_MOVE_OPERATIONS(IndexBuffer)

        IndexBuffer(uint8_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag);

        IndexBuffer(uint16_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag);

        IndexBuffer(uint32_t* _indicies, size_t _count, MemoryUsage _memoryUsageFlag);

        IndexBuffer() = default;

        ~IndexBuffer() override = default;

    private:
        std::shared_ptr<RhiIndexBuffer> m_RhiBuffer;

    };

END_PCCORE
