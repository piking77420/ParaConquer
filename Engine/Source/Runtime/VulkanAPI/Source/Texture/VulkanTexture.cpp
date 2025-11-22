#include "Texture/VulkanTexture.hpp"

#include <Memory>


#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "Buffer/VulkanBuffer.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Utils/HelperFunctions.hpp"
#include "Utils/RhiToVulkan.hpp"
#include "Utils/TransitionImageLayout.hpp"
#include "Utils/VulkanBufferHelper.hpp"


Vulkan::VulkanTexture::VulkanTexture(const PC_CORE::CreateImageInfo& _createTextureInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    textureAndAlloc.resize(MaxFramesInFlight);

    auto& context = VulkanContext::GetContext();
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();

    const vk::Format format = Utils::RhiFormatToVkFormat(_createTextureInfo.Format);
    const uint32_t mipLevel = _createTextureInfo.GenerateMipMap ? _createTextureInfo.MipsLevels : 1;
    const vk::SampleCountFlagBits sampleCount = Utils::RhiSampleCountToVuklan(_createTextureInfo.Samples);
    const VmaMemoryUsage vmaMemoryUsage = GetTextureMemoryUsage(_createTextureInfo.MemoryVisibility);
    const vk::ImageAspectFlags aspectFlags = GetImageAspectFlags(_createTextureInfo.TextureUsage);
    const vk::ImageType imageType = Utils::RhiImageToVkImageType(_createTextureInfo.TextureType);
    const vk::ImageViewType imageViewType = Utils::RhiImageToVkImageViewType(_createTextureInfo.TextureType);

    vk::ImageUsageFlags textureUsage = Utils::GetImageUsageFlags(_createTextureInfo.TextureUsage);
    textureUsage = mipLevel > 1 ? textureUsage | vk::ImageUsageFlagBits::eTransferSrc : textureUsage;
    textureUsage = _createTextureInfo.Datas.size() != 0
                       ? (textureUsage | vk::ImageUsageFlagBits::eTransferDst)
                       : textureUsage;

    vk::ImageLayout finalImageLayout = GetImageLayout(_createTextureInfo.TextureUsage);

    const vk::ImageCreateFlags imageCreateFlag = Utils::ImageCreateFlagFromTextureType(_createTextureInfo.TextureType);

    for (size_t i = 0; i < textureAndAlloc.size(); i++)
    {
        CreateImage(context.allocator, _createTextureInfo.Width
                    , _createTextureInfo.Height, _createTextureInfo.Depth, _createTextureInfo.LayerCount,
                    _createTextureInfo.MipsLevels,
                    sampleCount,
                    imageType,
                    format, vk::ImageTiling::eOptimal,
                    textureUsage, imageCreateFlag, vmaMemoryUsage,
                    reinterpret_cast<VkImage*>(&textureAndAlloc[i].image), &textureAndAlloc[i].allocation);
    }


    if (_createTextureInfo.Datas.empty())
    {
        const Utils::SingleCommandBeginInfo singleCommandBeginInfo =
        {
            .device = device,
            .commandPool = context.transferCommandPool,
            .queue = context.mainQueue
        };
        vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);


        for (size_t i = 0; i < textureAndAlloc.size(); i++)
        {
            if (mipLevel != 1)
            {
                //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

                GenerateMipMap(commandBuffer, textureAndAlloc[i].image, _createTextureInfo.Width,
                               _createTextureInfo.Height, format, mipLevel, aspectFlags);
                TransitionImageLayout(commandBuffer, textureAndAlloc[i].image, format,
                                      vk::ImageLayout::eShaderReadOnlyOptimal, finalImageLayout, aspectFlags,
                                      _createTextureInfo.LayerCount, mipLevel);
            }
            else
            {
                TransitionImageLayout(commandBuffer, textureAndAlloc[i].image, format, vk::ImageLayout::eUndefined,
                                      finalImageLayout, aspectFlags, _createTextureInfo.LayerCount, mipLevel);
            }
        }
        EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);
    }
    else
    {
        SendDataToGpu(_createTextureInfo.Width, _createTextureInfo.Height, _createTextureInfo.Depth,
                      _createTextureInfo.LayerCount, _createTextureInfo.Channel, format, aspectFlags,
                      _createTextureInfo.Datas, mipLevel, finalImageLayout);
    }


    for (auto& textureAndAlloc : textureAndAlloc)
        textureAndAlloc.imageView = CreateImageView(device, textureAndAlloc.image, imageViewType, format, aspectFlags,
                                                    _createTextureInfo.LayerCount, mipLevel);
}

