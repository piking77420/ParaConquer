#pragma once
#include "core_header.hpp"
#include "vulkan_debug_message.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_surface.hpp"
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>

#include "vulkan_command_pool.hpp"
#include "vulkan_device.hpp"
#include "vulkan_physical_devices.hpp"
#include "vulkan_swapchain.hpp"

BEGIN_PCCORE
class VulkanInterface
{
public:

    VulkanInterface();

    static inline VulkanInterface* instance = nullptr;
    
    static uint32_t GetCurrentFrame();

    static uint32_t GetNbrOfImage();

    static void ComputeNextFrame();
    
    static VulkanDevice GetDevice();

    static PhysicalDevice& GetPhysicalDevice();

    static VkSurfaceFormatKHR GetSwapChainImageFormat();

    static VkFramebuffer GetSwapChainFramebuffer(uint32_t _index);

    static VulkanSurface GetVulkanSurface();

    static VmaAllocator GetAllocator();
    
    // to do move to physical device
    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static void InitInterface(Window* _window);

    static void DestroyInterface();
    
    VulkanSwapchain vulkanSwapChapchain;
    
    VulkanCommandPool vulkanCommandPoolGraphic;

    VulkanCommandPool vulkanCommandPoolTransfert;
    
    
    void RecreateSwapChain(Window* _window);
    
    VkImage GetImage(uint32_t _index);

private:

    void Init(Window* _window);

    void Destroy();
    
    uint32_t nbrOfImage = -1;

    uint32_t currentFrame = 0;
    
    VulkanInstance vulkanInstance;
#ifndef NDEBUG
    VulkanDebugMessage vulkanDebugMessage;
#endif
    
    VulkanPhysicalDevices VulkanPhysicalDevices;
    
    VulkanDevice vulkanDevice;
    
    VulkanSurface vulkanSurface;

    VmaAllocator vmaAllocator;
    
};

END_PCCORE