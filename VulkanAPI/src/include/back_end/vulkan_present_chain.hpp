﻿#pragma once

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

        void AquireNetImageKHR();

        void WaitForAvailableImage();

        void SwapBuffer();

        vk::Semaphore* GetImageAvailableSemaphore();

        vk::Semaphore* GetRenderFinishedSemaphore();

        vk::Fence* GetInFlightFence();

        static uint32_t GetImageIndex();

        static vk::Extent2D GetExtent();

        static vk::Format GetSwapChainFormat();

        static vk::RenderPass GetRenderPassTmpr();

        static vk::Framebuffer GetFramebuffer(size_t index);

    private:
        struct SwapChainSyncObject
        {
            vk::Semaphore imageAvailableSemaphore;
            vk::Semaphore renderFinishedSemaphore;
            vk::Fence inFlightFence;
        };
		
        
        vk::SurfaceFormatKHR m_SurfaceFormat;

        vk::PresentModeKHR m_PresentMode;

        vk::Extent2D m_Extent2D;

        uint32_t m_SwapchainImageCount;

        uint32_t m_ImageIndex;

        vk::SwapchainKHR m_SwapchainKhr;

        SwapChainSyncObject m_SwapChainSyncObject;

        vk::Device* m_DeviceConstPtr = nullptr;

        std::vector<vk::Image> m_SwapchainImages;

        std::vector<vk::ImageView> m_SwapChainImageViews;

        std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

        vk::RenderPass m_RenderPass;

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& _availableFormats);

        vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& _availablePresentModes);

        vk::Extent2D ChooseSwapExtent(GLFWwindow* _window, const vk::SurfaceCapabilitiesKHR& _capabilities);

        void CreateSwapchainImages(vk::Device _device);

        void HandleMinimize(void* _windowPtr);

        void CreateFramebuffers();

        void CreateRenderPass();

        void CreateSyncObject();

        void PresentNewImage();

        static VulkanPresentChain* m_VulkanPresentChainIntance;
    };
}