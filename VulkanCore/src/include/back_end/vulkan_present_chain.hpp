#pragma once

#include "vulkan_header.h"

struct GLFWwindow;

namespace VK_NP
{
    
    class VulkanPresentChain
    {
    public:

        VulkanPresentChain(const VulkanAppCreateInfo& _vulkanMainCreateInfo);

        ~VulkanPresentChain();

        void RecreateSwapChain(void* _glfwWindowPtr);

        void CreateSwapchain(void* _glfwWindowPtr);

        void DestroySwapchain();
        
    private:
        vk::SurfaceFormatKHR m_SurfaceFormat;
        
        vk::PresentModeKHR m_PresentMode;

        vk::Extent2D m_Extent2D;

        uint32_t m_SwapchainImageCount;

        vk::SwapchainKHR m_SwapchainKhr;

        std::vector<vk::Image> m_SwapchainImages;

        std::vector<vk::ImageView> m_SwapChainImageViews;
        
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& _availableFormats);

        vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& _availablePresentModes);

        vk::Extent2D ChooseSwapExtent(GLFWwindow* _window, const vk::SurfaceCapabilitiesKHR& _capabilities);

        void CreateSwapchainImages(vk::Device _device);

        void HandleMinimize(void* _windowPtr);
    };
 
}
