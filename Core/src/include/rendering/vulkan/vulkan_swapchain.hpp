#pragma once

#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanSwapchain
{
public:
    VkSwapchainKHR swapchainKhr;

    std::vector<VkImage> swapChainImage;
    
    std::vector<VkImageView> swapChainImageView;

    VkSurfaceFormatKHR surfaceFormatKhr;

    uint32_t Init(const PhysicalDevice& _physicalDevice, const uint32_t _qfamilyIndex, const VkSurfaceKHR& _surface, const VkDevice
              _device);

    void Destroy(const VkDevice& device);
    
private:
    uint32_t ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities);
    
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& PresentModes);
    
    VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats);

    VkImageView CreateImageView(VkDevice Device, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags,
                            VkImageViewType ViewType, uint32_t LayerCount, uint32_t mipLevels);
};

END_PCCORE