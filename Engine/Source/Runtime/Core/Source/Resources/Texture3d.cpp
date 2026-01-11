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


    const uint32_t Width = static_cast<uint32_t>(width);
    const uint32_t Height = static_cast<uint32_t>(height);

    m_RhiTexture.reset(rhi.CreateTexture());
    m_RhiTexture
        ->SetWidth(Width)
        .SetHeight(Height)
        .SetLayer(6)
        .SetMemoryUsage(RhiMemoryUsage::StaticGPU)
        .SetTextureType(RhiTexture::Type::CubeMap)
        .SetTextureUsage(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::TransferDst)
        .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
        .Build();

    m_RhiTexture->UploadDataLayer(nullptr, datas, width, height, m_RhiTexture->GetLayer());
    /*rhi.PushResourceUpdate(
        [&](CommandList* list)
        {
            m_RhiTexture->UploadDataLayer(list, datas, width, height, m_RhiTexture->GetLayer());

            for (size_t i = 0; i < 6; i++)
                FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
        }
    );*/

    for (size_t i = 0; i < 6; i++)
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));

}

   
