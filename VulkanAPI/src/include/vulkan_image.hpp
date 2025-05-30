#pragma once

#include <vma/vk_mem_alloc.h>

#include "low_renderer/rhi_typedef.h"
#include "vulkan_header.h"

namespace Vulkan
{
    VULKAN_API  void CreateImage(VmaAllocator allocatore, uint32_t width, uint32_t height,
                                 uint32_t depth, uint32_t _mimpLevel,
                                 vk::SampleCountFlagBits _sampleCount,
                                 vk::ImageType _imageType, vk::Format format, vk::ImageTiling tiling,
                                 vk::ImageUsageFlags usage, VmaMemoryUsage imageMemory, VkImage* _outImage, VmaAllocation* _outAllocation);

    VULKAN_API   vk::ImageView CreateImageView(vk::Device _device, vk::Image _image, vk::ImageViewType _imageType,
        vk::Format _format, vk::ImageAspectFlags imageAspect , uint32_t _mipLevels);

    VULKAN_API  void GetTextureUsage(const PC_CORE::CreateImageInfo& _createTextureInfo, VmaMemoryUsage* _memoryUsage, vk::ImageUsageFlags* _usage,
        vk::ImageLayout* _finalLoayout, vk::ImageAspectFlags* _imageAspectFlag);
    
    VULKAN_API  void GenerateMipMap(vk::CommandBuffer _commandBuffer, vk::Image image, vk::ImageAspectFlags aspectFlag , vk::Format format, int32_t imageWidth, int32_t imageHeight, uint32_t _mipLevel);
}
