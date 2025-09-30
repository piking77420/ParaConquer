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
            .width = _size.x,
            .height = _size.y,
            .depth = 1,
            .layerCount = 1,
            .mipsLevels = 1,
            .textureType = TextureType::Texture2D,
            .format = RHIFormat::UNDEFINED,
            .channel = Channel::DEFAULT,
            .textureUsage = TextureUsage::RenderTarget | TextureUsage::Sampled,
            .memoryVisibility = MemoryLocalisation::GPU_Only,
            .samples = 1,
            .GenerateMipMap = false,
            .datas = {},
        };

        switch (gbufferType)
        {
        case GbufferType::Albedo:
            texture_info.format = RHIFormat::R16G16B16A16_SFLOAT; 
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::Normal:
            texture_info.format = RHIFormat::R16G16_SNORM;
            texture_info.channel = Channel::RGB;
            break;
        case GbufferType::RoughnessMetallicAo:
            texture_info.format = RHIFormat::R8G8B8A8_UNORM;
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::WorldPosition:
            texture_info.format = RHIFormat::R16G16B16A16_SFLOAT;
            texture_info.channel = Channel::RGBA;
            break;
        case GbufferType::Count:
        default:
            assert(false);
        }
        
        frameInFlight = Texture2D(texture_info);
        gbufferType = static_cast<GbufferType>((static_cast<int>(gbufferType) + 1) % static_cast<uint8_t>(GbufferType::Count)); 
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

