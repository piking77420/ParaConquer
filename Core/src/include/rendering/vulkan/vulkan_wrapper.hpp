#pragma once
#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"

BEGIN_PCCORE

void CreateBufferVma(VkBuffer* _vkBuffer, VmaAllocation* _allocation,VkBufferUsageFlags _vkBufferUsageFlags,
    VmaMemoryUsage _vmaMemoryUsage, size_t _size);

void TransitionImageLayout(VkImage _image, VkImageAspectFlags _aspectFlags, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _layerCount);

void CreateImageView(VkImageViewType _vkImageViewType, VkImage _image, VkFormat _format, VkImageView* _vkImageView,VkImageAspectFlags aspectFlags , uint32_t _layerCout);

void CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height,  uint32_t layer);

void CopyBufferToImage(VkBuffer _buffer, VkImage _image, const std::vector<VkBufferImageCopy>& _region);

END_PCCORE