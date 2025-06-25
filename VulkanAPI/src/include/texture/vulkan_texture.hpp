#pragma once

#include <vma/vk_mem_alloc.h>

#include "low_renderer/rhi_typedef.h"

#include "vulkan_header.h"

namespace Vulkan
{
    struct TextureAndAlloc
    {
        vk::Image image = VK_NULL_HANDLE;
        vk::ImageView imageView = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        
        DEFAULT_COPY_MOVE_OPERATIONS(TextureAndAlloc)
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(TextureAndAlloc)
    };
    
    class VulkanTexture
    {
    public:

        std::vector<TextureAndAlloc> textureAndAlloc;
        
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanTexture)
        
        VulkanTexture(PC_CORE::TextureMemoryUsage _usage);
        
        VulkanTexture() = default;
        
        ~VulkanTexture();
    private:
        
    };
    
}
