#include "VulkanSwapChain.hpp"

#include "Glfw/Glfw3.h"
#include "LowRenderer/Rhi.hpp"
#include "VulkanCommandList.hpp"
#include "VulkanContext.hpp"
#include "VulkanFrameBuffer.hpp"

#include <Utils/RhiToVulkan.hpp>

void* Vulkan::VulkanSwapChain::GetFrameBuffer()
{
    return m_Framebuffers.at(m_SwapChainImageIndex);
}

Vulkan::VulkanSwapChain::VulkanSwapChain(PC_CORE::Rhi& _Rhi, uint32_t _Widht, 
    uint32_t _Height, VulkanPhysicalDevices& vulkanPhysicalDevices, VulkanDevice& _VulkanDevice, vk::SurfaceKHR _Surface)
    : RhiSwapChain(_Rhi)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    m_Surface = _Surface;
    m_Device = _VulkanDevice.GetDevice();
    FromSurface(vulkanPhysicalDevices, _Surface, _Widht, _Height);

    const auto& queueFamilyIndices = vulkanPhysicalDevices.GetQueuesFamilies();
    m_QueueFamilyIndices = 0;
    for (int i = static_cast<int>(queueFamilyIndices.size()) - 1; i >= 0; i--)
    {
        if (queueFamilyIndices[static_cast<size_t>(i)].supportPresent)
        {
            m_QueueFamilyIndices = static_cast<uint32_t>(i);
            break;
        }
    }

    m_SwapChainRenderPass = std::make_shared<VulkanRenderPass>(m_Rhi, _VulkanDevice.GetDevice(), m_SurfaceFormatKHR.format);
    m_SwapChainRenderPass
        ->SetName("SwapChainRenderPass");
       // .Build(); DO NOT CALL IT

    CreateSwapChain();
    CreateImageViews();
    CreateFrameBuffers();

    m_ImagesInFligh.resize(m_SwapChainImageCount);
}

Vulkan::VulkanSwapChain::~VulkanSwapChain()
{
    CleanUpSwapChain();
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::GetSurfaceFormat()
{
    return m_SurfaceFormatKHR;
}


bool Vulkan::VulkanSwapChain::GetSwapChainImageIndex(PC_CORE::Window* windowHandle)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    VulkanContext& context = GET_VK_CONTEXT;
    vk::Device device = context.GetDevice()->GetDevice();
    uint32_t frameIndex = m_Rhi.GetFrameIndex();

    // Wait for the CPU/GPU pacing fence (frame reuse)
    device.waitForFences(
        1,
        &context.syncObjects[frameIndex].inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    uint32_t imageIndex;
    vk::Result result = device.acquireNextImageKHR
    (   m_SwapChain, 
        UINT64_MAX, 
        context.syncObjects[frameIndex].imageAvailableSemaphore, 
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        HandleRecreateSwapChain(windowHandle);
        return false;
    }
    else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
        VK_CALL(result);
    }

    // If this swapchain image is already in flight, wait for it
     // because acquireNextImageKHR can not give sequencal image index 
     // Exexpect : 0 1 2 0
     // Reality : 0 1 0 1 2
    if (m_ImagesInFligh[imageIndex] != VK_NULL_HANDLE)
    {
        VK_CALL(device.waitForFences
        (
            1,
            &m_ImagesInFligh[imageIndex],
            VK_TRUE,
            UINT64_MAX
        ));
    }

    // Mark image as now using this frame's fence
    m_ImagesInFligh[imageIndex] =
        context.syncObjects[frameIndex].inFlightFence;

    // Reset fence before submitting new work
    VK_CALL(device.resetFences
    (
        1,
        &context.syncObjects[frameIndex].inFlightFence
    ));

    m_SwapChainImageIndex = imageIndex;

    return true;
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

void Vulkan::VulkanSwapChain::FromSurface(const VulkanPhysicalDevices& VulkanPhysicalDevices, vk::SurfaceKHR _Surface, 
    uint32_t _Width, uint32_t _Height)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    m_SwapChainSupportDetails = VulkanPhysicalDevices.GetSwapChainSupport(_Surface);

    m_SurfaceFormatKHR = ChooseSwapSurfaceFormat(m_SwapChainSupportDetails.formats);
    m_PresentModeKHR = ChooseSwapPresentMode(m_SwapChainSupportDetails.presentModes);
    m_Extent2D = ChooseSwapExtent(m_SwapChainSupportDetails.capabilities, _Width, _Height);
    m_SwapChainImageCount = m_SwapChainSupportDetails.capabilities.minImageCount + 1;
    if (m_SwapChainSupportDetails.capabilities.maxImageCount > 0 && m_SwapChainImageCount > m_SwapChainSupportDetails.capabilities.
        maxImageCount)
    {
        m_SwapChainImageCount = m_SwapChainSupportDetails.capabilities.maxImageCount;
    }

    if (m_SwapChainSupportDetails.formats.empty() || m_SwapChainSupportDetails.presentModes.empty())
    {
        PC_LOGCRITICAL("Unsuported swapChain formats or presents modes");
        return;
    }

}

