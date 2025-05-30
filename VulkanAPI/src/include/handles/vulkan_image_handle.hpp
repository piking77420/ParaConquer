#pragma once
#include "vulkan_handle.hpp"


namespace Vulkan
{
    struct VulkanImageHandle : public Vulkan::VulkanHandle
    {

        VulkanImageHandle(const PC_CORE::CreateImageInfo& _createTextureInfo);
        
        VulkanImageHandle() = default;

        ~VulkanImageHandle() override;

        vk::ImageView GetImageView() const
        {
            return m_VulkanImageView;
        }

        vk::Image GetImage() const
        {
            return m_VulkanImage;
        }
        
    protected:
        VkImage m_VulkanImage = VK_NULL_HANDLE;
        VkImageView m_VulkanImageView = VK_NULL_HANDLE;

      
    };

}

