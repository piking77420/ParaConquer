#include "LowRenderer/RhiTexture.hpp"

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi, const std::string& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage)
    : RhiResource(_Rhi, _name, _memoryUsage)
    , m_RhiTextureDesciptor(_rhiTextureDesciptor)
{
}

PC_CORE::RhiTexture::RhiTexture(Rhi& _Rhi, std::string&& _name, const RhiTextureDesciptor& _rhiTextureDesciptor, MemoryUsage _memoryUsage)
    : RhiResource(_Rhi, std::move(_name), _memoryUsage)
    , m_RhiTextureDesciptor(std::move(_rhiTextureDesciptor))
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
