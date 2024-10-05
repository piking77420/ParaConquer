#include "back_end/vulkan_command_factory.hpp"

using namespace VK_NP;

void VulkanCommandFactory::AllocCommandBuffer(vk::CommandBuffer* _buffers, uint32_t _bufferCount, vk::CommandBufferLevel _commandBufferLevel)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocInfo.level = _commandBufferLevel;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = _bufferCount;
    
   VK_CALL(m_Device.allocateCommandBuffers(&allocInfo, _buffers));
}



VulkanCommandFactory::VulkanCommandFactory(uint32_t _queuFamliesIndicies, vk::CommandPoolCreateFlagBits _createFlagBits)
{
    m_Device = VK_NP::VulkanContext::currentContext->device;
    
    vk::CommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.queueFamilyIndex = _queuFamliesIndicies;
    commandPoolCreateInfo.flags = _createFlagBits;

    
    m_CommandPool = m_Device.createCommandPool(commandPoolCreateInfo, nullptr);
    
}

VulkanCommandFactory& VulkanCommandFactory::operator=(const VulkanCommandFactory&& _vulkanCommandFactory)
 noexcept
{
    m_CommandPool = _vulkanCommandFactory.m_CommandPool;
    m_Device = _vulkanCommandFactory.m_Device;
    return *this;
}


VulkanCommandFactory::~VulkanCommandFactory()
{
    if (m_CommandPool != VK_NULL_HANDLE)
    {
        //m_Device.destroyCommandPool(m_CommandPool, nullptr);
        m_CommandPool = VK_NULL_HANDLE;
    }

    m_Device = VK_NULL_HANDLE;
}
