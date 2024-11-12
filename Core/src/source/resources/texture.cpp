#include "resources/texture.hpp"

#include "log.hpp"
#include "rendering/render_harware_interface/RHI.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/file_loader.hpp"


using namespace PC_CORE;



Texture::Texture(const CreateTextureInfo& createTextureInfo)
{
    CreateFromCreateInfo(createTextureInfo);
    CreateMimmap();
    CreateSampler();
}

Texture::Texture(const fs::path& _path) : Resource(_path)
{
    int width;
    int height;
    
    uint8_t* pixels = FileLoader::LoadFile(path.generic_string().c_str(), &width, &height, &textureChannel, Channel::RGBA);
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
        .mipsLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))),
        .imageType = ImageType::TYPE_2D,
        .format = RHIFormat::R8G8B8A8_SRGB,
        .textureAspect = TextureAspect::COLOR,
        .GenerateMipMap = true,
        .useAsAttachement = false
        };
    
    CreateFromCreateInfo(createTextureInfo);
    RHI::GetInstance().UploadData2D(m_ImageHandle, m_MipLevel, TextureAspect::COLOR, pixels, static_cast<uint32_t>(m_TextureSize.x), static_cast<uint32_t>(m_TextureSize.y), Channel::RGBA);
    CreateMimmap();
    CreateSampler();
}

Texture::~Texture()
{
    Destroy();
}

void Texture::CreateFromCreateInfo(const CreateTextureInfo& createTextureInfo)
{
    m_TextureSize.x = createTextureInfo.width;
    m_TextureSize.y = createTextureInfo.height;
    m_Format = createTextureInfo.format;
    m_MipLevel = createTextureInfo.GenerateMipMap ? createTextureInfo.mipsLevels : 1;
    m_Depth = createTextureInfo.depth;
    RHI::GetInstance().CreateTexture(createTextureInfo, &m_ImageHandle, &m_ImageViewHandle);
}

void Texture::Load(const std::array<std::string, 6>& _maps)
{
    LoadCubeMap(_maps);
    name = path.filename().generic_string();
    format = path.extension().generic_string();
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



void Texture::LoadCubeMap(const std::array<std::string, 6>& _maps)
{
    /*
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] = FileLoader::LoadFile(_maps[i].c_str(), &m_TextureSize.x, &m_TextureSize.y, &textureChannel, Channel::RGBA);
    }

    const size_t size = m_TextureSize.x * m_TextureSize.y * 4;
    const size_t layerSize = size / 6;
    m_Format = RHIFormat::R8G8B8A8_SRGB;
    PC_CORE::GPUBufferHandle stagingBuffer = RHI::GetInstance().BufferData(size, GPU_BUFFER_USAGE::TRANSFERT_SRC);
    
    void* gpuData;
    RHI::GetInstance().MapData(stagingBuffer, &gpuData);

    for (size_t i = 0; i < _maps.size(); i++)
    {
        memcpy(gpuData + (layerSize * i), datas[i], layerSize);
    }
    RHI::GetInstance().UnMapData(stagingBuffer);

    PC_CORE::CreateTextureInfo createTextureInfo =
    {
        .width = m_TextureSize.x,
        .height = m_TextureSize.y,
        .depth = 1,
        .mipsLevels = m_MipLevel,
        .imageType = ImageType::TYPE_CUBE,
        .format = m_Format,
        .textureAspect = TextureAspect::COLOR,
    };
    RHI::GetInstance().CreateTexture(createTextureInfo, &m_ImageHandle, &m_ImageViewHandle);
    
    
    for (size_t i = 0; i < datas.size(); i++)
    {
        FileLoader::FreeData(static_cast<uint8_t*>(datas[i]));
    }

    const fs::path path(_maps[0]);*/
}

void Texture::CreateMimmap()
{
    RHI::GetInstance().GenerateMimpMap(m_ImageHandle, m_Format, m_TextureSize.x, m_TextureSize.y, m_MipLevel);
}

void Texture::CreateSampler()
{
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
    