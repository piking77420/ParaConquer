#include "resources/texture.hpp"

#include "log.hpp"
#include "rendering/render_harware_interface/RHI.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/file_loader.hpp"


using namespace PC_CORE;

Texture::~Texture()
{
    RHI& rhi = RHI::GetInstance();
    rhi.DestroyImageView(m_ImageViewHandle);
    rhi.DestroyImage(m_ImageHandle);
}
void Texture::SetPath(const fs::path& path)
{
    uint8_t* pixels = FileLoader::LoadFile(path.generic_string().c_str(), &textureSize.x, &textureSize.y, &textureChannel, Channel::RGBA);
    //VkDeviceSize dataImageSize=  {};
   // dataImageSize = textureSize.x * textureSize.y * 4;
    const size_t size = textureSize.x * textureSize.y * 4;

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

    
    m_ImageHandle = RHI::GetInstance().CreateImage(textureSize.x, textureSize.y, ImageType::IMAGE_2D, RHIFormat::R8G8B8A8_SRGB, ImageTiling::IMAGE_TILING_OPTIMAL,
        static_cast<RHIImageUsage>(IMAGE_USAGE_TRANSFER_DST_BIT | IMAGE_USAGE_SAMPLED_BIT));

    RHI::GetInstance().TransitionImageLayout(m_ImageHandle, IMAGE_ASPECT_COLOR_BIT, RHIFormat::R8G8B8A8_SRGB, PC_CORE::LAYOUT_UNDEFINED, PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL);
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
        .imageExtent3D = { static_cast<uint32_t>(textureSize.x), static_cast<uint32_t>(textureSize.x), 1}
     };
    
 
    RHI::GetInstance().CopyBufferToImage(stagingBuffer, m_ImageHandle, copyBufferImageInfo);
    
    RHI::GetInstance().TransitionImageLayout(m_ImageHandle, IMAGE_ASPECT_COLOR_BIT, RHIFormat::R8G8B8A8_SRGB, PC_CORE::LAYOUT_TRANSFER_DST_OPTIMAL, PC_CORE::LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    RHI::GetInstance().DestroyBuffer(stagingBuffer);

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
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
            }
        };

    m_ImageViewHandle = RHI::GetInstance().CreateImageView(imageViewCreateInfo);
    
    name = path.filename().generic_string();
    format = path.extension().generic_string();
    
}

void Texture::Load(std::array<std::string, 6>& _maps)
{
    std::array<void*, 6> datas = {};
    for (size_t i = 0; i < datas.size(); i++)
    {
        datas[i] =  FileLoader::LoadFile(_maps[i].c_str(), &textureSize.x, &textureSize.y, &textureChannel, Channel::RGBA);
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