Vulkan::VulkanTexture::~VulkanTexture()
{
    for (auto& textureAndAlloc : textureAndAlloc)
    {
        if (textureAndAlloc.allocation == VK_NULL_HANDLE || textureAndAlloc.image == VK_NULL_HANDLE)
            return;

        auto& context = VulkanContext::GetContext();
        vk::Device device = context.GetDevice()->GetDevice();

        device.destroyImageView(textureAndAlloc.imageView);
        vmaDestroyImage(context.allocator, textureAndAlloc.image, textureAndAlloc.allocation);

        textureAndAlloc.allocation = VK_NULL_HANDLE;
        textureAndAlloc.imageView = VK_NULL_HANDLE;
        textureAndAlloc.image = VK_NULL_HANDLE;
    }
}

void Vulkan::VulkanTexture::SendDataToGpu(uint32_t _imageWidht, uint32_t _imageHeight, int32_t _imageDepth,
                                          uint32_t _layerCount, PC_CORE::Channel _channel,
                                          vk::Format _format, const vk::ImageAspectFlags _aspectFlags,
                                          const std::vector<void*>& _datas, uint32_t _mimpLevel,
                                          vk::ImageLayout _finalImageLayout)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    auto& context = VulkanContext::GetContext();
    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(PC_CORE::Rhi::GetRhiContext()->rhiDevice)->
        GetDevice();
    const Utils::SingleCommandBeginInfo singleCommandBeginInfo =
    {
        .device = device,
        .commandPool = context.transferCommandPool,
        .queue = context.mainQueue
    };
    vk::CommandBuffer commandBuffer = BeginSingleTimeCommand(singleCommandBeginInfo);

    const int multiplayer = GetMultiplayer(_channel);
    const size_t imageSize = _imageWidht * _imageHeight * _imageDepth * _layerCount * multiplayer;
    const size_t layerSize = imageSize / _layerCount;


    std::vector<BufferAndAlloc> bufferAndAllocs;
    bufferAndAllocs.resize(textureAndAlloc.size());

    std::vector<void*> mappedData;
    mappedData.resize(textureAndAlloc.size());

    for (size_t i = 0; i < textureAndAlloc.size(); i++)
    {
        auto buffPtr = reinterpret_cast<VkBuffer*>(&bufferAndAllocs[i].buffer);
        Utils::CreateBuffer(context.allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                            VMA_MEMORY_USAGE_CPU_TO_GPU,
                            buffPtr, &bufferAndAllocs[i].alloc);

        vmaMapMemory(context.allocator, bufferAndAllocs[i].alloc, &mappedData[i]);

        for (size_t j = 0; j < _datas.size(); j++)
            memcpy(static_cast<uint8_t*>(mappedData[i]) + (layerSize * j), _datas[j], layerSize);

        vmaUnmapMemory(context.allocator, bufferAndAllocs[i].alloc);
    }

    for (size_t i = 0; i < textureAndAlloc.size(); i++)
    {
        const uint32_t w = _imageWidht;
        const uint32_t h = _imageHeight;
        const uint32_t d = _imageDepth;
        TransitionImageLayout(commandBuffer, textureAndAlloc[i].image, _format, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal, _aspectFlags, _layerCount, _mimpLevel);

        // for each layer
        for (size_t j = 0; j < _layerCount; j++)
        {
            vk::BufferImageCopy region{};
            region.bufferOffset = (layerSize * j);
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = _aspectFlags;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = static_cast<uint32_t>(j);
            region.imageSubresource.layerCount = 1;
            region.imageOffset = VkOffset3D{0, 0, 0};
            region.imageExtent = vk::Extent3D{
                w,
                h,
                d
            };
            commandBuffer.copyBufferToImage(bufferAndAllocs[i].buffer, textureAndAlloc[i].image,
                                            vk::ImageLayout::eTransferDstOptimal, region);
        }

        if (_mimpLevel != 1)
        {
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
            GenerateMipMap(commandBuffer, textureAndAlloc[i].image, static_cast<int32_t>(_imageWidht),
                           static_cast<int32_t>(_imageHeight), _format, _mimpLevel, _aspectFlags);
        }
        else
        {
            TransitionImageLayout(commandBuffer, textureAndAlloc[i].image, _format,
                                  vk::ImageLayout::eTransferDstOptimal, _finalImageLayout, _aspectFlags, _layerCount,
                                  _mimpLevel);
        }
    }


    EndSingleTimeCommand(commandBuffer, singleCommandBeginInfo, context.transferFence);

    // Single command ended usless buffer now
    for (auto& it : bufferAndAllocs)
    {
        vmaDestroyBuffer(context.allocator, it.buffer, it.alloc);
    }
}
