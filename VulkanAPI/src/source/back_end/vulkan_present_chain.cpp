#include "back_end/vulkan_present_chain.hpp"
#include "back_end/vulkan_harware_wrapper.hpp"
#include "GLFW/glfw3.h"

VK_NP::VulkanPresentChain* VK_NP::VulkanPresentChain::m_VulkanPresentChainIntance = nullptr; 

VK_NP::VulkanPresentChain::VulkanPresentChain(const VulkanAppCreateInfo& _vulkanMainCreateInfo)
{
    if (m_VulkanPresentChainIntance == nullptr)
        m_VulkanPresentChainIntance = this;

    m_DeviceConstPtr = VulkanHarwareWrapper::GetDevice();
    
    CreateSwapchain(_vulkanMainCreateInfo.windowPtr);
    CreateSyncObject();
}

VK_NP::VulkanPresentChain::~VulkanPresentChain()
{
    DestroySwapchain();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        m_DeviceConstPtr.destroySemaphore(m_SwapChainSyncObject[i].imageAvailableSemaphore);
        m_DeviceConstPtr.destroySemaphore(m_SwapChainSyncObject[i].renderFinishedSemaphore);
        m_DeviceConstPtr.destroyFence(m_SwapChainSyncObject[i].inFlightFence);
    }
    
    m_DeviceConstPtr.destroyRenderPass(m_RenderPass);
}

void VK_NP::VulkanPresentChain::RecreateSwapChain(void* _glfwWindowPtr)
{
    HandleMinimize(_glfwWindowPtr);
    
    m_DeviceConstPtr.waitIdle();
    
    DestroySwapchain();
    CreateSwapchain(_glfwWindowPtr);
}

