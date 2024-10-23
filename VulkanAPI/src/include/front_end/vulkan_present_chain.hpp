#pragma once

#include "vulkan_header.h"

struct GLFWwindow;

namespace VK_NP
{
    class VulkanPresentChain
    {
    public:
        
        void Init(const VulkanAppCreateInfo& _vulkanMainCreateInfo, VulkanContext* _vulkanContext);

        void Destroy(VulkanContext* _context);

        void RecreateSwapChain(VulkanContext* _context, void* _glfwWindowPtr, uint32_t _newWidht, uint32_t _newHeight);

        void CreateSwapchain(void* _glfwWindowPtr, VulkanContext* _vulkanContext);

        void DestroySwapchain(VulkanContext* _vulkanContext);

        void AquireNetImageKHR(VulkanContext* _vulkanContext);

        void WaitForAvailableImage(VulkanContext* _vulkanContext);

        void SwapBuffer(vk::CommandBuffer* _commandBuffers, uint32_t _bufferCount, VulkanContext* _vulkanContext);
    
    private:
        
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& _availableFormats);

        vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& _availablePresentModes);

        vk::Extent2D ChooseSwapExtent(GLFWwindow* _window, const vk::SurfaceCapabilitiesKHR& _capabilities);

        void CreateSwapchainImages(VulkanContext* _vulkanContext);
        
        void CreateFramebuffers(VulkanContext* _vulkanContext);

        void CreateRenderPass(VulkanContext* _vulkanContext);

        void CreateSyncObject(VulkanContext* _vulkanContext);

        void DestroySyncObject(VulkanContext* _vulkanContext);

        void PresentNewImage(VulkanContext* _vulkanContext);
    };
}
