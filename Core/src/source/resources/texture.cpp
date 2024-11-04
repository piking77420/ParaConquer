#include "resources/texture.hpp"

#include "log.hpp"
#include "rendering/render_harware_interface/RHI.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/file_loader.hpp"


using namespace PC_CORE;


Texture& Texture::operator=(Texture&& _other)
{
    Destroy();

    m_ImageHandle = _other.m_ImageHandle;
    _other.m_ImageHandle = nullptr;

    m_ImageViewHandle = _other.m_ImageViewHandle;
    _other.m_ImageViewHandle = nullptr;

    m_SamplerHandle = _other.m_SamplerHandle;
    _other.m_SamplerHandle = nullptr;

    textureChannel = _other.textureChannel;
    _other.textureChannel = 0;

    m_MipLevel = _other.m_MipLevel;
    _other.m_MipLevel = 0;

    m_TextureSize = _other.m_TextureSize;
    _other.m_TextureSize = {};
    
    
    return *this;
}


Texture::Texture(const CreateTextureInfo& createTextureInfo)
{
    CreateFromCreateInfo(createTextureInfo);
}

Texture::Texture(const fs::path& _path) : Resource(_path)
{
    CreateTextureFromFile(_path);
}

Texture::~Texture()
{
    Destroy();
}

void Texture::Load(std::array<std::string, 6>& _maps)
{
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] = FileLoader::LoadFile(_maps[i].c_str(), &m_TextureSize.x, &m_TextureSize.y, &textureChannel, Channel::RGBA);
    }
    //vulkanTexture.Init(datas, textureSize.x , textureSize.y);
    for (size_t i = 0; i < datas.size(); i++)
    {
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
    }

    const fs::path path(_maps[0]);

    name = path.filename().generic_string();
    format = path.extension().generic_string();
}

PC_CORE_API void Texture::Reset(const CreateTextureInfo& createTextureInfo)
{
    Destroy();
    CreateFromCreateInfo(createTextureInfo);
}



PC_CORE::ImageHandle Texture::GetImageHandle() const
{
    return m_ImageHandle;
}

PC_CORE::ImageHandle Texture::GetImageViewHandle() const
{
    return m_ImageViewHandle;
}

PC_CORE::ImageHandle Texture::GetSamplerHandle() const
{
    return m_SamplerHandle;
}

Tbx::Vector2i Texture::GetTextureSize() const
{
    return m_TextureSize;
}

DescriptorImageInfo Texture::GetDescriptorImageInfo() const
{
    return {.sampler = GetSamplerHandle(), .imageView = GetImageViewHandle(), .imageLayout = ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL};
}

