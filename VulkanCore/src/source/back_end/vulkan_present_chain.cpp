#include "back_end/vulkan_present_chain.hpp"
#include "back_end/vulkan_harware_wrapper.hpp"
#include "GLFW/glfw3.h"



VK_NP::VulkanPresentChain::VulkanPresentChain(const VulkanAppCreateInfo& _vulkanMainCreateInfo)
{
    CreateSwapchain(_vulkanMainCreateInfo.windowPtr);
}

VK_NP::VulkanPresentChain::~VulkanPresentChain()
{
    
}

void VK_NP::VulkanPresentChain::RecreateSwapChain(void* _glfwWindowPtr)
{
    HandleMinimize(_glfwWindowPtr);
    
    auto device = VulkanHarwareWrapper::GetDevice();
    device.waitIdle();
    
    DestroySwapchain();
    CreateSwapchain(_glfwWindowPtr);
}

void VK_NP::VulkanPresentChain::CreateSwapchain(void* _glfwWindowPtr)
{
    auto phyDevice = VulkanHarwareWrapper::GetPhysicalDevices();
    auto device = VulkanHarwareWrapper::GetDevice();
    auto surface = VulkanHarwareWrapper::GetSurface();
    auto swapChainSupportDetails = VulkanHarwareWrapper::GetSwapChainSupportDetailsSurface();
    const QueuFamiliesIndicies queuFamiliesIndicies = phyDevice.GetQueueFamiliesIndicies();
    uint32_t queueFamilyIndices[] = {queuFamiliesIndicies.graphicsFamily, queuFamiliesIndicies.presentFamily};
    
    m_SurfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    m_PresentMode = ChoosePresentMode(swapChainSupportDetails.presentModes);
    m_Extent2D = ChooseSwapExtent(static_cast<GLFWwindow*>(_glfwWindowPtr), swapChainSupportDetails.capabilities);

    m_SwapchainImageCount = swapChainSupportDetails.capabilities.maxImageCount;
    
    
    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = m_SwapchainImageCount;
    swapChainCreateInfo.imageFormat = m_SurfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = m_Extent2D;
    swapChainCreateInfo.imageArrayLayers = 1;
    // MAY CHANGE IN FUTURE
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    if (queuFamiliesIndicies.graphicsFamily != queuFamiliesIndicies.presentFamily)
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = &queueFamilyIndices[0];
    } else
    {
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }
    swapChainCreateInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    swapChainCreateInfo.presentMode = m_PresentMode;
    swapChainCreateInfo.clipped = vk::True;
    swapChainCreateInfo.oldSwapchain = nullptr;

    VK_CALL(device.createSwapchainKHR(&swapChainCreateInfo, nullptr, &m_SwapchainKhr));

    // Create SwapChain Image
    m_SwapchainImages = device.getSwapchainImagesKHR(m_SwapchainKhr);
    
}

void VK_NP::VulkanPresentChain::DestroySwapchain()
{
    auto device = VulkanHarwareWrapper::GetDevice();
    
    for (auto imageView : m_SwapChainImageViews)
    {
        device.destroyImageView(imageView, nullptr);
    }
    
    device.destroySwapchainKHR(m_SwapchainKhr);
}

vk::SurfaceFormatKHR VK_NP::VulkanPresentChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& _availableFormats)
{
    for (const auto& availableFormat : _availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
        {
            return availableFormat;
        }
    }
    
    return _availableFormats[0];
}

vk::PresentModeKHR VK_NP::VulkanPresentChain::ChoosePresentMode(
    const std::vector<vk::PresentModeKHR>& _availablePresentModes)
{
    // TODO may change in future
    /*
    for (const auto& availablePresentMode : availablePresentModes)
    {
        
    }*/

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VK_NP::VulkanPresentChain::ChooseSwapExtent(GLFWwindow* _window, const vk::SurfaceCapabilitiesKHR& _capabilities)
{

    if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return _capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        vk::Extent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VK_NP::VulkanPresentChain::CreateSwapchainImages(vk::Device _device)
{
    for (size_t i = 0; i < m_SwapchainImages.size(); i++)
    {
        vk::ImageViewCreateInfo viewInfo = {};
        viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
        viewInfo.image = m_SwapchainImages[i];
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = m_SurfaceFormat.format;

        viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.a = vk::ComponentSwizzle::eIdentity;

        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CALL(_device.createImageView(&viewInfo, nullptr, &m_SwapChainImageViews[i]));
    }
}

void VK_NP::VulkanPresentChain::HandleMinimize(void* _windowPtr)
{
    int width = 0, height = 0;
    GLFWwindow* wwindow = static_cast<GLFWwindow*>(_windowPtr);
    glfwGetFramebufferSize(wwindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(wwindow, &width, &height);
        glfwWaitEvents();
    }

}


