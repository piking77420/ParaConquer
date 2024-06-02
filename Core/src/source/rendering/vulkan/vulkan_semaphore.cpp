#include "rendering/vulkan/vulkan_semaphore.h"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanSemaphore::Init(const VkSemaphoreCreateInfo& semaphoreCreateInfo)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    Destroy();
    const VkResult r = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
    VK_CHECK_ERROR(r,"Failed to create Semaphore");
    
}


void VulkanSemaphore::Destroy()
{
    if (semaphore != VK_NULL_HANDLE)
    {
        const VkDevice& device = VulkanInterface::GetDevice().device;
        vkDestroySemaphore(device, semaphore, nullptr);
    }
}
