#include "front_end/vulkan_present_chain.hpp"

#include "back_end/vulkan_image.hpp"
#include "back_end/vulkan_transition_image_layout.hpp"
#include "front_end/vulkan_harware_wrapper.hpp"
#include "GLFW/glfw3.h"


void Vulkan::VulkanPresentChain::DestroySyncObject(VulkanContext* _vulkanContext)
{
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        _vulkanContext->device.destroySemaphore(_vulkanContext->m_syncObject[i].imageAvailableSemaphore);
        _vulkanContext->device.destroySemaphore(_vulkanContext->m_syncObject[i].renderFinishedSemaphore);
        _vulkanContext->device.destroyFence(_vulkanContext->m_syncObject[i].inFlightFence);
    }
}


void Vulkan::VulkanPresentChain::Init(const VulkanAppCreateInfo& _vulkanMainCreateInfo, VulkanContext* _vulkanContext)
{
    const SwapChainSupportDetails& swapChainSupportDetails = _vulkanContext->swapChainSupportDetails;
    _vulkanContext->m_SurfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    _vulkanContext->m_PresentMode = ChoosePresentMode(swapChainSupportDetails.presentModes);

    CreateRenderPass(_vulkanContext);
    CreateSwapchain(_vulkanMainCreateInfo.windowPtr, _vulkanContext);
    CreateSyncObject(_vulkanContext);
}

void Vulkan::VulkanPresentChain::Destroy(VulkanContext* _context)
{
    DestroySyncObject(_context);
    DestroySwapchain(_context);
    _context->device.destroyRenderPass(_context->swapChainRenderPass);
}

void Vulkan::VulkanPresentChain::RecreateSwapChain(VulkanContext* _context, void* _glfwWindowPtr ,uint32_t _newWidht, uint32_t _newHeight)
{
    DestroySwapchain(_context);
    CreateSwapchain(_glfwWindowPtr, _context);
}

void Vulkan::VulkanPresentChain::CreateSwapchain(void* _glfwWindowPtr, VulkanContext* _vulkanContext)
{
    vk::PhysicalDevice phyDevice = _vulkanContext->physicalDevice;
    vk::Device device = _vulkanContext->device;
    vk::SurfaceKHR surface = _vulkanContext->surface;
    
    const SwapChainSupportDetails& swapChainSupportDetails = _vulkanContext->swapChainSupportDetails;
    _vulkanContext->extent2D = ChooseSwapExtent(static_cast<GLFWwindow*>(_glfwWindowPtr));
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
    swapChainCreateInfo.imageExtent = _vulkanContext->extent2D;
    swapChainCreateInfo.imageArrayLayers = 1;
    // MAY CHANGE IN FUTURE
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;

    const QueuFamiliesIndicies& queuFamiliesIndicies = _vulkanContext->queuFamiliesIndicies;
    uint32_t queueFamilyIndices[] = {queuFamiliesIndicies.graphicsFamily, queuFamiliesIndicies.presentFamily};
    
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

   
    // Create SwapChain Image
    _vulkanContext->m_SwapchainImages = device.getSwapchainImagesKHR(_vulkanContext->swapChain);
    CreateSwapchainImages(_vulkanContext);
    CreateDepthResources(_vulkanContext);
    CreateFramebuffers(_vulkanContext);
    
}

void Vulkan::VulkanPresentChain::DestroySwapchain(VulkanContext* _vulkanContext)
{

    for (auto& framebuffer : _vulkanContext->m_SwapChainFramebuffers)
    {
        _vulkanContext->device.destroyFramebuffer(framebuffer, nullptr);
    }
    
    for (auto& imageView : _vulkanContext->m_SwapChainImageViews)
    {
        _vulkanContext->device.destroyImageView(imageView, nullptr);
    }

    DestroyDepthResources(_vulkanContext);
    
    _vulkanContext->device.destroySwapchainKHR(_vulkanContext->swapChain);
}


void Vulkan::VulkanPresentChain::WaitForAvailableImage(VulkanContext* _vulkanContext)
{
    const uint32_t currentFrame = _vulkanContext->currentFrame;
    VK_CALL(_vulkanContext->device.waitForFences(1, &_vulkanContext->m_syncObject[currentFrame].inFlightFence, VK_TRUE, UINT64_MAX));

    VK_CALL(_vulkanContext->device.resetFences(1, &_vulkanContext->m_syncObject[currentFrame].inFlightFence));
}

