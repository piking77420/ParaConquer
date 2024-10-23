#include "back_end/vulkan_command_pool_function.hpp"

vk::CommandBuffer VK_NP::BeginSingleTimeCommands(vk::Device _device, vk::CommandPool _commandPool)
{
    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer outBuffer;
    VK_CALL(_device.allocateCommandBuffers(&allocInfo, &outBuffer));

    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    outBuffer.begin(beginInfo);

    return outBuffer;
}

void VK_NP::EndSingleTimeCommands(vk::CommandBuffer _commandBuffer)
{
    _commandBuffer.end();
}
