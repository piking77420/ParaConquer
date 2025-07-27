#include "vulkan_swap_chain.hpp"

#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_physical_devices.hpp"
#include "GLFW/glfw3.h"
#include "low_renderer/rhi.hpp"


void* Vulkan::VulkanSwapChain::GetFrameBuffer()
{
    return m_Framebuffers.at(m_SwapChainImageIndex);
}

Vulkan::VulkanSwapChain::VulkanSwapChain(uint32_t _widht, uint32_t _height): SwapChain(_widht, _height)
{
    PERF_REGION_SCOPED;
    CreateSwapChain(m_SwapChainWidth, m_SwapChainHeight);
    CreateImageViews();
    CreateFrameBuffers();
}

Vulkan::VulkanSwapChain::~VulkanSwapChain()
{
    CleanUpSwapChain();
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::GetSurfaceFormat()
{
    return m_SurfaceFormat;
}



void Vulkan::VulkanSwapChain::GetSwapChainImageIndex(PC_CORE::Window* windowHandle)
{
    VulkanContext& context = VulkanContext::GetContext();

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice);
    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    std::array<vk::Fence, 2> inflightFence = { context.syncObjects[frameIndex].inFlightFence,
        context.syncObjects[frameIndex].computeInFlightFence };

    vk::Semaphore imageAvaibleSemaphore = context.syncObjects[frameIndex].imageAvailableSemaphore;

	VK_CALL(vulkanDevice->GetDevice().waitForFences(inflightFence.size(), inflightFence.data(), VK_TRUE, UINT64_MAX));

	uint32_t nextImageIndex = 0;
	vk::Result result = vulkanDevice->GetDevice().acquireNextImageKHR(m_SwapChain, UINT64_MAX,imageAvaibleSemaphore, 
        VK_NULL_HANDLE, &nextImageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        HandleRecreateSwapChain(windowHandle);
    }
    else if (vk::Result::eSuccess != result)
    {
        VK_CALL(result);
    }

    m_SwapChainImageIndex = nextImageIndex;
    VK_CALL(vulkanDevice->GetDevice().resetFences(inflightFence.size(), inflightFence.data()));
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eR8G8B8A8Unorm && availableFormat.colorSpace ==
            vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR Vulkan::VulkanSwapChain::ChooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Vulkan::VulkanSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t _width,
                                                       uint32_t _height)
{
    vk::Extent2D actualExtent =
    {
        _width,
        _height
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                    capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                     capabilities.maxImageExtent.height);

    return actualExtent;
}

void Vulkan::VulkanSwapChain::CreateImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImage.size());

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eImageViewCreateInfo;
        createInfo.image = m_SwapChainImage[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_SurfaceFormat.format;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        m_SwapChainImageViews[i] = vulkanDevice->GetDevice().createImageView(createInfo);
    }
}

void Vulkan::VulkanSwapChain::CreateFrameBuffers()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);
    m_Framebuffers.resize(m_SwapChainImage.size());

    for (size_t i = 0; i < m_SwapChainImage.size(); i++)
    {
        std::array<vk::ImageView, 1> attachments = {
            m_SwapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferInfo.renderPass = std::reinterpret_pointer_cast<VulkanRenderPass>(m_SwapChainRenderPass)->
            GetVulkanRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_Extent2D.width;
        framebufferInfo.height = m_Extent2D.height;
        framebufferInfo.layers = 1;

        vk::Framebuffer framebuffer = vulkanDevice->GetDevice().createFramebuffer(framebufferInfo);
        m_Framebuffers[i] = framebuffer;
    }
}


void Vulkan::VulkanSwapChain::CleanUpSwapChain()
{
    VulkanContext& vulkanContext = VulkanContext::GetContext();
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice);

    m_SwapChainRenderPass.reset();
    m_SwapChainRenderPass = nullptr;

    for (const auto& frameBuffer : m_Framebuffers)
        device->GetDevice().destroyFramebuffer(frameBuffer);
    
    for (const auto& swapChainImageView : m_SwapChainImageViews)
        device->GetDevice().destroyImageView(swapChainImageView);

    device->GetDevice().destroySwapchainKHR(m_SwapChain);
}

