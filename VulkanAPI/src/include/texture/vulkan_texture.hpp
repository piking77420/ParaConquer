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
        
        VulkanTexture(const PC_CORE::CreateImageInfo& _createTextureInfo);
        
        VulkanTexture() = default;
        
        ~VulkanTexture();
    private:
        void SendDataToGpu(uint32_t _imageWidht, uint32_t _imageHeight, int32_t _imageDepth, uint32_t _layerCount, PC_CORE::Channel _channel,
            vk::Format _format, vk::ImageAspectFlags _aspectFlags, const std::vector<void*>& _datas, uint32_t _mimpLeve, vk::ImageLayout _finalImageLayout);
    };
    
}