void Texture::CreateTextureFromFile(const fs::path& _path)
{
    uint8_t* pixels = FileLoader::LoadFile(path.generic_string().c_str(), &m_TextureSize.x, &m_TextureSize.y, &textureChannel, Channel::RGBA);
    const size_t size = m_TextureSize.x * m_TextureSize.y * 4;
    m_Format = RHIFormat::R8G8B8A8_SRGB;

    PC_CORE::GPUBufferHandle stagingBuffer = RHI::GetInstance().BufferData(size, pixels, GPU_BUFFER_USAGE::BUFFER_USAGE_TRANSFER_SRC_BIT);

    if (!pixels)
    {
        PC_LOGERROR("failed to load texture image!");
        throw std::runtime_error("failed to load texture image!");
    }

    void* gpuData;
    RHI::GetInstance().MapData(stagingBuffer, &gpuData);
    memcpy(gpuData, pixels, size);
    RHI::GetInstance().UnMapData(stagingBuffer);
    FileLoader::FreeData(pixels);
    

    m_MipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(m_TextureSize.x, m_TextureSize.y))));

    PC_CORE::CreateTextureInfo createTextureInfo =
    {
        .width = m_TextureSize.x,
        .height = m_TextureSize.y,
        .depth = 1,
        .mipsLevels = m_MipLevel,
        .data = {gpuData},
        .imageType = ImageType::TYPE_2D,
        .format = m_Format,
        .textureAspect = TextureAspect::COLOR,
    };

    RHI::GetInstance().CreateTexture(createTextureInfo, &m_ImageHandle, &m_ImageViewHandle);
    RHI::GetInstance().TransitionImageLayout(m_ImageHandle, IMAGE_ASPECT_COLOR_BIT, m_MipLevel, PC_CORE::LAYOUT_UNDEFINED, PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL);
    
    const CopyBufferImageInfo copyBufferImageInfo =
    {
       .bufferOffset = 0,
       .bufferRowLength = 0,
       .bufferImageHeight = 0,
       .imageSubresource =
       {
           .aspectMask = IMAGE_ASPECT_COLOR_BIT,
           .mipLevel = 0,
           .baseArrayLayer = 0,
           .layerCount = 1
       },
       .imageOffset3D = {0, 0, 0 },
       .imageExtent3D = { static_cast<uint32_t>(m_TextureSize.x), static_cast<uint32_t>(m_TextureSize.y), 1}
    };
    RHI::GetInstance().CopyBufferToImage(stagingBuffer, m_ImageHandle, copyBufferImageInfo);
    RHI::GetInstance().DestroyBuffer(stagingBuffer);
    RHI::GetInstance().GenerateMimpMap(m_ImageHandle, m_Format, m_TextureSize.x, m_TextureSize.y, m_MipLevel);

    const SamplerCreateInfo samplerCreateInfo =
    {
      .flags = {},
      .magFilter = Filter::LINEAR,
      .minFilter = Filter::LINEAR,
      .mipmapMode = {},
      .addressModeU = SamplerAddressMode::REPEAT,
      .addressModeV = SamplerAddressMode::REPEAT,
      .addressModeW = SamplerAddressMode::REPEAT,
      .mipLodBias = 1,
      .anisotropyEnable = true,
      .compareEnable = false,
      .compareOp = CompareOp::ALWAYS,
      .minLod = 0,
      .maxLod = static_cast<float>(m_MipLevel),
      .borderColor = BorderColor::INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = false
    };

    m_SamplerHandle = RHI::GetInstance().CreateSampler(samplerCreateInfo);
}

void Texture::Destroy()
{
    RHI& rhi = RHI::GetInstance();
    if (m_ImageViewHandle != NULL_HANDLE)
    {
        rhi.DestroyImageView(m_ImageViewHandle);
        m_ImageViewHandle = NULL_HANDLE;
    }

    if (m_ImageHandle != NULL_HANDLE)
    {
        rhi.DestroyTexture(m_ImageHandle);
        m_ImageHandle = NULL_HANDLE;

    }

    if (m_SamplerHandle != NULL_HANDLE)
    {
        rhi.DestroySampler(m_SamplerHandle);
        m_SamplerHandle = NULL_HANDLE;
    }
}

void Texture::CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo)
{
    m_TextureSize.x = createTextureInfo.width;
    m_TextureSize.y = createTextureInfo.height;
    m_Format = createTextureInfo.format;

    const uint32_t widht = static_cast<uint32_t>(m_TextureSize.x);
    const uint32_t height = static_cast<uint32_t>(m_TextureSize.y);
    //m_MipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(widht, height))));
    m_MipLevel = createTextureInfo.mipsLevels;

    RHI::GetInstance().CreateTexture(createTextureInfo, &m_ImageHandle, &m_ImageViewHandle);

    RHI::GetInstance().TransitionImageLayout(m_ImageHandle, IMAGE_ASPECT_COLOR_BIT, m_MipLevel, PC_CORE::LAYOUT_UNDEFINED, PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL);
    RHI::GetInstance().GenerateMimpMap(m_ImageHandle, m_Format, m_TextureSize.x, m_TextureSize.y, m_MipLevel);

    const SamplerCreateInfo samplerCreateInfo =
    {
      .flags = {},
      .magFilter = Filter::LINEAR,
      .minFilter = Filter::LINEAR,
      .mipmapMode = {},
      .addressModeU = SamplerAddressMode::REPEAT,
      .addressModeV = SamplerAddressMode::REPEAT,
      .addressModeW = SamplerAddressMode::REPEAT,
      .mipLodBias = 1,
      .anisotropyEnable = true,
      .compareEnable = false,
      .compareOp = CompareOp::ALWAYS,
      .minLod = 0,
      .maxLod = static_cast<float>(m_MipLevel),
      .borderColor = BorderColor::INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = false
    };

    m_SamplerHandle = RHI::GetInstance().CreateSampler(samplerCreateInfo);
}
    