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

    const uint32_t Width = static_cast<uint32_t>(_size.x);
    const uint32_t Height = static_cast<uint32_t>(_size.y);

    for (auto& texture : gbuffers)
    {
        RhiFormat format{};
        switch (gbufferType)
        {
        case GbufferType::AlbedoFactors:
            format = RhiFormat::R16G16B16A16Sfloat;
            break;
        case GbufferType::Normal:
            format = RhiFormat::R16G16Snorm;
            break;
        case GbufferType::RoughnessMetallicAo:
            format = RhiFormat::R8G8B8A8Unorm;
            break;
        case GbufferType::WorldPosition:
            format = RhiFormat::R16G16B16A16Sfloat;
            break;
        case GbufferType::Count:
        default:
            assert(false);
        }

        texture.reset(_Rhi.CreateTexture());
        texture
            ->SetWidth(Width)
            .SetHeight(Height)
            .SetRhiFormat(format)
            .SetTextureUsage(RhiTexture::RenderTarget | RhiTexture::Sampled)
            .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetName("Gbuffer" + GbufferTypeToString(gbufferType))
            .Build();

        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % static_cast<uint8_t>(
            GbufferType::Count));
    }
}

PC_CORE::RhiTexture& PC_CORE::Gbuffers::GetTexture(GbufferType _type)
{
    const size_t index = static_cast<size_t>(_type);
    assert(gbuffers.size() <= index);
    return *gbuffers[index];
}

const PC_CORE::RhiTexture& PC_CORE::Gbuffers::GetTexture(GbufferType _type) const
{
    const size_t index = static_cast<size_t>(_type);
    assert(gbuffers.size() <= index);
    return *gbuffers[index];
}
