#include "rendering\vulkan\vulkan_interface.hpp"
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.2
#include <vma/vk_mem_alloc.h>

#include <stacktrace>

#include "app.hpp"
#include "log.hpp"

using namespace PC_CORE;


void VulkanInterface::Init(Window* _window)
{
    
    vulkanInstance.Init();
#ifndef NDEBUG
    vulkanDebugMessage.Init(vulkanInstance.instance);
#endif
    vulkanSurface.Init(vulkanInstance.instance,_window->window);
    vulkanPhysicalDevices.Init(vulkanInstance.instance, vulkanSurface.surfaceKhr);
    vulkanPhysicalDevices.SelectDevice(VK_QUEUE_GRAPHICS_BIT,true);
    vulkanDevice.Init(vulkanPhysicalDevices);

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.physicalDevice = vulkanPhysicalDevices.GetCurrentDevice().physDevice;
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

    // get the nbr of image for the swap chain
    vulkanSwapChapchain.Init(_window->windowSize.x, _window->windowSize.y, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
    vulkanMaterialManager.Init();
    vulkanTextureSampler.CreateBasicSampler();
}

void VulkanInterface::Destroy()
{
    vulkanMaterialManager.Destroy();
    vulkanSwapChapchain.Destroy();
    vulkanCommandPoolGraphic.Destroy();
    vulkanCommandPoolViewPort.Destroy();
    vulkanTextureSampler.Destroy();

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
    return vulkanSwapChapchain.swapChainImages.at(_index).image;
}

VkSurfaceFormatKHR VulkanInterface::GetSwapChainImageFormat()
{
    return vulkanSwapChapchain.surfaceFormatKhr;
}

VkFramebuffer VulkanInterface::GetSwapChainFramebuffer(uint32_t _index)
{
    return vulkanSwapChapchain.swapChainFramebuffers.at(_index);
}

VulkanSurface VulkanInterface::GetVulkanSurface()
{
    return vulkanSurface;
}

VmaAllocator VulkanInterface::GetAllocator()
{
    return vmaAllocator;
}

uint32_t VulkanInterface::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    const VkPhysicalDeviceMemoryProperties& memProperties = vulkanPhysicalDevices.GetCurrentDevice().memProps;

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
    vulkanSwapChapchain.DestroySwapChain();
    vulkanSwapChapchain.InitSwapChain(_window->windowSize.x, _window->windowSize.y, vulkanDevice.graphicsQueue.index, vulkanSurface.surfaceKhr);
}


uint32_t VulkanInterface::GetCurrentFrame()
{
    return currentFrame;
}

uint32_t VulkanInterface::GetNbrOfImage()
{
    return vulkanSwapChapchain.nbrOfImage;
}

void VulkanInterface::ComputeNextFrame()
{
    currentFrame = (currentFrame + 1) % vulkanSwapChapchain.nbrOfImage;
}

VulkanDevice& VulkanInterface::GetDevice()
{
    return vulkanDevice;
}

PhysicalDevice& VulkanInterface::GetPhysicalDevice()
{
    return vulkanPhysicalDevices.GetCurrentDevice();
}

