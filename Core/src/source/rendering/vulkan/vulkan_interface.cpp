#include "rendering\vulkan\vulkan_interface.hpp"

#include <stacktrace>

#include "app.hpp"
#include "log.hpp"

using namespace PC_CORE;

void VulkanInterface::Init(GLFWwindow* window)
{
    vulkanInstance.Init();
#ifndef NDEBUG
    vulkanDebugMessage.Init(vulkanInstance.instance);
#endif
    vulkanSurface.Init(vulkanInstance.instance,window);
    vulkanDevices.Init(vulkanInstance.instance, vulkanSurface.surfaceKhr);
    queuefamily = vulkanDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);
    vulkanDevice.Init(vulkanDevices.GetCurrentDevice(), queuefamily);

    const VkCommandPoolCreateInfo vkCommandPoolCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queuefamily
    };
    
    vulkanCommandPool.Init(vulkanDevice.device, &vkCommandPoolCreateInfo);
    
    // get the nbr of image for the swap chain
    nbrOfImage = vulkanChapchain.Init(vulkanDevices.GetCurrentDevice(), queuefamily, vulkanSurface.surfaceKhr, vulkanDevice.device);
}

void VulkanInterface::Destroy()
{
    vulkanChapchain.Destroy(vulkanDevice.device);
    vulkanCommandPool.Destroy(vulkanDevice.device);

    vulkanDevice.Destroy();
    vulkanSurface.Destroy(vulkanInstance.instance);
#ifndef NDEBUG
    vulkanDebugMessage.Destroy(vulkanInstance.instance);
#endif
    vulkanInstance.Destroy();
}

void VulkanInterface::AllocateCommandBuffers(size_t _nbr, VkCommandBuffer* _vkCommandBufferPtr)
{
    vulkanCommandPool.AllocCommandBuffer(vulkanDevice.device, _nbr, _vkCommandBufferPtr);
}

VkImage VulkanInterface::GetImage(uint32_t _index)
{
    return vulkanChapchain.swapChainImage.at(_index);
}