void Vulkan::VulkanPresentChain::AquireNetImageKHR(VulkanContext* _vulkanContext)
{
    const uint32_t currentFrame = _vulkanContext->currentFrame;
    uint32_t index = -1;
    VK_CALL(_vulkanContext->device.acquireNextImageKHR(_vulkanContext->swapChain, UINT64_MAX, _vulkanContext->m_syncObject[currentFrame].imageAvailableSemaphore, VK_NULL_HANDLE,  &index));
    _vulkanContext->imageIndex = index;
}


void Vulkan::VulkanPresentChain::SwapBuffer(vk::CommandBuffer* _commandBuffers, uint32_t _bufferCount,
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

    
    VK_CALL(_vulkanContext->vkQueues.graphicQueue.submit(1, &submitInfo,
        _vulkanContext->m_syncObject.at(_vulkanContext->currentFrame).inFlightFence));
    
    PresentNewImage(_vulkanContext);
}

vk::SurfaceFormatKHR Vulkan::VulkanPresentChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& _availableFormats)
{
    for (const auto& availableFormat : _availableFormats)
    {
        // eR8G8B8Unorm for imgui
        if (availableFormat.format == vk::Format::eR8G8B8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
        {
            return availableFormat;
        }
    }
    
    return _availableFormats[0];
}

vk::PresentModeKHR Vulkan::VulkanPresentChain::ChoosePresentMode(
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

vk::Extent2D Vulkan::VulkanPresentChain::ChooseSwapExtent(GLFWwindow* _window)
{
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        vk::Extent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    
        return actualExtent;
    
}

void Vulkan::VulkanPresentChain::CreateSwapchainImages(VulkanContext* _vulkanContext)
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



void Vulkan::VulkanPresentChain::CreateFramebuffers(VulkanContext* _vulkanContext)
{
    _vulkanContext->m_SwapChainFramebuffers.resize(_vulkanContext->m_SwapchainImages.size());
    
    
    for (size_t i = 0; i < _vulkanContext->m_SwapChainFramebuffers.size(); i++)
    {
        std::array<vk::ImageView, 2> attachments[] = {
            _vulkanContext->m_SwapChainImageViews[i]
            , _vulkanContext->depthImageView
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferInfo.renderPass = _vulkanContext->swapChainRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments->size());
        framebufferInfo.pAttachments = attachments->data();
        framebufferInfo.width = _vulkanContext->extent2D.width;
        framebufferInfo.height = _vulkanContext->extent2D.height;
        framebufferInfo.layers = 1;
        
        VK_CALL(_vulkanContext->device.createFramebuffer(&framebufferInfo, nullptr, &_vulkanContext->m_SwapChainFramebuffers[i]));
    }
}

void Vulkan::VulkanPresentChain::CreateRenderPass(VulkanContext* _vulkanContext)
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

    const vk::Format detphFormat = VulkanPhysicalDevices::FindDepthFormat(_vulkanContext->physicalDevice);

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = detphFormat;
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp =  vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;  
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;  

    _vulkanContext->swapChainRenderPass = _vulkanContext->device.createRenderPass(renderPassInfo);
}

void Vulkan::VulkanPresentChain::CreateSyncObject(VulkanContext* _vulkanContext)
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



void Vulkan::VulkanPresentChain::PresentNewImage(VulkanContext* _vulkanContext)
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
    
    VK_CALL(_vulkanContext->vkQueues.presentQueue.presentKHR(&presentInfo));
}

void Vulkan::VulkanPresentChain::CreateDepthResources(VulkanContext* _vulkanContext)
{
    const vk::Format detphFormat = VulkanPhysicalDevices::FindDepthFormat(_vulkanContext->physicalDevice);
    
    Backend::CreateImage(_vulkanContext, _vulkanContext->extent2D.width, _vulkanContext->extent2D.height, 1,
        vk::ImageType::e2D, detphFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
        VMA_MEMORY_USAGE_AUTO, &_vulkanContext->depthImage, &_vulkanContext->depthImageAllocation);

    Backend::TransitionImageLayout(_vulkanContext, _vulkanContext->depthImage, vk::ImageAspectFlagBits::eDepth |  vk::ImageAspectFlagBits::eStencil , 1, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    
    vk::ImageViewCreateInfo viewCreateInfo{};
    viewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
    viewCreateInfo.image = _vulkanContext->depthImage;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = detphFormat;
    viewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    
    VK_CALL(_vulkanContext->device.createImageView(&viewCreateInfo, nullptr, &_vulkanContext->depthImageView));
}

void Vulkan::VulkanPresentChain::DestroyDepthResources(VulkanContext* _vulkanContext)
{
    vmaDestroyImage(_vulkanContext->allocator, _vulkanContext->depthImage, _vulkanContext->depthImageAllocation);
    _vulkanContext->device.destroyImageView(_vulkanContext->depthImageView);
}



