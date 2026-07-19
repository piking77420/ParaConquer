#pragma once

#include <vk_mem_alloc.h>
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
    
        VULKAN_API bool UploadData2D(PC_CORE::CommandList* _CommandList, const void* _ImageData, const std::vector<LevelUploadOperation>& _LevelUpload) override;
    
        VULKAN_API void UploadDataLayer(PC_CORE::CommandList* commandList, const std::vector<void*>& _imageDatas, const std::vector<std::vector<PC_CORE::RhiTexture::LevelUploadOperation>>& _LayerUploads) override;
        
        VULKAN_API bool GenerateMipMap(PC_CORE::CommandList* _CommandList, PC_CORE::Filter _Filter, RhiResourceState _StateAfterOperation) override;

        const TextureAndAlloc* GetTextureAndAlloc() const;
        
        TextureAndAlloc* GetTextureAndAlloc();

        vk::ImageViewCreateInfo GetImageViewCreateInfo(vk::ImageViewType _ViewType, uint32_t _BaseArrayLayer, uint32_t _LayerCount, uint32_t _BaseMipLevel, uint32_t _LevelCount) const;
        
        vk::ImageAspectFlags VkImageAspectFlags{};
        
        vk::Format VkFormat{};

    private:
        TextureAndAlloc m_Handle;
        
        BufferAndAlloc m_StagingBuffer{};
    };
}
