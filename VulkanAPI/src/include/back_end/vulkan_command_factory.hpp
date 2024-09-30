#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    class VulkanCommandFactory
    {
    public:
    
        void CreateCommandBuffer(vk::CommandBuffer* _buffers, uint32_t _bufferCount, vk::CommandBufferLevel _commandBufferLevel);

        
        VulkanCommandFactory(uint32_t _queuFamliesIndicies, vk::CommandPoolCreateFlagBits _createFlagBits);
        
        VulkanCommandFactory& operator=(const VulkanCommandFactory&& _vulkanCommandFactory) noexcept;
        
        VulkanCommandFactory() = default;
        
        ~VulkanCommandFactory();

    private:
        vk::CommandPool m_CommandPool;

        vk::Device m_Device;
    };

}

