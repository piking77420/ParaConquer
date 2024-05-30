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
    uint32_t nbrOfImage = -1;

    uint32_t queuefamily = -1;
    
    void Init(GLFWwindow* window);

    void Destroy();

    void AllocateCommandBuffers(size_t _nbr, VkCommandBuffer* _vkCommandBufferPtr);

    VkImage GetImage(uint32_t _index);
    
private:
    VulkanInstance vulkanInstance;
#ifndef NDEBUG
    VulkanDebugMessage vulkanDebugMessage;
#endif

    VulkanSurface vulkanSurface;

    VulkanPhysicalDevices vulkanDevices;
    
    VulkanSwapchain vulkanChapchain;
    
    VulkanDevice vulkanDevice;

    VulkanCommandPool vulkanCommandPool;

};

END_PCCORE