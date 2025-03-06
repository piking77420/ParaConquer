#include "resources/texture.hpp"

#include "log.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/file_loader.hpp"


using namespace PC_CORE;



Texture::Texture(const CreateTextureInfo& createTextureInfo)
{
    for (auto& texture : m_TextureHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateTexture(createTextureInfo, &texture))
        {
            PC_LOGERROR("failed to create texture!");
        }    
    }
    
}

Texture::Texture(const fs::path& _path) : ResourceInterface<PC_CORE::Texture>(_path)
{
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
        .canbeSampled = true,
        .GenerateMipMap = true,
        .data = pixels
    };

    for (auto& texture : m_TextureHandles)
    {
        if (!Rhi::GetRhiContext()->gpuAllocator->CreateTexture(createTextureInfo, &texture))
        {
            PC_LOGERROR("failed to create texture!");
        }    
    }
    

    FileLoader::FreeData(pixels);
}

Texture::~Texture()
{
    for (auto& texture : m_TextureHandles)
    {
        if (texture == nullptr)
            return;
    
        if (!Rhi::GetRhiContext()->gpuAllocator->DestroyTexture(&texture))
        {
            PC_LOGERROR("failed to destroy texture!");
        }
    }
   
}

void Texture::CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo)
{
    
}

void Texture::Load(const std::array<std::string, 6>& _maps)
{
  
}

std::shared_ptr<GpuHandle> Texture::GetHandle() const
{
    return m_TextureHandles[Rhi::GetFrameIndex()];
}

std::shared_ptr<GpuHandle> Texture::GetHandle(size_t _frameIndex) const
{
    return m_TextureHandles[_frameIndex];
}