void VK_NP::VulkanPresentChain::CreateSwapchain(void* _glfwWindowPtr)
{
    auto phyDevice = VulkanHarwareWrapper::GetPhysicalDevices();
    auto device = VulkanHarwareWrapper::GetDevice();
    auto surface = VulkanHarwareWrapper::GetSurface();
    auto swapChainSupportDetails = VulkanHarwareWrapper::GetSwapChainSupportDetailsSurface();
    QueuFamiliesIndicies queuFamiliesIndicies = phyDevice.GetQueueFamiliesIndicies();


    uint32_t queueFamilyIndices[] = {queuFamiliesIndicies.graphicsFamily, queuFamiliesIndicies.presentFamily};
    
    m_SurfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    m_PresentMode = ChoosePresentMode(swapChainSupportDetails.presentModes);
    m_Extent2D = ChooseSwapExtent(static_cast<GLFWwindow*>(_glfwWindowPtr), swapChainSupportDetails.capabilities);

    m_SwapchainImageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
    
     if (swapChainSupportDetails.capabilities.maxImageCount > 0 && m_SwapchainImageCount > swapChainSupportDetails.capabilities.maxImageCount)
     {
         m_SwapchainImageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    
    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = m_SwapchainImageCount;
    swapChainCreateInfo.imageFormat = m_SurfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = m_Extent2D;
    swapChainCreateInfo.imageArrayLayers = 1;
    // MAY CHANGE IN FUTURE
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;

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

   
    // Important to be here tmpr
    CreateRenderPass();
    // Create SwapChain Image
    m_SwapchainImages = device.getSwapchainImagesKHR(m_SwapchainKhr);
    CreateSwapchainImages(m_DeviceConstPtr);
    CreateFramebuffers();
    
}

void VK_NP::VulkanPresentChain::DestroySwapchain()
{

    for (auto& framebuffer : m_SwapChainFramebuffers)
    {
        m_DeviceConstPtr.destroyFramebuffer(framebuffer, nullptr);
    }
    
    for (auto& imageView : m_SwapChainImageViews)
    {
        m_DeviceConstPtr.destroyImageView(imageView, nullptr);
    }
    
    m_DeviceConstPtr.destroySwapchainKHR(m_SwapchainKhr);
}


void VK_NP::VulkanPresentChain::WaitForAvailableImage(uint32_t _currentFrame)
{
    VK_CALL(m_DeviceConstPtr.waitForFences(1, &m_SwapChainSyncObject[_currentFrame].inFlightFence, VK_TRUE, UINT64_MAX));

    VK_CALL(m_DeviceConstPtr.resetFences(1, &m_SwapChainSyncObject[_currentFrame].inFlightFence));
}

void VK_NP::VulkanPresentChain::AquireNetImageKHR(uint32_t _currentFrame)
{
    
    VK_CALL(m_DeviceConstPtr.acquireNextImageKHR(m_SwapchainKhr, UINT64_MAX, m_SwapChainSyncObject[_currentFrame].imageAvailableSemaphore, VK_NULL_HANDLE,  &m_ImageIndex));
}


void VK_NP::VulkanPresentChain::SwapBuffer(uint32_t _currentFrame)
{
    PresentNewImage(_currentFrame);
}

vk::Semaphore* VK_NP::VulkanPresentChain::GetImageAvailableSemaphore(uint32_t _currentFrame)
{
    return &m_SwapChainSyncObject[static_cast<size_t>(_currentFrame)].imageAvailableSemaphore;
}

vk::Semaphore* VK_NP::VulkanPresentChain::GetRenderFinishedSemaphore(uint32_t _currentFrame)
{
    return &m_SwapChainSyncObject[static_cast<size_t>(_currentFrame)].renderFinishedSemaphore;
}

vk::Fence* VK_NP::VulkanPresentChain::GetInFlightFence(uint32_t _currentFrame)
{
    return &m_SwapChainSyncObject[static_cast<size_t>(_currentFrame)].inFlightFence;
}

uint32_t VK_NP::VulkanPresentChain::GetImageIndex()
{
    return m_VulkanPresentChainIntance->m_ImageIndex;
}

vk::Extent2D VK_NP::VulkanPresentChain::GetExtent()
{
    return m_VulkanPresentChainIntance->m_Extent2D;
}

vk::Format VK_NP::VulkanPresentChain::GetSwapChainFormat()
{
    return m_VulkanPresentChainIntance->m_SurfaceFormat.format;
}

vk::RenderPass VK_NP::VulkanPresentChain::GetRenderPassTmpr()
{
    return m_VulkanPresentChainIntance->m_RenderPass;
}

vk::Framebuffer VK_NP::VulkanPresentChain::GetFramebuffer(size_t index)
{
    return  m_VulkanPresentChainIntance->m_SwapChainFramebuffers.at(index);
}

vk::SurfaceFormatKHR VK_NP::VulkanPresentChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& _availableFormats)
{
    for (const auto& availableFormat : _availableFormats)
    {
        if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
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
   
    for (const auto& availablePresentMode : _availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            return vk::PresentModeKHR::eMailbox;
        
    }

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

void VK_NP::VulkanPresentChain::CreateSwapchainImages(const vk::Device _device)
{
    m_SwapChainImageViews.resize(m_SwapchainImageCount);

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

void VK_NP::VulkanPresentChain::CreateFramebuffers()
{
    m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());
    
    auto device = VulkanHarwareWrapper::GetDevice();
    
    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        vk::ImageView attachments[] = {
            m_SwapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Extent2D.width;
        framebufferInfo.height = m_Extent2D.height;
        framebufferInfo.layers = 1;
        
        VK_CALL(device.createFramebuffer(&framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]));
    }
}

void VK_NP::VulkanPresentChain::CreateRenderPass()
{
    auto device = VulkanHarwareWrapper::GetDevice();  // Assuming this returns a vk::Device

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = m_SurfaceFormat.format;  
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;  
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;  

    m_RenderPass = device.createRenderPass(renderPassInfo);
}

void VK_NP::VulkanPresentChain::CreateSyncObject()
{

    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CALL(m_DeviceConstPtr.createSemaphore(&semaphoreCreateInfo,nullptr , &m_SwapChainSyncObject[i].imageAvailableSemaphore));
        VK_CALL(m_DeviceConstPtr.createSemaphore(&semaphoreCreateInfo, nullptr, &m_SwapChainSyncObject[i].renderFinishedSemaphore));
        VK_CALL(m_DeviceConstPtr.createFence(&fenceCreateInfo, nullptr, &m_SwapChainSyncObject[i].inFlightFence));
    }
}

void VK_NP::VulkanPresentChain::PresentNewImage(uint32_t _currentFrame)
{
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_SwapChainSyncObject[_currentFrame].renderFinishedSemaphore;

    // Swapchain and image index
    vk::SwapchainKHR swapChains[] = {m_SwapchainKhr};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_ImageIndex;

    vk::Result result = VulkanHarwareWrapper::GetPresentQueu().presentKHR(&presentInfo);
}