void Vulkan::VulkanSwapChain::CreateSwapChain(uint32_t _width, uint32_t _height)
{
    m_SwapChainWidth = _width;
    m_SwapChainHeight = _height;

    VulkanContext& vulkanContext = reinterpret_cast<VulkanContext&>(VulkanContext::GetContext());

    const Vulkan::SwapChainSupportDetails swapChainSupportDetails = std::reinterpret_pointer_cast<
        VulkanPhysicalDevices>(vulkanContext.physicalDevices)->UpdateSwapChainSupport(vulkanContext.GetSurface());
    
    const auto& queueFamilyIndices = std::reinterpret_pointer_cast<VulkanPhysicalDevices>(vulkanContext.physicalDevices)
        ->GetQueuesFamilies();

    const vk::SurfaceFormatKHR surfaceFormatKHR = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    const vk::PresentModeKHR presentModeKHR = ChooseSwapPresentMode(swapChainSupportDetails.presentModes);


    m_Extent2D = swapChainSupportDetails.capabilities.currentExtent;
    m_SurfaceFormat = surfaceFormatKHR;
    m_SwapChainRenderPass = std::make_shared<VulkanRenderPass>(m_SurfaceFormat.format);


    uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

    if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.
        maxImageCount)
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapChainCreateInfo.surface = vulkanContext.GetSurface();
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormatKHR.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormatKHR.colorSpace;
    swapChainCreateInfo.imageExtent = m_Extent2D;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t presentIndex = 0;
    for (int i = static_cast<int>(queueFamilyIndices.size()) - 1; i >= 0; i--)
    {
        if (queueFamilyIndices[static_cast<size_t>(i)].supportPresent)
        {
            presentIndex = static_cast<uint32_t>(i);
            break;
        }
    }
    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = &presentIndex;
    swapChainCreateInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCreateInfo.presentMode = presentModeKHR;
    swapChainCreateInfo.clipped = vk::True;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;


    vk::Device device = std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice)->GetDevice();
    m_SwapChain = device.createSwapchainKHR(swapChainCreateInfo, nullptr);

    VK_CALL(device.getSwapchainImagesKHR(m_SwapChain, &imageCount, nullptr));
    m_SwapChainImage.resize(imageCount);
    VK_CALL(device.getSwapchainImagesKHR(m_SwapChain, &imageCount, m_SwapChainImage.data()));

    m_SwapChainImageCount = m_SwapChainImage.size();
}

void Vulkan::VulkanSwapChain::Present(PC_CORE::Window* _window)
{
    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    VulkanContext& context = VulkanContext::GetContext();

    vk::Fence inFlightFence = context.syncObjects[frameIndex].inFlightFence;
    vk::Fence computeinFlightFence = context.syncObjects[frameIndex].computeInFlightFence;

    vk::Semaphore imageAvailableSemaphore = context.syncObjects[frameIndex].imageAvailableSemaphore;

    vk::Semaphore computeFinishSemaphore = context.syncObjects[frameIndex].computeFinishedSemaphore;
    vk::Semaphore renderFinishSemaphore = context.syncObjects[frameIndex].renderFinishedSemaphore;

    // Compute Work
    {
        vk::Queue computeQueue = context.computeQueu;

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eComputeShader };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(context.computeCommandBuffer.size());
        submitInfo.pCommandBuffers = context.computeCommandBuffer.data();

        vk::Semaphore signalSemaphores[] = { computeFinishSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_CALL(computeQueue.submit(1, &submitInfo, computeinFlightFence));
    }
    // Graphic Work
    {

        vk::Queue mainQueu = VulkanContext::GetContext().mainQueue;

        vk::SubmitInfo submitInfo{};
        submitInfo.sType = vk::StructureType::eSubmitInfo;


        vk::Semaphore waitSemaphores[] = { computeFinishSemaphore };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(context.renderFrameCommandBuffer.size());
        submitInfo.pCommandBuffers = context.renderFrameCommandBuffer.data();

        vk::Semaphore signalSemaphores[] = { renderFinishSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        VK_CALL(mainQueu.submit(1, &submitInfo, inFlightFence));
    }
  
    std::array<vk::Semaphore, 1> waitForPresentSemaphores = { renderFinishSemaphore };
    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitForPresentSemaphores.size());
    presentInfo.pWaitSemaphores = waitForPresentSemaphores.data();

    vk::SwapchainKHR swapChains[] = { m_SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_SwapChainImageIndex;

    vk::Result result = context.mainQueue.presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
    {
        HandleRecreateSwapChain(_window);
    }
    else if (result != vk::Result::eSuccess)
    {
        VK_CALL(result);
    }

    context.renderFrameCommandBuffer.clear();
    context.computeCommandBuffer.clear();
}

void Vulkan::VulkanSwapChain::HandleRecreateSwapChain(PC_CORE::Window* windowHandle)
{
    if (!windowHandle->resizeDirty)
        return;

    std::shared_ptr<VulkanDevice> vPtrDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        PC_CORE::Rhi::GetRhiContext()->rhiDevice);
    vPtrDevice->GetDevice().waitIdle();

    Tbx::Vector2ui size = windowHandle->GetWindowSize();
    CleanUpSwapChain();
    CreateSwapChain(size.x, size.y);
    CreateImageViews();
    CreateFrameBuffers();
    windowHandle->resizeDirty = false;
}

void Vulkan::VulkanSwapChain::BeginSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    VulkanCommandList* vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);
    
    std::shared_ptr<VulkanRenderPass> renderPass = reinterpret_pointer_cast<VulkanRenderPass>(m_SwapChainRenderPass);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = m_Framebuffers.at(m_SwapChainImageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = vk::Extent2D{m_SwapChainWidth, m_SwapChainHeight};

    vk::ClearValue clearColor  = {};
    clearColor.color.setFloat32({ 
        0, 0, 0, 0});
    
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vcommandList->GetHandle().beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    
}
        
void Vulkan::VulkanSwapChain::EndSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    VulkanCommandList* vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);
    vcommandList->GetHandle().endRenderPass();
}
