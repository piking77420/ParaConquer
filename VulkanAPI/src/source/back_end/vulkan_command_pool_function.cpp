#include "back_end/vulkan_command_pool_function.hpp"

vk::CommandBuffer Vulkan::BeginSingleTimeCommands(vk::Device _device, vk::CommandPool _commandPool)
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

void Vulkan::EndSingleTimeCommands(vk::CommandBuffer _commandBuffer,vk::Device _device, vk::Fence _fence, vk::Queue _queue)
{
    _commandBuffer.end();

    vk::SubmitInfo submitInfo = {};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.pCommandBuffers = &_commandBuffer;
    submitInfo.commandBufferCount = 1;

    VK_CALL(_queue.submit(1, &submitInfo, _fence));
    VK_CALL(_device.waitForFences(1, &_fence, VK_TRUE, UINT64_MAX));
    
    _device.resetFences(_fence);
}
