#pragma once

#include "vulkan_header.h"
#include "low_renderer/swap_chain.hpp"

namespace Vulkan
{
    class VulkanSwapChain : public PC_CORE::SwapChain
    {
    public:
         VULKAN_API explicit VulkanSwapChain(size_t _width, size_t _height);
        
        VULKAN_API explicit VulkanSwapChain() = default;

        VULKAN_API ~VulkanSwapChain() override;

    private:
        vk::SwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        std::vector<vk::Image> m_SwapChainImage;

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities , uint32_t _width, uint32_t _height);
        
    };

}

