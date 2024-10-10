#include "back_end/vulkan_command_pool.hpp"

VkCommandPool VK_NP::VulkanCommandPool::CreateCommandPool(VulkanContext* _currentContext,
    const PC_CORE::CommandPoolCreateInfo& _createInfo)
{
    vk::CommandPoolCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    createInfo.flags = CommandPoolCreateFlagBitsToVulkan(_createInfo.commandPoolBufferFlag);
    createInfo.queueFamilyIndex = GetQueueFamiliesIndexFromType(_currentContext, _createInfo.queueType);

    vk::CommandPool commandPool = VK_NULL_HANDLE;
    commandPool = _currentContext->device.createCommandPool(createInfo, nullptr);


    const CommandPoolUsage commandPoolUsage =
        {
        .pool = commandPool,
        .maxCommandBuffers = _createInfo.maxCommandBufferCount,
        .currentAllocateCommandBuffer = 0
        };
    _currentContext->commandPoolsData.commandPools.insert({_currentContext->commandPoolsData.idsCount++, commandPoolUsage});
}

void VK_NP::VulkanCommandPool::DestroyCommandPool(VulkanContext* _currentContext)
{
    for (auto errorDescription : _currentContext->commandPoolsData.commandPools)
    {
        _currentContext->device.destroyCommandPool(errorDescription.second.pool);
        _currentContext->commandPoolsData.commandPools.erase(errorDescription.first);
    }
}

vk::CommandPoolCreateFlagBits VK_NP::VulkanCommandPool::CommandPoolCreateFlagBitsToVulkan(
    PC_CORE::CommandPoolBufferFlag _flag)
{
    vk::CommandPoolCreateFlagBits commandPoolCreateFlagBits {};

    if (_flag & PC_CORE::CommandPoolBufferFlag::RESET)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if (_flag & PC_CORE::CommandPoolBufferFlag::TRANSIENT)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eTransient;

    if (_flag & PC_CORE::CommandPoolBufferFlag::PROTECTED)
        commandPoolCreateFlagBits = vk::CommandPoolCreateFlagBits::eProtected;

    return commandPoolCreateFlagBits;
}

uint32_t VK_NP::VulkanCommandPool::GetQueueFamiliesIndexFromType(VulkanContext* _context, PC_CORE::QueuType queueType)
{
    uint32_t queueFamiliesIndex = 0;
    
    switch (queueType)
    {
    case PC_CORE::QueuType::NONE:
        break;
    case PC_CORE::QueuType::GRAPHICS:
        queueFamiliesIndex = _context->queuFamiliesIndicies.graphicsFamily;
        break;
    case PC_CORE::QueuType::COMPUTE:
        queueFamiliesIndex = _context->queuFamiliesIndicies.computeFamily;
        break;
    case PC_CORE::QueuType::TRANSFERT:
        queueFamiliesIndex = _context->queuFamiliesIndicies.transferFamily;
        break;
    case PC_CORE::QueuType::SPARSE_BINDING:
        break;
    case PC_CORE::QueuType::VIDEO_DECODE:
        break;
    case PC_CORE::QueuType::VIDEO_ENCODE:
        break;
    case PC_CORE::QueuType::OPTICAL_FLOW:
        break;
    case PC_CORE::QueuType::COUT:
        break;
    default:
    }

    return queueFamiliesIndex;
}

