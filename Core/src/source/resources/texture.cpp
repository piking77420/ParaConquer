#include "resources/texture.hpp"

#include "log.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/file_loader.hpp"


using namespace PC_CORE;



Texture::Texture(const Texture& other) noexcept
{
    std::exchange(m_TextureChannel, other.m_TextureChannel);

    std::exchange(m_TextureHandles, other.m_TextureHandles);

}

Texture::Texture(Texture&& other) noexcept
{
    std::swap(m_TextureChannel, other.m_TextureChannel);
    std::swap(m_TextureHandles, other.m_TextureHandles);

    for (size_t i = 0; i < other.m_TextureHandles.size(); i++)
    {
        other.m_TextureHandles[i] = GPU_INVALID_ID;
    }

}

PC_CORE_API Texture& Texture::operator=(const Texture& other) noexcept
{
    std::exchange(m_TextureChannel, other.m_TextureChannel);

    std::exchange(m_TextureHandles, other.m_TextureHandles);
   
    return *this;
}

PC_CORE_API Texture& Texture::operator=(Texture&& other) noexcept
{
    std::swap(m_TextureChannel, other.m_TextureChannel);
    std::swap(m_TextureHandles, other.m_TextureHandles);
    return *this;
}

void Texture::Build()
{
    if (!pathToFile.empty())
        LoadFromFile((fs::path)pathToFile);

}

Texture::Texture()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        m_TextureHandles[i] = GPU_INVALID_ID;
    }
}

Texture::Texture(const CreateTextureInfo& _createTextureInfo)
{
    for (auto& texture : m_TextureHandles)
    {
        texture = Rhi::CreateTexture(_createTextureInfo);
    }
    
}

Texture::Texture(const fs::path& _path) : ResourceInterface<PC_CORE::Texture>(_path)
{
    LoadFromFile(_path);
}

Texture::~Texture()
{
    for (auto& texture : m_TextureHandles)
    {
        if (texture == GPU_INVALID_ID)
            continue;

        Rhi::DestroyGpuHandle(texture);
        texture = GPU_INVALID_ID;
    }
   
}

void Texture::CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo)
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

    const CreateTextureInfo createTextureInfo =
    {
        .width = width,
        .height = height,
        .depth = 1,
        .mipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1,
        .imageType = ImageType::TYPE_2D,
        .format = RHIFormat::R8G8B8A8_SRGB,
        .channel = Channel::RGBA,
        .textureAttachement = TextureAttachement::None,
        .textureNature = TextureNature::Default,
        .samples = 1,
        .canbeSampled = true,
        .GenerateMipMap = true,
        .data = pixels
    };

    for (auto& texture : m_TextureHandles)
    {
        texture = Rhi::CreateTexture(createTextureInfo);
    }
    
    FileLoader::FreeData(pixels);
}

void Texture::Load(const std::array<std::string, 6>& _maps)
{
  
}

GPUHandleID Texture::GetGPUHandleID(int _frameIndex)
{
    return m_TextureHandles[_frameIndex];
}
