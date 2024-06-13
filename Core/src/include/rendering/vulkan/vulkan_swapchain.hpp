#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
    class VulkanSwapchain
{
private:
    struct SwapChainImage
    {
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
    };
    
public:
    VkSwapchainKHR swapchainKhr;

    VkExtent2D swapChainExtent;
    
    std::vector<SwapChainImage> swapChainImages;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkFormat depthFormat;
    
    VulkanRenderPass swapchainRenderPass;
    
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
    
    void CreateFrameBuffer(const VkDevice& _device);
};

END_PCCORE