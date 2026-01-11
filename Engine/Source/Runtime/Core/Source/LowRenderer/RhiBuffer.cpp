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
        assert( ((m_Usage & BufferUsageFlagBits::Uniform) != 0 || (m_Usage & BufferUsageFlagBits::ShaderStorage != 0) ) &&
            "CpuVisibleRing only available for uniform buffers");
        const uint32_t minUniformAlignment = m_Rhi.GetRhiContext().rhiPhysicalDevices->GetPhysicalDevice().GetUniformBufferOffsetAlignment();
        m_NbrOfBackendObject = 1;

        assert(m_MaxObjectPerFrame != 0 && m_ObjectSize != 0);

        m_Stride = AlignUp(m_ObjectSize , minUniformAlignment);
        m_FrameStride = m_MaxObjectPerFrame * m_Stride;
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
