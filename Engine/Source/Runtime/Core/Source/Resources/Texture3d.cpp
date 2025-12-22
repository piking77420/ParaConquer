#include "Resources/Texture3d.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Resources/FileLoader.hpp"


PC_CORE::Texture3D::Texture3D(PC_CORE::Rhi& rhi, const std::string& _name, const std::array<std::string, 6>& _maps)
    : Texture(_name)
{
    DYNAMIC_REFLECT_INIT

    std::vector<void*> datas;
    datas.resize(6);

    int width{};
    int height{};

    RhiChannel channel;
    for (size_t i = 0; i < 6; i++)
        datas[i] = FileLoader::LoadImage(_maps[i].c_str(), &width, &height, &channel, RhiChannel::Rgba);

    const RhiTexture::RhiTextureDesciptor desc =
    {
        .Width = static_cast<uint32_t>(width),
        .Height = static_cast<uint32_t>(height),
        .Depth = 1,
        .Level = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .LayerCount = static_cast<uint32_t>(_maps.size()),
        .Samples = 1,
        .TextureType = RhiTexture::Type::CubeMap,
        .TextureUsage = static_cast<RhiTexture::TextureUsageFlagBits>(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::TransferDst),
        .RhiFormat = RhiFormat::R8G8B8A8Unorm,
        .AllowCpuAcces = false
    };

    m_RhiTexture.reset(rhi.CreateTexture(_name, desc, RhiResource::MemoryUsage::Static));
    m_RhiTexture->Build();

    rhi.PushResourceUpdate(
        [&](CommandList* list)
        {
            m_RhiTexture->UploadDataLayer(list, datas, width, height, desc.LayerCount, channel);

            for (size_t i = 0; i < 6; i++)
                FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
        }
    );

}

   
