#include "resources/texture_3d.hpp"

#include "low_renderer/rhi.hpp"
#include "resources/file_loader.hpp"


PC_CORE::Texture3D::Texture3D(const std::string& _name, const std::array<std::string, 6>& _maps) : Texture(_name)
{
    DYNAMIC_REFLECT_INIT

    std::vector<void*> datas;
    datas.resize(6);

    int width{};
    int height{};

    for (size_t i = 0; i < 6; i++)
        datas[i] = FileLoader::LoadFile(_maps[i].c_str(), &width, &height, &m_TextureChannel, Channel::RGBA);
    m_TextureChannel = Channel::RGBA;

    // TO DO HANDLE MIPMAP
    const CreateImageInfo createTextureInfo =
    {
        .width = width,
        .height = height,
        .depth = 1,
        .layerCount = static_cast<uint32_t>(_maps.size()),
        .mipsLevels = 1,
        .textureType = TextureType::CubeMap,
        .format = RHIFormat::R8G8B8A8_UNORM,
        .channel = m_TextureChannel,
        .textureUsage = TextureUsage::Sampled,
        .memoryVisibility = MemoryLocalisation::GPU_Only,
        .samples = 1,
        .GenerateMipMap = true,
        .datas = datas
    };
    
    m_RhiTexture3D = Rhi::CreateTexture3D(createTextureInfo);


    for (size_t i = 0; i < 6; i++)
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
}
