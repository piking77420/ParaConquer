#pragma once

#include <vma/vk_mem_alloc.h>


#include "LowRenderer/RhiTypedef.h"
#include "VulkanHeader.h"

namespace Vulkan
{
    VULKAN_API void CreateImage(VmaAllocator allocatore, uint32_t width, uint32_t height, uint32_t depth,
                                uint32_t arrayLayers, uint32_t _mimpLevel,
                                vk::SampleCountFlagBits _sampleCount, vk::ImageType _imageType, vk::Format format,
                                vk::ImageTiling tiling,
                                vk::ImageUsageFlags usage, vk::ImageCreateFlags createFlag, VmaMemoryUsage imageMemory,
                                VkImage* _outImage, VmaAllocation* _outAllocation);

    VULKAN_API vk::ImageView CreateImageView(vk::Device _device, vk::Image _image, vk::ImageViewType _imageType,
                                             vk::Format _format, vk::ImageAspectFlags imageAspect, uint32_t _layerCount,
                                             uint32_t _mipLevels);

    VULKAN_API VmaMemoryUsage GetTextureMemoryUsage(PC_CORE::MemoryLocalisation _textureMemoryUsage);


    VULKAN_API vk::ImageAspectFlags GetImageAspectFlags(PC_CORE::TextureUsage _textureUsage);

    VULKAN_API vk::ImageLayout GetImageLayout(PC_CORE::TextureUsage _textureUsage);


    VULKAN_API void GenerateMipMap(vk::CommandBuffer _commandBuffer, vk::Image image,
                                   int32_t imageWidth, int32_t imageHeight, vk::Format format, uint32_t _mipLevel,
                                   vk::ImageAspectFlags aspectFlag);

    VULKAN_API int GetMultiplayer(PC_CORE::Channel _channel);
}
