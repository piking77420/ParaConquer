#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"

BEGIN_PCCORE
    class VulkanSwapchain
{
public:
    VkSwapchainKHR swapchainKhr;

    VkExtent2D swapChainExtent;
    
    std::vector<VkImage> swapChainImage;
    
    std::vector<VkImageView> swapChainImageView;
    
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VulkanRenderPass mainRenderPass;
    
    VkSurfaceFormatKHR surfaceFormatKhr;

    uint32_t Init(const PhysicalDevice& _physicalDevice, const uint32_t _qfamilyIndex, const VkSurfaceKHR& _surface, const VkDevice
              _device);

    void Destroy(const VkDevice& _device);
    
private:
    uint32_t ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities);
    
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes);
    
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats);

    VkImageView CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
                            VkImageViewType ViewType, uint32_t LayerCount, uint32_t mipLevels);

    void CreateFrameBuffer(const VkDevice& _device);
};

END_PCCORE