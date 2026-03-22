#pragma once

#include "CoreHeader.hpp"
#include "Rendering/GpuResource.hpp"
#include "LowRenderer/RhiBuffer.h"

BEGIN_PCCORE
    class GpuBuffer : public IGpuResource
    {
    public:
        GpuBuffer(const GpuBuffer&) = delete;

        GpuBuffer& operator=(const GpuBuffer&) = delete;

        GpuBuffer(GpuBuffer&&) noexcept = default;

        GpuBuffer& operator=(GpuBuffer&&) noexcept = default;

        PC_CORE_API GpuBuffer() = default;

        PC_CORE_API ~GpuBuffer() override = default;
    
        RhiBuffer* Get() const
        {
            return m_RhiBuffer.get();
        }
    
        RhiBuffer* const operator->() const
        {
            return m_RhiBuffer.get();
        }

        RhiBuffer& operator*()
        {
            return *m_RhiBuffer;
        }

        const RhiBuffer& operator*() const
        {
            return *m_RhiBuffer;
        }

    protected:
        std::unique_ptr<RhiBuffer> m_RhiBuffer;

    };

END_PCCORE
