#pragma once

#include <vma/vk_mem_alloc.h>
#include "VulkanHeader.h"

#include "LowRenderer/RhiTexture.hpp"
#include "VulkanBuffer.hpp"

namespace Vulkan
{
    
    struct TextureAndAlloc
    {
        vk::Image Image = VK_NULL_HANDLE;
        vk::ImageView ImageView = VK_NULL_HANDLE;
        VmaAllocation Allocation = VK_NULL_HANDLE;
        
        DEFAULT_COPY_MOVE_OPERATIONS(TextureAndAlloc)
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(TextureAndAlloc)
    };

    class VulkanTexture : public PC_CORE::RhiTexture
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(VulkanTexture)

        VULKAN_API VulkanTexture(PC_CORE::Rhi& _Rhi);
        
        VULKAN_API ~VulkanTexture() override;
        
        VULKAN_API bool Build() override;
    
        VULKAN_API bool UploadData2D(PC_CORE::CommandList* _CommandList, const void* _ImageData, size_t _DataSize) override;
    
        VULKAN_API void UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, uint32_t _imageWidht, uint32_t _imageHeight, uint32_t _layerCount) override;
        
        VULKAN_API bool GenerateMipMap(PC_CORE::CommandList* _CommandList, PC_CORE::Filter _Filter, RhiResourceState _StateAfterOperation) override;

        const TextureAndAlloc* GetTextureAndAlloc() const;
        
        TextureAndAlloc* GetTextureAndAlloc();
        
        vk::ImageAspectFlags VkImageAspectFlags{};
        
        vk::Format VkFormat{};

    private:
        TextureAndAlloc m_Handle;
        
        BufferAndAlloc m_StagingBuffer{};

        RhiResourceState m_ResourceState = RhiResourceState::Undefined;

        void FreeAlloc(TextureAndAlloc& _handle);
    };
}
