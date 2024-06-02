#include "rendering\vulkan\vulkan_interface.hpp"
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.2
#include <vma/vk_mem_alloc.h>

#include <stacktrace>

#include "app.hpp"
#include "log.hpp"

using namespace PC_CORE;
VulkanInterface::VulkanInterface()
{
    instance = this;
}

void VulkanInterface::Init(Window* _window)
{
    instance->vulkanInstance.Init();
#ifndef NDEBUG
    vulkanDebugMessage.Init(vulkanInstance.instance);
#endif
    vulkanSurface.Init(vulkanInstance.instance,_window->window);
    VulkanPhysicalDevices.Init(vulkanInstance.instance, vulkanSurface.surfaceKhr);
    VulkanPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT,true);
    vulkanDevice.Init(VulkanPhysicalDevices);

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.physicalDevice = VulkanPhysicalDevices.GetCurrentDevice().physDevice;
    allocatorCreateInfo.device = vulkanDevice.device;
    allocatorCreateInfo.instance = vulkanInstance.instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    
    
    vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);

    const VkCommandPoolCreateInfo vkCommandPoolCreateInfoGraphic =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vulkanDevice.graphicsQueue.index
    };
    vulkanCommandPoolGraphic.Init(&vkCommandPoolCreateInfoGraphic);

    const VkCommandPoolCreateInfo vkCommandPoolCreateInfoTransfer =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = vulkanDevice.transferQueue.index
    };
    vulkanCommandPoolTransfert.Init(&vkCommandPoolCreateInfoTransfer);
    
    // get the nbr of image for the swap chain
    vulkanSwapChapchain.Init(_window->widht, _window->height, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
}

void VulkanInterface::Destroy()
{
    vulkanSwapChapchain.Destroy();
    vulkanCommandPoolGraphic.Destroy();
    vulkanCommandPoolTransfert.Destroy();

    vmaDestroyAllocator(vmaAllocator);
    vulkanDevice.Destroy();
    vulkanSurface.Destroy(vulkanInstance.instance);
#ifndef NDEBUG
    vulkanDebugMessage.Destroy(vulkanInstance.instance);
#endif
    vulkanInstance.Destroy();
}

VkImage VulkanInterface::GetImage(uint32_t _index)
{
    return vulkanSwapChapchain.swapChainImage.at(_index);
}

VkSurfaceFormatKHR VulkanInterface::GetSwapChainImageFormat()
{
    return instance->vulkanSwapChapchain.surfaceFormatKhr;
}

VkFramebuffer VulkanInterface::GetSwapChainFramebuffer(uint32_t _index)
{
    return instance->vulkanSwapChapchain.swapChainFramebuffers.at(_index);
}

VulkanSurface VulkanInterface::GetVulkanSurface()
{
    return instance->vulkanSurface;
}

VmaAllocator VulkanInterface::GetAllocator()
{
    return instance->vmaAllocator;
}

uint32_t VulkanInterface::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    const VkPhysicalDeviceMemoryProperties& memProperties = instance->VulkanPhysicalDevices.GetCurrentDevice().memProps;

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanInterface::InitInterface(Window* _window)
{
    instance->Init(_window);
}

void VulkanInterface::DestroyInterface()
{
    instance->Destroy();
}

void VulkanInterface::RecreateSwapChain(Window* _window)
{
    vulkanSwapChapchain.DestroySwapChain();
    vulkanSwapChapchain.InitSwapChain(_window->widht, _window->height, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
}


uint32_t VulkanInterface::GetCurrentFrame()
{
    return instance->currentFrame;
}

uint32_t VulkanInterface::GetNbrOfImage()
{
    return instance->vulkanSwapChapchain.nbrOfImage;
}

void VulkanInterface::ComputeNextFrame()
{
    instance->currentFrame = (instance->currentFrame + 1) % instance->vulkanSwapChapchain.nbrOfImage;
}

VulkanDevice VulkanInterface::GetDevice()
{
    return instance->vulkanDevice;
}

PhysicalDevice& VulkanInterface::GetPhysicalDevice()
{
    return instance->VulkanPhysicalDevices.GetCurrentDevice();
}

