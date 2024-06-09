#pragma once
#include "core_header.hpp"
#include "vulkan_debug_message.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_surface.hpp"
#include <GLFW/glfw3.h>
#include <vma/vk_mem_alloc.h>

#include "vulkan_command_pool.hpp"
#include "vulkan_device.hpp"
#include "vulkan_material_manager.hpp"
#include "vulkan_physical_devices.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_texture_sampler.hpp"

BEGIN_PCCORE
    class VulkanInterface
{
public:

    static void Init(Window* _window);

    static void Destroy();
    
    static uint32_t GetCurrentFrame();

    static uint32_t GetNbrOfImage();

    static void ComputeNextFrame();
    
    static VulkanDevice GetDevice();

    static PhysicalDevice& GetPhysicalDevice();

    static VkSurfaceFormatKHR GetSwapChainImageFormat();

    static VkFramebuffer GetSwapChainFramebuffer(uint32_t _index);

    static VulkanSurface GetVulkanSurface();

    static VmaAllocator GetAllocator();

    static void RecreateSwapChain(Window* _window);
       
    static VkImage GetImage(uint32_t _index);
    
    // to do move to physical device
    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    static inline VulkanSwapchain vulkanSwapChapchain;
    
    static inline VulkanCommandPool vulkanCommandPoolGraphic;

    static inline VulkanCommandPool vulkanCommandPoolTransfert;

    static inline VulkanTextureSampler vulkanTextureSampler;

    static inline VulkanPhysicalDevices vulkanPhysicalDevices;

    static inline uint32_t currentFrame = 0;
    
    static inline VulkanInstance vulkanInstance;
#ifndef NDEBUG
    static inline VulkanDebugMessage vulkanDebugMessage;
#endif
    
    static inline VulkanDevice vulkanDevice;
    
    static inline VulkanSurface vulkanSurface;

    static inline VmaAllocator vmaAllocator;

    static inline VulkanMaterialManager vulkanMaterialManager;
};

END_PCCORE