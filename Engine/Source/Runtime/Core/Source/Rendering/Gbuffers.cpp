#include "rendering/gbuffers.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ComputeShader.hpp"


PC_CORE::Gbuffers::Gbuffers(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _size)
{
    CreateGBuffers(_Rhi, _size);
}

void PC_CORE::Gbuffers::CreateGBuffers(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _size)
{
    GbufferType gbufferType = {};
    for (auto& frameInFlight : gbuffers)
    {
        RhiTexture::RhiTextureDesciptor rhiTextureDesciptor = 
        {
        .Width = static_cast<uint32_t>(_size.x),
        .Height = static_cast<uint32_t>(_size.y),
        .Depth = 1,
        .Level = 1,
        .LayerCount = 1,
        .Samples = 1,
        .TextureType = RhiTexture::Type::Texture2D,
        .TextureUsage = static_cast<RhiTexture::TextureUsageFlag>(RhiTexture::RenderTarget | RhiTexture::Sampled),
        .RhiFormat = RhiFormat::Undefined,
        .AllowCpuAcces = false  
        };

        switch (gbufferType)
        {
        case GbufferType::Albedo:
            rhiTextureDesciptor.RhiFormat = RhiFormat::R16G16B16A16Sfloat;
            break;
        case GbufferType::Normal:
            rhiTextureDesciptor.RhiFormat = RhiFormat::R16G16Snorm;
            break;
        case GbufferType::RoughnessMetallicAo:
            rhiTextureDesciptor.RhiFormat = RhiFormat::R8G8B8A8Unorm;
            break;
        case GbufferType::WorldPosition:
            rhiTextureDesciptor.RhiFormat = RhiFormat::R16G16B16A16Sfloat;
            break;
        case GbufferType::Count:
        default:
            assert(false);
        }

        frameInFlight = Texture2D(_Rhi, "Gbuffer" + GbufferTypeToString(gbufferType), rhiTextureDesciptor, RhiResource::MemoryUsage::Dynamic);
        frameInFlight->Build();

        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % static_cast<uint8_t>(
            GbufferType::Count));
    }
}

PC_CORE::Texture2D& PC_CORE::Gbuffers::GetTexture(GbufferType _type)
{
    const size_t index = static_cast<size_t>(_type);
    assert(gbuffers.size() <= index);
    return gbuffers[index];
}

const PC_CORE::Texture2D& PC_CORE::Gbuffers::GetTexture(GbufferType _type) const
{
    const size_t index = static_cast<size_t>(_type);
    assert(gbuffers.size() <= index);
    return gbuffers[index];
}
