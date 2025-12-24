#include "LowRenderer/RhiTexture.hpp"

#include "LowRenderer/Rhi.hpp"

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi)
    : RhiResourceT(_Rhi)
{
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
