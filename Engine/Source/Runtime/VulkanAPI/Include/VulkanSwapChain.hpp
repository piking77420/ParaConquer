#pragma once

#include "VulkanHeader.h"
#include "VulkanRenderPass.hpp"
#include "Io/Window.hpp"
#include "LowRenderer/SwapChain.hpp"
#include "VulkanPhysicalDevices.hpp"
#include "VulkanDevice.hpp"

namespace PC_CORE
{
    class CommandList;
}

namespace Vulkan
{

    class VulkanSwapChain : public PC_CORE::RhiSwapChain
    {
    public:
        VULKAN_API void* GetFrameBuffer() override;

        VULKAN_API explicit VulkanSwapChain(PC_CORE::Rhi& _Rhi, uint32_t _Widht, uint32_t _Height, 
                                            VulkanPhysicalDevices& vulkanPhysicalDevices, VulkanDevice& _VulkanDevice,  vk::SurfaceKHR _Surface);

        VULKAN_API explicit VulkanSwapChain() = default;

        VULKAN_API ~VulkanSwapChain() override;

        VULKAN_API vk::SurfaceFormatKHR GetSurfaceFormat();

        VULKAN_API bool GetSwapChainImageIndex(PC_CORE::Window* windowHandle) override;

        VULKAN_API void Present(PC_CORE::Window* _window) override;

        VULKAN_API void HandleRecreateSwapChain(PC_CORE::Window* windowHandle) override;

        VULKAN_API void BeginSwapChainRenderPass(PC_CORE::CommandList* _commandList) override;

        VULKAN_API void EndSwapChainRenderPass(PC_CORE::CommandList* _commandList) override;

        VULKAN_API const void* GetFrameNativeHandle(size_t _frameIndex) const override;

        VULKAN_API void* GetFrameNativeHandle(size_t _frameIndex) override;

        VULKAN_API bool Build() override;

        VULKAN_API  uint32_t GetWidth() const override;

        VULKAN_API  uint32_t GetHeight() const override;

    private:
        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

        vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t _width,
                                      uint32_t _height);

        void FromSurface(const VulkanPhysicalDevices& VulkanPhysicalDevices, vk::SurfaceKHR _Surface, 
            uint32_t _Width, uint32_t _Height);

        void CreateImageViews();

        void CreateFrameBuffers();

        void CleanUpSwapChain();

        void CreateSwapChain();

        std::vector<vk::Fence> m_ImagesInFligh;

        vk::SwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        vk::SurfaceKHR m_Surface = VK_NULL_HANDLE;

        vk::Device m_Device = VK_NULL_HANDLE;

        uint32_t m_QueueFamilyIndices = 0;

        SwapChainSupportDetails m_SwapChainSupportDetails;

        vk::SurfaceFormatKHR m_SurfaceFormatKHR{};

        vk::PresentModeKHR m_PresentModeKHR{};

        vk::Extent2D m_Extent2D{};

        std::vector<vk::Framebuffer> m_Framebuffers;

        std::vector<vk::Image> m_SwapChainImage;

        std::vector<vk::ImageView> m_SwapChainImageViews;

        const vk::Queue* m_PresentQueue;
        
    };
}
