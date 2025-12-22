#include "LowRenderer/RhiTexture.hpp"

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi, const std::string& _name)
    : RhiResourceT(_Rhi, _name)
{
}

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi, std::string&& _name)
    : RhiResourceT(_Rhi, std::move(_name))
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
