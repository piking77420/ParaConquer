#pragma once

#include <vma/vk_mem_alloc.h>
#include "VulkanHeader.h"

#include "LowRenderer/RhiTexture.hpp"

namespace Vulkan
{
    
    struct TextureAndAlloc
    {
        vk::Image Image = VK_NULL_HANDLE;
        vk::ImageView ImageView = VK_NULL_HANDLE;
        VmaAllocation Allocation = VK_NULL_HANDLE;
        RhiResourceState resourceState = RhiResourceState::Undefined;
        
        DEFAULT_COPY_MOVE_OPERATIONS(TextureAndAlloc)
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(TextureAndAlloc)
    };

    class VulkanTexture : public PC_CORE::RhiTexture // TODO FIND A WAY TO GET RIDE OF SINGLECOMMAND
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanTexture)

        VULKAN_API VulkanTexture(PC_CORE::Rhi& _Rhi);
        
        VULKAN_API ~VulkanTexture() override;
        
        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override;
        
        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override;
        
        VULKAN_API bool Build() override;
    
        VULKAN_API void UploadData2D(PC_CORE::CommandList* commandList, const void* _imageData, uint32_t _imageWidht, uint32_t _imageHeight) override;
    
        VULKAN_API void UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _layerCount) override;
        
        VULKAN_API void GenerateMipMap(PC_CORE::CommandList* commandList) override;
        
        const TextureAndAlloc* GetTextureAndAlloc(size_t _frameIndex) const;
        
        TextureAndAlloc* GetTextureAndAlloc(size_t _frameIndex); 
        
        vk::ImageAspectFlags VkImageAspectFlags{};
        
        vk::Format VkFormat{};

    private:
        std::vector<TextureAndAlloc> m_Handles;
        
        TextureAndAlloc m_StagingBuffer{};
            
        void FreeAlloc(TextureAndAlloc& _handle);
    };
}
