#include "back_end/vulkan_present_chain.hpp"
#include "back_end/vulkan_harware_wrapper.hpp"
#include "GLFW/glfw3.h"

VK_NP::VulkanPresentChain::VulkanPresentChain(const VulkanAppCreateInfo& _vulkanMainCreateInfo)
{
    VulkanContext* vulkanContext = VulkanContext::currentContext; 
    
    CreateSwapchain(_vulkanMainCreateInfo.windowPtr, vulkanContext);
    CreateSyncObject(vulkanContext);
}


void VK_NP::VulkanPresentChain::DestroySyncObject(VulkanContext* _vulkanContext)
{
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        _vulkanContext->device.destroySemaphore(_vulkanContext->m_syncObject[i].imageAvailableSemaphore);
        _vulkanContext->device.destroySemaphore(_vulkanContext->m_syncObject[i].renderFinishedSemaphore);
        _vulkanContext->device.destroyFence(_vulkanContext->m_syncObject[i].inFlightFence);
    }
}

VK_NP::VulkanPresentChain::~VulkanPresentChain()
{
    VulkanContext* vulkanContext = VulkanContext::currentContext; 

    DestroySyncObject(vulkanContext);
    DestroySwapchain(vulkanContext);
    vulkanContext->device.destroyRenderPass(vulkanContext->swapChainRenderPass);
}

void VK_NP::VulkanPresentChain::RecreateSwapChain(void* _glfwWindowPtr  ,uint32_t _newWidht, uint32_t _newHeight)
{
    VulkanContext* vulkanContext = VulkanContext::currentContext; 

    DestroySwapchain(vulkanContext);
    CreateSwapchain(_glfwWindowPtr, vulkanContext);
}

