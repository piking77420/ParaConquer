#include "helper_functions.hpp"

vk::CommandBuffer Vulkan::BeginSingleTimeCommand(const SingleCommandBeginInfo& _singleCommandBeginInfo)
{
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocateInfo.commandPool = _singleCommandBeginInfo.commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    
    vk::CommandBuffer commandBuffer = _singleCommandBeginInfo.device.allocateCommandBuffers(allocateInfo)[0];

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Vulkan::EndSingleTimeCommand(vk::CommandBuffer _commandBuffer, const SingleCommandBeginInfo& _singleCommandBeginInfo)
{
    _commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.pCommandBuffers = &_commandBuffer;
    submitInfo.commandBufferCount = 1;
    
    VK_CALL(_singleCommandBeginInfo.queue.submit(1, &submitInfo, nullptr));
    _singleCommandBeginInfo.queue.waitIdle();
    
    _singleCommandBeginInfo.device.freeCommandBuffers(_singleCommandBeginInfo.commandPool, 1, &_commandBuffer);

}
