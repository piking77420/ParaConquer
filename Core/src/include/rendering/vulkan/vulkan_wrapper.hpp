#pragma once
#include <vma/vk_mem_alloc.h>

#include "vulkan_header.h"

BEGIN_PCCORE

void CreateBufferVma(VkBuffer* _vkBuffer, VmaAllocation* _allocation,VkBufferUsageFlags _vkBufferUsageFlags,
    VmaMemoryUsage _vmaMemoryUsage,void const* _data, size_t _size);

void TransitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout);

void CreateImageView(VkImage _image, VkFormat _format, VkImageView* _vkImageView);

END_PCCORE