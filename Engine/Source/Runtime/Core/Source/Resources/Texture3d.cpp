#include "Resources/Texture3d.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Resources/FileLoader.hpp"


PC_CORE::Texture3D::Texture3D(const std::string& _name, const std::array<std::string, 6>& _maps) : Texture(_name)
{
    DYNAMIC_REFLECT_INIT

    std::vector<void*> datas;
    datas.resize(6);

    int width{};
    int height{};

    for (size_t i = 0; i < 6; i++)
        datas[i] = FileLoader::LoadImage(_maps[i].c_str(), &width, &height, &m_TextureChannel, Channel::Rgba);
    m_TextureChannel = Channel::Rgba;

    // TO DO HANDLE MIPMAP
    const CreateImageInfo createTextureInfo =
    {
        .Width = width,
        .Height = height,
        .Depth = 1,
        .LayerCount = static_cast<uint32_t>(_maps.size()),
        .MipsLevels = 1,
        .TextureType = TextureType::CubeMap,
        .Format = RhiFormat::R8G8B8A8Unorm,
        .Channel = m_TextureChannel,
        .TextureUsage = TextureUsage::Sampled,
        .MemoryVisibility = MemoryLocalisation::GpuOnly,
        .Samples = 1,
        .GenerateMipMap = true,
        .Datas = datas
    };

    m_RhiTexture3D = Rhi::CreateTexture3D(createTextureInfo);


    for (size_t i = 0; i < 6; i++)
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
}