void Vulkan::VulkanSwapChain::CreateImageViews()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    m_SwapChainImageViews.resize(m_SwapChainImage.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eImageViewCreateInfo;
        createInfo.image = m_SwapChainImage[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_SurfaceFormatKHR.format;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        m_SwapChainImageViews[i] = m_Device.createImageView(createInfo);
    }
}

void Vulkan::VulkanSwapChain::CreateFrameBuffers()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

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

        m_Framebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
    }
}


void Vulkan::VulkanSwapChain::CleanUpSwapChain()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    for (const auto& frameBuffer : m_Framebuffers)
        m_Device.destroyFramebuffer(frameBuffer);

    for (const auto& swapChainImageView : m_SwapChainImageViews)
        m_Device.destroyImageView(swapChainImageView);


    m_Device.destroySwapchainKHR(m_SwapChain);
}

void Vulkan::VulkanSwapChain::CreateSwapChain()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapChainCreateInfo.surface = m_Surface;
    swapChainCreateInfo.minImageCount = m_SwapChainImageCount;
    swapChainCreateInfo.imageFormat = m_SurfaceFormatKHR.format;
    swapChainCreateInfo.imageColorSpace = m_SurfaceFormatKHR.colorSpace;
    swapChainCreateInfo.imageExtent = m_Extent2D;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;


    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapChainCreateInfo.queueFamilyIndexCount = 1;
    swapChainCreateInfo.pQueueFamilyIndices = &m_QueueFamilyIndices;
    swapChainCreateInfo.preTransform = m_SwapChainSupportDetails.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCreateInfo.presentMode = m_PresentModeKHR;
    swapChainCreateInfo.clipped = vk::True;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;


    m_SwapChain = m_Device.createSwapchainKHR(swapChainCreateInfo, nullptr);

    VK_CALL(m_Device.getSwapchainImagesKHR(m_SwapChain, &m_SwapChainImageCount, nullptr));
    m_SwapChainImage.resize(m_SwapChainImageCount);
    VK_CALL(m_Device.getSwapchainImagesKHR(m_SwapChain, &m_SwapChainImageCount, m_SwapChainImage.data()));

    m_SwapChainImageCount = m_SwapChainImage.size();
}

bool Vulkan::VulkanSwapChain::Build()
{
    return true; // TODO
}

uint32_t Vulkan::VulkanSwapChain::GetWidth() const
{
    return m_Extent2D.width;
}

uint32_t Vulkan::VulkanSwapChain::GetHeight() const
{
    return m_Extent2D.height;
}

vk::SwapchainKHR Vulkan::VulkanSwapChain::GetVulkanSwapChain() const
{
    return m_SwapChain;
}

