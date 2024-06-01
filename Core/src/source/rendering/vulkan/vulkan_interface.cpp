#include "rendering\vulkan\vulkan_interface.hpp"

#include <stacktrace>

#include "app.hpp"
#include "log.hpp"

using namespace PC_CORE;

void VulkanInterface::Init(Window* _window)
{
    m_Instance = this;
    
    vulkanInstance.Init();
#ifndef NDEBUG
    vulkanDebugMessage.Init(vulkanInstance.instance);
#endif
    vulkanSurface.Init(vulkanInstance.instance,_window->window);
    VulkanPhysicalDevices.Init(vulkanInstance.instance, vulkanSurface.surfaceKhr);
    VulkanPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT,true);
    vulkanDevice.Init(VulkanPhysicalDevices);

    const VkCommandPoolCreateInfo vkCommandPoolCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vulkanDevice.graphicsQueue.index
    };
    
    vulkanCommandPool.Init(vulkanDevice.device, &vkCommandPoolCreateInfo);
    
    // get the nbr of image for the swap chain
    nbrOfImage = vulkanSwapChapchain.Init(_window->widht, _window->height, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
}

void VulkanInterface::Destroy()
{
    vulkanSwapChapchain.Destroy();
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
    return vulkanSwapChapchain.swapChainImage.at(_index);
}

VkSurfaceFormatKHR VulkanInterface::GetSwapChainImageFormat()
{
    return m_Instance->vulkanSwapChapchain.surfaceFormatKhr;
}

VkFramebuffer VulkanInterface::GetSwapChainFramebuffer(uint32_t _index)
{
    return m_Instance->vulkanSwapChapchain.swapChainFramebuffers.at(_index);
}

VulkanSurface VulkanInterface::GetVulkanSurface()
{
    return m_Instance->vulkanSurface;
}

uint32_t VulkanInterface::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    const VkPhysicalDeviceMemoryProperties& memProperties = m_Instance->VulkanPhysicalDevices.GetCurrentDevice().memProps;

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanInterface::RecreateSwapChain(Window* _window)
{
    vulkanSwapChapchain.Destroy();
    vulkanSwapChapchain.Init(_window->widht, _window->height, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
}


uint32_t VulkanInterface::GetCurrentFrame()
{
    return m_Instance->currentFrame;
}

uint32_t VulkanInterface::GetNbrOfImage()
{
    return m_Instance->nbrOfImage;
}

void VulkanInterface::ComputeNextFrame()
{
    m_Instance->currentFrame = (m_Instance->currentFrame + 1) % m_Instance->nbrOfImage;
}

VulkanDevice VulkanInterface::GetDevice()
{
    return m_Instance->vulkanDevice;
}

PhysicalDevice& VulkanInterface::GetPhysicalDevice()
{
    return m_Instance->VulkanPhysicalDevices.GetCurrentDevice();
}

