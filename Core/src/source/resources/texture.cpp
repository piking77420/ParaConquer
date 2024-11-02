#include "resources/texture.hpp"

#include "log.hpp"
#include "rendering/render_harware_interface/RHI.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/file_loader.hpp"


using namespace PC_CORE;

Texture::Texture(const CreateTextureInfo& createTextureInfo)
{
    m_TextureSize.x = .width;
    m_TextureSize.y = createTextureInfo.height;

    const uint32_t widht = static_cast<uint32_t>(m_TextureSize.x);
    const uint32_t height = static_cast<uint32_t>(m_TextureSize.y);

    m_MipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(widht, height))));

    m_ImageHandle = RHI::GetInstance().CreateImage(widht, height, m_MipLevel, ImageType::IMAGE_2D, RHIFormat::R8G8B8A8_SRGB,
        ImageTiling::IMAGE_TILING_OPTIMAL, static_cast<RHIImageUsage>(IMAGE_USAGE_TRANSFER_DST_BIT | IMAGE_USAGE_TRANSFER_SRC_BIT | IMAGE_USAGE_SAMPLED_BIT));
    RHI::GetInstance().GenerateMimpMap(m_ImageHandle, RHIFormat::R8G8B8A8_SRGB, m_TextureSize.x, m_TextureSize.y, m_MipLevel);
    
    const ImageViewCreateInfo imageViewCreateInfo =
    {
    .flags = {},
    .image = m_ImageHandle,
    .viewType = ImageViewType::e2D,
    .format = RHIFormat::R8G8B8A8_SRGB,
    .components = {},
    .subresourceRange =
        {
        .aspectMask = IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = m_MipLevel,
        .baseArrayLayer = 0,
        .layerCount = 1
        }
    };
    m_ImageViewHandle = RHI::GetInstance().CreateImageView(imageViewCreateInfo);
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

Texture::Texture(const fs::path& _path) : Resource(_path)
{
    CreateTextureFromFile(_path);
}

Texture::~Texture()
{
    RHI& rhi = RHI::GetInstance();
    if (m_ImageViewHandle != NULL_HANDLE)
        rhi.DestroyImageView(m_ImageViewHandle);
    
    if (m_ImageHandle != NULL_HANDLE)
        rhi.DestroyImage(m_ImageHandle);
    
    if (m_SamplerHandle != NULL_HANDLE)
        rhi.DestroySampler(m_SamplerHandle);
}

void Texture::Load(std::array<std::string, 6>& _maps)
{
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] =  FileLoader::LoadFile(_maps[i].c_str(), &m_TextureSize.x, &m_TextureSize.y, &textureChannel, Channel::RGBA);
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

void Texture::CreateTexture()
{
}

PC_CORE::ImageHandle Texture::GetImageHandle()
{
    return m_ImageHandle;
}

PC_CORE::ImageHandle Texture::GetImageViewHandle()
{
    return m_ImageViewHandle;
}

PC_CORE::ImageHandle Texture::GetSamplerHandle()
{
    return m_SamplerHandle;
}

Tbx::Vector2i Texture::GetTextureSize() const
{
    return m_TextureSize;
}

void Texture::CreateTextureFromFile(const fs::path& _path)
{
    uint8_t* pixels = FileLoader::LoadFile(path.generic_string().c_str(), &m_TextureSize.x, &m_TextureSize.y, &textureChannel, Channel::RGBA);
    const size_t size = m_TextureSize.x * m_TextureSize.y * 4;

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

    const uint32_t widht = static_cast<uint32_t>(m_TextureSize.x);
    const uint32_t height = static_cast<uint32_t>(m_TextureSize.y);

    m_MipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(widht, height))));

    m_ImageHandle = RHI::GetInstance().CreateImage(widht, height, m_MipLevel, ImageType::IMAGE_2D, RHIFormat::R8G8B8A8_SRGB,
        ImageTiling::IMAGE_TILING_OPTIMAL, static_cast<RHIImageUsage>(IMAGE_USAGE_TRANSFER_DST_BIT | IMAGE_USAGE_TRANSFER_SRC_BIT | IMAGE_USAGE_SAMPLED_BIT));

    RHI::GetInstance().TransitionImageLayout(m_ImageHandle, IMAGE_ASPECT_COLOR_BIT, RHIFormat::R8G8B8A8_SRGB, m_MipLevel, PC_CORE::LAYOUT_UNDEFINED, PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL);
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
       .imageExtent3D = { widht, height, 1}
    };


    RHI::GetInstance().CopyBufferToImage(stagingBuffer, m_ImageHandle, copyBufferImageInfo);
    RHI::GetInstance().DestroyBuffer(stagingBuffer);
    RHI::GetInstance().GenerateMimpMap(m_ImageHandle, RHIFormat::R8G8B8A8_SRGB, m_TextureSize.x, m_TextureSize.y, m_MipLevel);


    const ImageViewCreateInfo imageViewCreateInfo =
    {
    .flags = {},
    .image = m_ImageHandle,
    .viewType = ImageViewType::e2D,
    .format = RHIFormat::R8G8B8A8_SRGB,
    .components = {},
    .subresourceRange =
        {
        .aspectMask = IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = m_MipLevel,
        .baseArrayLayer = 0,
        .layerCount = 1
        }
    };

    m_ImageViewHandle = RHI::GetInstance().CreateImageView(imageViewCreateInfo);

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
    