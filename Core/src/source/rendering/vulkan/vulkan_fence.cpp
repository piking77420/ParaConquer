#include "rendering/vulkan/vulkan_fence.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanFence::Init(const VkFenceCreateInfo& _semaphoreCreateInfo)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    Destroy();
    VkResult r = vkCreateFence(device, &_semaphoreCreateInfo, nullptr, &fences);
    VK_CHECK_ERROR(r,"Failed to create fence");
}

void VulkanFence::Destroy()
{

    if (fences != VK_NULL_HANDLE)
    {
        const VkDevice& device = VulkanInterface::GetDevice().device;
        vkDestroyFence(device, fences, nullptr);
    }
    
}

