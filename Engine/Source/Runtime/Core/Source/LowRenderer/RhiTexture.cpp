#include "LowRenderer/RhiTexture.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi)
    : RhiResourceT(_Rhi)
{
}

bool PC_CORE::RhiTexture::Build()
{
    switch (m_MemoryUsage)
    {
    case PC_CORE::RhiResource::MemoryUsage::StaticGPU:

    {
        m_NbrOfBackendObject = 1;
    }
        break;
    case PC_CORE::RhiResource::MemoryUsage::CPUVisible:
        assert(false && "Not supported");
        break;
    case PC_CORE::RhiResource::MemoryUsage::ReadbackCPU:
        assert(false && "Not supported");
        break;
    default:
        break;
    }

    return true;
}

bool PC_CORE::RhiTexture::IsDepthFormat(RhiFormat _format)
{
    if (_format == PC_CORE::RhiFormat::D32Sfloat ||
        _format == PC_CORE::RhiFormat::D24UnormS8Uint ||
        _format == PC_CORE::RhiFormat::D16UnormS8Uint ||
        _format == PC_CORE::RhiFormat::D32SfloatS8Uint)
    {
        
        return true;
    }

    return false;
}
