#include "LowRenderer/RhiBuffer.h"
#include <LowRenderer/Rhi.hpp>

PC_CORE::RhiBuffer::RhiBuffer(Rhi& _Rhi)
    : RhiResourceT(_Rhi)
{
}

bool PC_CORE::RhiBuffer::Build()
{
    ChooseBackingStrategy();

    return true;
}

void PC_CORE::RhiBuffer::ChooseBackingStrategy()
{
    MemoryUsage MemoryUsage = GetMemoryUsage();
    BufferUpdateRate BufferUpdateRate = GetBufferUpdateRate();

    BufferBackingStrategy strategy = {};

    switch (MemoryUsage)
    {
    case MemoryUsage::StaticGPU:
    {
        switch (BufferUpdateRate)
        {
        case BufferUpdateRate::Static:
            strategy = BufferBackingStrategy::SingleBuffer;
            break;

        case BufferUpdateRate::PerFrame:
        case BufferUpdateRate::Dynamic:
            // GPU-only but updated → must stage
            strategy = BufferBackingStrategy::StagedUpload;
            break;

        default:
            assert(false);
            break;
        }
        break;
    }

    case MemoryUsage::CPUVisible:
    {
        switch (BufferUpdateRate)
        {
        case BufferUpdateRate::Static:
            strategy = BufferBackingStrategy::SingleBuffer;
            break;

        case BufferUpdateRate::PerFrame:
            strategy = BufferBackingStrategy::PerFrameBuffers;
            break;

        case BufferUpdateRate::Dynamic:
            strategy = BufferBackingStrategy::CpuVisibleRing;
            break;

        default:
            assert(false);
            break;
        }
        break;
    }

    case MemoryUsage::ReadbackCPU:
    {
        // Update rate is irrelevant for readback
        strategy = BufferBackingStrategy::Readback;
        break;
    }

    default:
        assert(false);
        break;
    }

    m_BufferBackingStrategy = strategy;

    switch (m_BufferBackingStrategy)
    {
    case BufferBackingStrategy::SingleBuffer:
        m_NbrOfBackendObject = 1;
        break;
    case BufferBackingStrategy::CpuVisibleRing:
    {
       const bool isUniform =
            (m_Usage & BufferUsageFlagBits::Uniform) != 0;
        const bool isStorage =
            (m_Usage & BufferUsageFlagBits::ShaderStorage) != 0;

        assert(isUniform || isStorage);
        uint32_t alignment = 1;

        if (isUniform)
        {
            alignment =
                m_Rhi.GetRhiContext()
                .rhiPhysicalDevices
                ->GetPhysicalDevice()
                .GetUniformBufferOffsetAlignment();
        }
        else 
        {
            alignment =
                m_Rhi.GetRhiContext()
                .rhiPhysicalDevices
                ->GetPhysicalDevice()
                .GetStorageBufferOffsetAlignment();
        }

        assert(alignment != 0);
        assert((alignment & (alignment - 1)) == 0);

        m_Stride = AlignUp(m_ObjectSize, alignment);
        m_FrameStride = m_MaxObjectPerFrame * m_Stride;
        m_NbrOfBackendObject = 1;
    }
        break;

    case BufferBackingStrategy::PerFrameBuffers:
        m_NbrOfBackendObject = MaxFramesInFlight;
        break;
    case BufferBackingStrategy::StagedUpload:
        m_NbrOfBackendObject = 1; // GPU buffer only
        break;
    case BufferBackingStrategy::Readback:
        m_NbrOfBackendObject = 1;
        break;
    default:
        assert(false);
        break;
    }
}
