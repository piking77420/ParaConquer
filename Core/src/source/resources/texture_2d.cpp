#include "resources/texture_2d.hpp"

#include "log.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/file_loader.hpp"


using namespace PC_CORE;

Texture2D::Texture2D()
{
    DYNAMIC_REFLECT_INIT
}

Texture2D::Texture2D(const std::string& _name) : Texture(_name)
{
    DYNAMIC_REFLECT_INIT
}

Texture2D::Texture2D(const std::string& _name, const std::string& _path) : Texture(_name)
{
    DYNAMIC_REFLECT_INIT
    
    LoadFromFile(_path);
}

Texture2D::Texture2D(const CreateImageInfo& _createTextureInfo) : m_Size(_createTextureInfo.width, _createTextureInfo.height)
{
    DYNAMIC_REFLECT_INIT
    
    m_Format = _createTextureInfo.format;
    
    m_Texture2D = Rhi::CreateTexture2D(_createTextureInfo);
}


Texture2D::~Texture2D()
{
}

void Texture2D::LoadFromFile(const std::string& _path)
{
    Resource::LoadFromFile(_path);

    int width;
    int height;

    uint8_t* pixels = FileLoader::LoadFile(_path.c_str(), &width, &height, &m_TextureChannel, Channel::RGBA);
    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }
    m_Size = { width, height };

    RHIFormat format = RHIFormat::UNDEFINED;
    m_TextureChannel = Channel::RGBA;

    switch (m_TextureChannel)
    {
    case PC_CORE::Channel::RGB:
    case PC_CORE::Channel::RGBA:
        format = RHIFormat::R8G8B8A8_UNORM;
        break;
    default:
        assert("false");
        break;
    }

    const CreateImageInfo createTextureInfo =
    {
        .width = width,
        .height = height,
        .depth = 1,
        .layerCount = 1,
        .mipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .textureType = TextureType::Texture2D,
        .format = format,
        .channel = m_TextureChannel,
        .textureUsage = TextureUsage::Sampled,
        .memoryVisibility = MemoryLocalisation::GPU_Only,
        .samples = 1,
        .GenerateMipMap = true,
        .datas = {reinterpret_cast<void*>(pixels)}
    };

    m_Texture2D = Rhi::CreateTexture2D(createTextureInfo);

    FileLoader::FreeData(pixels);
}

RHIFormat Texture2D::GetRHIFormat() const
{
    return m_Format;
}
