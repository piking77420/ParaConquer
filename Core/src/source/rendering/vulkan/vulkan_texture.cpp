﻿#include "rendering/vulkan/vulkan_texture.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanTexture::Init(void const* const _data, size_t _dataSize , Vector2i _imageSize)
{
    VkImageCreateInfo imgCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.extent.width = _imageSize.x;
    imgCreateInfo.extent.height = _imageSize.y;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
 
    VmaAllocationCreateInfo allocCreateInfo = {}; VMA_ALLOCATION_CREATE_HOST_ACCESS
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocCreateInfo.priority = 1.0f;

    
    const VkResult result = vmaCreateImage(VulkanInterface::GetAllocator(), &imgCreateInfo, &allocCreateInfo, &textureImage, &allocation, nullptr);
    VK_CHECK_ERROR(result,"vmaCreateImage failed on create image")
    
    
    VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufCreateInfo.size = _dataSize;
    bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
 
    VmaAllocationCreateInfo buffAllocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
        VMA_ALLOCATION_CREATE_MAPPED_BIT;
    
    vmaCopyMemoryToAllocation(VulkanInterface::GetAllocator(),_data, allocation,0, _dataSize);

    
    CreateImageView(textureImage,VK_FORMAT_R8G8B8A8_UNORM, &textureImageView);

    vulkanTextureSampler.Init();
}

void VulkanTexture::Destroy()
{
    vulkanTextureSampler.Destroy();
    vmaDestroyImage(VulkanInterface::GetAllocator(), textureImage, allocation);
    vkDestroyImageView(VulkanInterface::GetDevice().device, textureImageView, nullptr);
}
