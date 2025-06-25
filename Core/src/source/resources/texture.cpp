#include "resources/texture.hpp"

#include "log.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/file_loader.hpp"


using namespace PC_CORE;




void Texture::Build()
{
    if (!pathToFile.empty())
        LoadFromFile((fs::path)pathToFile);

}

Texture::Texture()
{
    DYNAMIC_REFLECT_INIT
}

Texture::Texture(const CreateImageInfo& _createTextureInfo)
{
    DYNAMIC_REFLECT_INIT
    
    m_Format = _createTextureInfo.format;
    
    m_Texture2D = Rhi::CreateTexture2D(_createTextureInfo);
    
}

Texture::Texture(const fs::path& _path) : Resource(_path)
{
    DYNAMIC_REFLECT_INIT
    
    LoadFromFile(_path);
}

Texture::~Texture()
{
}



void Texture::LoadFromFile(const fs::path& _path)
{
    pathToFile = _path.generic_string();

    int width;
    int height;

    uint8_t* pixels = FileLoader::LoadFile(_path.generic_string().c_str(), &width, &height, &m_TextureChannel, Channel::RGBA);
    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }


    
    const CreateImageInfo createTextureInfo =
    {
        .width = width,
        .height = height,
        .depth = 1,
        .mipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .format = RHIFormat::R8G8B8A8_SRGB,
        .channel = Channel::RGBA,
        .textureUsage = TextureUsage::Sampled | TextureUsage::TransferDst,
        .textureMemoryUsage = TextureMemoryUsage::GPU_Only,
        .samples = 1,
        .GenerateMipMap = true,
        .data = pixels
    };

   m_Texture2D = Rhi::CreateTexture2D(createTextureInfo);
    
    FileLoader::FreeData(pixels);
}


RHIFormat Texture::GetRHIFormat() const
{
    return m_Format;
}
