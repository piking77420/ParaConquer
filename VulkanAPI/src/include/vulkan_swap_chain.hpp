#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "io/window.hpp"
#include "low_renderer/swap_chain.hpp"

namespace Vulkan
{
    class VulkanSwapChain : public PC_CORE::SwapChain
    {
    public:
        VULKAN_API explicit VulkanSwapChain(uint32_t _widht, uint32_t _height);
        
        VULKAN_API explicit VulkanSwapChain() = default;

        VULKAN_API ~VulkanSwapChain() override;

        VULKAN_API vk::SurfaceFormatKHR GetSurfaceFormat();

        VULKAN_API void WaitForFrame() const override;

        VULKAN_API bool GetSwapChainImageIndex(PC_CORE::Window* windowHandle) override;

        VULKAN_API void Present(const PC_CORE::CommandList* _commandList, PC_CORE::Window* _window) override;
        
        VULKAN_API void HandleRecreateSwapChain(PC_CORE::Window* windowHandle) override;
        
    private:
         struct SyncObject
         {
             vk::Semaphore imageAvailableSemaphore;
             vk::Semaphore renderFinishedSemaphore;
             vk::Fence inFlightFence;
         };
        vk::SwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        std::vector<vk::Image> m_SwapChainImage;

        std::vector<vk::ImageView> m_SwapChainImageViews;
        
        vk::Extent2D m_Extent2D;
        
        vk::SurfaceFormatKHR m_SurfaceFormat;

        std::array<SyncObject, MAX_FRAMES_IN_FLIGHT> m_SyncObject;

        const vk::Queue* m_PresentQueue;
        
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities , uint32_t _width, uint32_t _height);

        void CreateImageViews();
        
        void CreateFrameBuffers();

        void CreateSyncObjects();

        void DestroySyncObjects();

        void CleanUpSwapChain();

        void CreateSwapChain(uint32_t _width, uint32_t _height);
    };

}

