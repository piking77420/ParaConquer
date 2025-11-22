#include "Resources/Texture2d.hpp"

#include "Log.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Resources/FileLoader.hpp"


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

    LoadTextureFromPath(_path);
}

Texture2D::Texture2D(const CreateImageInfo& _createTextureInfo) : m_Size(
    _createTextureInfo.Width, _createTextureInfo.Height)
{
    DYNAMIC_REFLECT_INIT

    m_Format = _createTextureInfo.Format;

    m_Texture2D = Rhi::CreateTexture2D(_createTextureInfo);
}


Texture2D::~Texture2D()
{
}

void Texture2D::AfterSerialize(Serializer* _serializer) const
{
}

void Texture2D::AfterDeSerialize(Serializer* _serializer)
{
    _serializer->DeSerializeStream(m_Texture2DMetaData);


    if (!m_Texture2DMetaData.data.empty() && m_Format != RhiFormat::Undefined
        && m_Size != Tbx::Vector2i::Zero() && m_TextureChannel != Channel::Default)
    {
        PC_LOG("Create Texture");

        const CreateImageInfo createTextureInfo =
        {
            .Width = m_Size.x,
            .Height = m_Size.y,
            .Depth = 1,
            .LayerCount = 1,
            .MipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_Size.x, m_Size.y)))) + 1,
            .TextureType = TextureType::Texture2D,
            .Format = m_Format,
            .Channel = m_TextureChannel,
            .TextureUsage = TextureUsage::Sampled,
            .MemoryVisibility = MemoryLocalisation::GpuOnly,
            .Samples = 1,
            .GenerateMipMap = true,
            .Datas = {reinterpret_cast<void*>(m_Texture2DMetaData.data.data())}
        };

        m_Texture2D = Rhi::CreateTexture2D(createTextureInfo);
        m_Texture2DMetaData.data.clear();
    }
}

void Texture2D::LoadTextureFromPath(const std::string& _path)
{
    int width;
    int height;

    uint8_t* pixels = FileLoader::LoadImage(_path.c_str(), &width, &height, &m_TextureChannel, Channel::Rgba);
    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }
    m_Size = {width, height};

    auto format = RhiFormat::Undefined;
    m_TextureChannel = Channel::Rgba;

    switch (m_TextureChannel)
    {
    case Channel::Rgb:
    case Channel::Rgba:
        format = RhiFormat::R8G8B8A8Unorm;
        break;
    default:
        assert("false");
        break;
    }

    const CreateImageInfo createTextureInfo =
    {
        .Width = width,
        .Height = height,
        .Depth = 1,
        .LayerCount = 1,
        .MipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .TextureType = TextureType::Texture2D,
        .Format = format,
        .Channel = m_TextureChannel,
        .TextureUsage = TextureUsage::Sampled,
        .MemoryVisibility = MemoryLocalisation::GpuOnly,
        .Samples = 1,
        .GenerateMipMap = true,
        .Datas = {reinterpret_cast<void*>(pixels)}
    };

    m_Texture2D = Rhi::CreateTexture2D(createTextureInfo);

    FileLoader::FreeData(pixels);
}

RhiFormat Texture2D::GetRhiFormat() const
{
    return m_Format;
}
