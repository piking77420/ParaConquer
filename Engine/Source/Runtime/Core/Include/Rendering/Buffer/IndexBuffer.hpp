#pragma once

#include "GpuBuffer.hpp"
#include "LowRenderer/RhiIndexBuffer.hpp"
#include "LowRenderer/RhiTypedef.h"

BEGIN_PCCORE
    class RhiBuffer;

    class IndexBuffer final : public GpuBuffer
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

        IndexBuffer(const uint8_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                    MemoryUsage _memoryUsageFlag);

        IndexBuffer(const uint16_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                    MemoryUsage _memoryUsageFlag);

        IndexBuffer(const uint32_t* _indicies, size_t _count, MemoryLocalisation _memoryLocalisation,
                    MemoryUsage _memoryUsageFlag);

        IndexBuffer() = default;

        ~IndexBuffer() override = default;

    private:
        std::shared_ptr<RhiIndexBuffer> m_RhiBuffer;
    };

END_PCCORE
