#pragma once

#include "CoreHeader.hpp"
#include "Rendering/GpuResource.hpp"
#include "LowRenderer/RhiBuffer.h"

BEGIN_PCCORE
    class GpuBuffer : public IGpuResource
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(GpuBuffer)

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

        
    protected:
        std::unique_ptr<RhiBuffer> m_RhiBuffer;

    };

END_PCCORE
