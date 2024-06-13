#pragma once
#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"

BEGIN_PCCORE

void CreateBufferVma(VkBuffer* _vkBuffer, VmaAllocation* _allocation,VkBufferUsageFlags _vkBufferUsageFlags,
    VmaMemoryUsage _vmaMemoryUsage, size_t _size);

void TransitionImageLayout(VkImage _image, VkImageAspectFlags _aspectFlags, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout);

void CreateImageView(VkImage _image, VkFormat _format, VkImageView* _vkImageView,VkImageAspectFlags aspectFlags);

void CopyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height);

END_PCCORE