void Vulkan::VulkanSwapChain::Present(PC_CORE::Window* _window)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    const uint32_t frameIndex = m_Rhi.GetFrameIndex();

    VulkanContext& context = GET_VK_CONTEXT;
    vk::Device device = context.GetDevice()->GetDevice();
    vk::Queue mainQueu = context.mainQueue;


    uint32_t WaitSemaphoreCount = 0;
    std::array<vk::Semaphore, 2> waitSemaphore = {};
    std::array<vk::PipelineStageFlags, 2> waitPipelineStageImageAvailable = {};

    // Transfer
    if (context.PendingTransferOperation())
    {
        waitSemaphore[WaitSemaphoreCount] = context.syncObjects[frameIndex].transferFinishSemaphore;
        waitPipelineStageImageAvailable[WaitSemaphoreCount] = vk::PipelineStageFlagBits::eTransfer;
        WaitSemaphoreCount++;
    }
    
    // SwapChain
    waitSemaphore[WaitSemaphoreCount] = context.syncObjects[frameIndex].imageAvailableSemaphore;
    waitPipelineStageImageAvailable[WaitSemaphoreCount] = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    WaitSemaphoreCount++;

    vk::Semaphore signalSemaphores[] = { context.syncObjects[m_SwapChainImageIndex].renderFinishedSemaphore };
    // Handle all user Command list
    context.SubmitInfoBuffer.clear();
    context.SubmitInfoBuffer.resize(context.flushedCommands.Commands.size());
    for (size_t i = 0; i < context.SubmitInfoBuffer.size(); i++)
    {
        vk::SubmitInfo& submitInfo = context.SubmitInfoBuffer[i];
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        // Wait previous Work
        if (i == 0)
        {
            submitInfo.waitSemaphoreCount = WaitSemaphoreCount;
            submitInfo.pWaitSemaphores = waitSemaphore.data();
            submitInfo.pWaitDstStageMask = waitPipelineStageImageAvailable.data();
        }
        else
        {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &context.flushedCommands.Semaphores[i - 1];
            submitInfo.pWaitDstStageMask = &context.flushedCommands.BatchPipelineStageFlag[i - 1]; // wait the previous 
        }
 
        // Signal n + 1 work
        if (i == context.SubmitInfoBuffer.size() - 1)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
        }
        else
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &context.flushedCommands.Semaphores[i];
        }

        // Current Command
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &context.flushedCommands.Commands[i];
    }
   
    VK_CALL(context.mainQueue.submit(static_cast<uint32_t>(context.SubmitInfoBuffer.size()), context.SubmitInfoBuffer.data(), context.syncObjects[frameIndex].inFlightFence));

    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {m_SwapChain};
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

    context.flushedCommands.Clear();
}

void Vulkan::VulkanSwapChain::HandleRecreateSwapChain(PC_CORE::Window* windowHandle)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    if (!windowHandle->resizeDirty)
        return;

    GET_VK_DEVICE.waitIdle();

    Tbx::Vector2ui size = windowHandle->GetWindowSize();
    CleanUpSwapChain();

    std::shared_ptr<VulkanPhysicalDevices> physicalDevice = GET_VK_CONTEXT.GetPhysicalDevices();

    FromSurface(*physicalDevice, m_Surface, size.x, size.y);
    CreateSwapChain();
    CreateImageViews();
    CreateFrameBuffers();
    windowHandle->resizeDirty = false;
}

void Vulkan::VulkanSwapChain::BeginSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    auto vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);

    std::shared_ptr<VulkanRenderPass> renderPass = reinterpret_pointer_cast<VulkanRenderPass>(m_SwapChainRenderPass);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = m_Framebuffers.at(m_SwapChainImageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = m_Extent2D;

    vk::ClearValue clearColor = {};
    clearColor.color.setFloat32({
        0, 0, 0, 0
    });

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vcommandList->GetVulkanCommandBufferHandle().beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanSwapChain::EndSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    auto vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);
    vcommandList->GetVulkanCommandBufferHandle().endRenderPass();
}
