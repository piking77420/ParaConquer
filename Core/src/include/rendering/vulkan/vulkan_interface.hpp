#pragma once
#include "core_header.hpp"
#include "vulkan_debug_message.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_surface.hpp"
#include <GLFW/glfw3.h>

#include "vulkan_command_pool.hpp"
#include "vulkan_device.hpp"
#include "vulkan_physical_devices.hpp"
#include "vulkan_swapchain.hpp"

BEGIN_PCCORE
class VulkanInterface
{
public:

    static uint32_t GetCurrentFrame();

    static uint32_t GetNbrOfImage();

    static void ComputeNextFrame();
    
    static VulkanDevice GetDevice();

    static VkSurfaceFormatKHR GetSwapChainImageFormat();

    static VkFramebuffer GetSwapChainFramebuffer(uint32_t _index);

    VulkanSwapchain vulkanSwapChapchain;
    
    void Init(GLFWwindow* window);

    void Destroy();

    void AllocateCommandBuffers(size_t _nbr, VkCommandBuffer* _vkCommandBufferPtr);

    VkImage GetImage(uint32_t _index);

private:

    static inline VulkanInterface* m_Instance = nullptr;

    uint32_t nbrOfImage = -1;

    uint32_t currentFrame = 0;
    
    VulkanInstance vulkanInstance;
#ifndef NDEBUG
    VulkanDebugMessage vulkanDebugMessage;
#endif

    VulkanSurface vulkanSurface;

    VulkanPhysicalDevices VulkanPhysicalDevices;
    
    VulkanDevice vulkanDevice;

    VulkanCommandPool vulkanCommandPool;

};

END_PCCORE