void VK_NP::VulkanPresentChain::CreateSwapchain(void* _glfwWindowPtr, VulkanContext* _vulkanContext)
{
    vk::PhysicalDevice phyDevice = _vulkanContext->physicalDevice;
    vk::Device device = _vulkanContext->device;
    vk::SurfaceKHR surface = _vulkanContext->surface;
    const SwapChainSupportDetails& swapChainSupportDetails = _vulkanContext->swapChainSupportDetails;
    const QueuFamiliesIndicies& queuFamiliesIndicies = _vulkanContext->queuFamiliesIndicies;
        
    uint32_t queueFamilyIndices[] = {queuFamiliesIndicies.graphicsFamily, queuFamiliesIndicies.presentFamily};
    
    _vulkanContext->m_SurfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    _vulkanContext->m_PresentMode = ChoosePresentMode(swapChainSupportDetails.presentModes);
    _vulkanContext->m_Extent2D = ChooseSwapExtent(static_cast<GLFWwindow*>(_glfwWindowPtr), swapChainSupportDetails.capabilities);

    _vulkanContext->swapChainImageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
    
     if (swapChainSupportDetails.capabilities.maxImageCount > 0 && _vulkanContext->swapChainImageCount > swapChainSupportDetails.capabilities.maxImageCount)
     {
         _vulkanContext->swapChainImageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    
    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = _vulkanContext->swapChainImageCount;
    swapChainCreateInfo.imageFormat = _vulkanContext->m_SurfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = _vulkanContext->m_SurfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = _vulkanContext->m_Extent2D;
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

    swapChainCreateInfo.presentMode = _vulkanContext->m_PresentMode;
    swapChainCreateInfo.clipped = vk::True;
    swapChainCreateInfo.oldSwapchain = nullptr;

    VK_CALL(device.createSwapchainKHR(&swapChainCreateInfo, nullptr, &_vulkanContext->swapChain));

   
    // Important to be here tmpr
    CreateRenderPass(_vulkanContext);
    // Create SwapChain Image
    _vulkanContext->m_SwapchainImages = device.getSwapchainImagesKHR(_vulkanContext->swapChain);
    CreateSwapchainImages(_vulkanContext);
    CreateFramebuffers(_vulkanContext);
    
}

void VK_NP::VulkanPresentChain::DestroySwapchain(VulkanContext* _vulkanContext)
{

    for (auto& framebuffer : _vulkanContext->m_SwapChainFramebuffers)
    {
        _vulkanContext->device.destroyFramebuffer(framebuffer, nullptr);
    }
    
    for (auto& imageView : _vulkanContext->m_SwapChainImageViews)
    {
        _vulkanContext->device.destroyImageView(imageView, nullptr);
    }

    
    _vulkanContext->device.destroySwapchainKHR(_vulkanContext->swapChain);
}


void VK_NP::VulkanPresentChain::WaitForAvailableImage(VulkanContext* _vulkanContext)
{
    const uint32_t currentFrame = _vulkanContext->currentFrame;
    VK_CALL(_vulkanContext->device.waitForFences(1, &_vulkanContext->m_syncObject[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX));

    VK_CALL(_vulkanContext->device.resetFences(1, &_vulkanContext->m_syncObject[currentFrame].inFlightFence));
}

void VK_NP::VulkanPresentChain::AquireNetImageKHR(VulkanContext* _vulkanContext)
{
    const uint32_t currentFrame = _vulkanContext->currentFrame;
    uint32_t index = -1;
    VK_CALL(_vulkanContext->device.acquireNextImageKHR(_vulkanContext->swapChain, UINT64_MAX, _vulkanContext->m_syncObject[currentFrame].imageAvailableSemaphore, VK_NULL_HANDLE,  &index));
    _vulkanContext->imageIndex = index;
}


void VK_NP::VulkanPresentChain::SwapBuffer(vk::CommandBuffer* _commandBuffers, uint32_t _bufferCount,
    VulkanContext* _vulkanContext)
{
    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;


    vk::Semaphore waitSemaphores[] = {
        _vulkanContext->m_syncObject.at(_vulkanContext->currentFrame).imageAvailableSemaphore
    };
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = _bufferCount;
    submitInfo.pCommandBuffers = _commandBuffers;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_vulkanContext->m_syncObject.at(_vulkanContext->currentFrame).
                                                    renderFinishedSemaphore;

    
    VK_CALL(_vulkanContext->graphicQueue.submit(1, &submitInfo,
        _vulkanContext->m_syncObject.at(_vulkanContext->currentFrame).inFlightFence));
    
    PresentNewImage(_vulkanContext);
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

void VK_NP::VulkanPresentChain::CreateSwapchainImages(VulkanContext* _vulkanContext)
{
    _vulkanContext->m_SwapChainImageViews.resize(_vulkanContext->swapChainImageCount);

    for (size_t i = 0; i < _vulkanContext->m_SwapchainImages.size(); i++)
    {
        vk::ImageViewCreateInfo viewInfo = {};
        viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
        viewInfo.image = _vulkanContext->m_SwapchainImages[i];
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = _vulkanContext->m_SurfaceFormat.format;

        viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.a = vk::ComponentSwizzle::eIdentity;

        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CALL(_vulkanContext->device.createImageView(&viewInfo, nullptr, &_vulkanContext->m_SwapChainImageViews[i]));
    }
}



void VK_NP::VulkanPresentChain::CreateFramebuffers(VulkanContext* _vulkanContext)
{
    _vulkanContext->m_SwapChainFramebuffers.resize(_vulkanContext->m_SwapchainImages.size());
    
    
    for (size_t i = 0; i < _vulkanContext->m_SwapChainFramebuffers.size(); i++)
    {
        vk::ImageView attachments[] = {
            _vulkanContext->m_SwapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferInfo.renderPass = _vulkanContext->swapChainRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _vulkanContext->m_Extent2D.width;
        framebufferInfo.height = _vulkanContext->m_Extent2D.height;
        framebufferInfo.layers = 1;
        
        VK_CALL(_vulkanContext->device.createFramebuffer(&framebufferInfo, nullptr, &_vulkanContext->m_SwapChainFramebuffers[i]));
    }
}

void VK_NP::VulkanPresentChain::CreateRenderPass(VulkanContext* _vulkanContext)
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = _vulkanContext->m_SurfaceFormat.format;  
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

    _vulkanContext->swapChainRenderPass = _vulkanContext->device.createRenderPass(renderPassInfo);
}

void VK_NP::VulkanPresentChain::CreateSyncObject(VulkanContext* _vulkanContext)
{

    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CALL(_vulkanContext->device.createSemaphore(&semaphoreCreateInfo,nullptr , &_vulkanContext->m_syncObject[i].imageAvailableSemaphore));
        VK_CALL(_vulkanContext->device.createSemaphore(&semaphoreCreateInfo, nullptr, &_vulkanContext->m_syncObject[i].renderFinishedSemaphore));
        VK_CALL(_vulkanContext->device.createFence(&fenceCreateInfo, nullptr, &_vulkanContext->m_syncObject[i].inFlightFence));
    }
}



void VK_NP::VulkanPresentChain::PresentNewImage(VulkanContext* _vulkanContext)
{
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_vulkanContext->m_syncObject[_vulkanContext->currentFrame].renderFinishedSemaphore;

    // Swapchain and image index
    vk::SwapchainKHR swapChains[] = {_vulkanContext->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &_vulkanContext->imageIndex;

    vk::Result result =_vulkanContext->presentQueue.presentKHR(&presentInfo);
}



