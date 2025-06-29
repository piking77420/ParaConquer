#include "utils/helper_functions.hpp"

vk::CommandBuffer Vulkan::Utils::BeginSingleTimeCommand(const SingleCommandBeginInfo& _singleCommandBeginInfo)
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



void Vulkan::Utils::EndSingleTimeCommand(vk::CommandBuffer _commandBuffer,
    const SingleCommandBeginInfo& _singleCommandBeginInfo, vk::Fence _fence)
{
    _commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.pCommandBuffers = &_commandBuffer;
    submitInfo.commandBufferCount = 1;
    
    VK_CALL(_singleCommandBeginInfo.queue.submit(1, &submitInfo, _fence));
    
    VK_CALL(_singleCommandBeginInfo.device.waitForFences(1, &_fence, vk::True, UINT64_MAX));
    VK_CALL(_singleCommandBeginInfo.device.resetFences(1, &_fence));

    _singleCommandBeginInfo.device.freeCommandBuffers(_singleCommandBeginInfo.commandPool, 1, &_commandBuffer);
}

