﻿#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"
#include "vulkan_texture_sampler.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE

class VulkanTexture
{
public:
    VkImage textureImage = VK_NULL_HANDLE;

    VmaAllocation allocation = VK_NULL_HANDLE;

    VkImageView textureImageView = VK_NULL_HANDLE;

    uint32_t samplerId = -1;
    
    void Init(void const* const _data, size_t _dataSize, Tbx::Vector2i _imageSize);

    void Init(size_t _dataSize, Tbx::Vector2i _imageSize);

    void Init(VkImageCreateInfo _imageInfo , VkImageAspectFlags aspectFlags, VkImageLayout  _imageLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    void Init(std::array<void*,6> _cubeMapsData, size_t width , size_t height);
    
    void Destroy();
    
    ~VulkanTexture() = default;

private:
    
};

END_PCCORE