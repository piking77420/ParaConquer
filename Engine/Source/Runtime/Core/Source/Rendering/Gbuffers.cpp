#include "rendering/gbuffers.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/ComputeShader.hpp"


PC_CORE::Gbuffers::Gbuffers(Tbx::Vector2i _size)
{
    CreateGBuffers(_size);
}

void PC_CORE::Gbuffers::CreateGBuffers(Tbx::Vector2i _size)
{
    GbufferType gbufferType = {};
    for (auto& frameInFlight : gbuffers)
    {
        CreateImageInfo texture_info =
        {
            .Width = _size.x,
            .Height = _size.y,
            .Depth = 1,
            .LayerCount = 1,
            .MipsLevels = 1,
            .TextureType = TextureType::Texture2D,
            .Format = RhiFormat::Undefined,
            .Channel = Channel::Default,
            .TextureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .MemoryVisibility = MemoryLocalisation::GpuOnly,
            .Samples = 1,
            .GenerateMipMap = false,
            .Datas = {},
        };

        switch (gbufferType)
        {
        case GbufferType::Albedo:
            texture_info.Format = RhiFormat::R16G16B16A16Sfloat;
            texture_info.Channel = Channel::Rgba;
            break;
        case GbufferType::Normal:
            texture_info.Format = RhiFormat::R16G16Snorm;
            texture_info.Channel = Channel::Rgb;
            break;
        case GbufferType::RoughnessMetallicAo:
            texture_info.Format = RhiFormat::R8G8B8A8Unorm;
            texture_info.Channel = Channel::Rgba;
            break;
        case GbufferType::WorldPosition:
            texture_info.Format = RhiFormat::R16G16B16A16Sfloat;
            texture_info.Channel = Channel::Rgba;
            break;
        case GbufferType::Count:
        default:
            assert(false);
        }

        frameInFlight = Texture2D(texture_info);
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
