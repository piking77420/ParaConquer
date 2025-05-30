#pragma once

#include "vulkan_handle.hpp"


namespace Vulkan
{

    struct VulkanBufferHandle : public VulkanHandle
    {
    public:

        VulkanBufferHandle() = default;

        explicit VulkanBufferHandle(const PC_CORE::GPUBufferCreateInfo& _createInfo);
        
        ~VulkanBufferHandle() override;
        
        void MapBuffer(void** _mapPtr);

        void UnMapBuffer();

        vk::Buffer GetBuffer() const
        {
            return m_VulkanBuffer;
        }
        
    private:
        VkBuffer m_VulkanBuffer = VK_NULL_HANDLE;
    };
    
}
