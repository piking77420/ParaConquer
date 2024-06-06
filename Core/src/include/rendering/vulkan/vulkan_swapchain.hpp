#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
    class VulkanSwapchain
{
public:
    VkSwapchainKHR swapchainKhr;

    VkExtent2D swapChainExtent;

    std::vector<VulkanTexture> swapChainImages;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VulkanRenderPass mainRenderPass;
    
    VkSurfaceFormatKHR surfaceFormatKhr;

    uint32_t nbrOfImage = -1;

    void Init(uint32_t widht, uint32_t _height, const uint32_t _qfamilyIndex, const VkSurfaceKHR& _surface);

    void Destroy();
    
    void InitSwapChain(uint32_t widht, uint32_t _height, const uint32_t _qfamilyIndex, const VkSurfaceKHR& _surface);

    void InitRenderPass();

    void DestroySwapChain();

private:
    uint32_t ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities);
    
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes);
    
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats);
    
    void CreateFrameBuffer(const VkDevice& _device);
};

END_PCCORE