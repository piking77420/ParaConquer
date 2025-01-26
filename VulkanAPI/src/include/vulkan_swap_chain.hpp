#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "low_renderer/swap_chain.hpp"

namespace Vulkan
{
    class VulkanSwapChain : public PC_CORE::SwapChain
    {
    public:
         VULKAN_API explicit VulkanSwapChain(size_t _width, size_t _height);
        
        VULKAN_API explicit VulkanSwapChain() = default;

        VULKAN_API ~VulkanSwapChain() override;

        VULKAN_API vk::SurfaceFormatKHR GetSurfaceFormat();

    private:
        vk::SwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        std::vector<vk::Image> m_SwapChainImage;

        std::vector<vk::ImageView> m_SwapChainImageViews;

        std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

        vk::Extent2D m_Extent2D;
        
        vk::SurfaceFormatKHR m_SurfaceFormat;
        
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities , uint32_t _width, uint32_t _height);

        void CreateImageViews();
        
        void CreateFrameBuffers();
        
    };

}

