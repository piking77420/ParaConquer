#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
    class VulkanTexture : public VulkanBuffer
{
public:

    VkImage textureImage = VK_NULL_HANDLE;
    
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    
    void Init(void const* const _data, size_t _dataSize, Vector2ui _imageSize);
    
    void Destroy() override;
    
    ~VulkanTexture() override;

private:
    
};

END_PCCORE