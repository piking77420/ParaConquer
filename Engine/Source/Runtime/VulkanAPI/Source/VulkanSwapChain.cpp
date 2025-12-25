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
    : RhiSwapChain(_Rhi, _Widht, _Height)
{
    PERF_REGION_SCOPED;
    m_Surface = _Surface;
    m_SwapChainSupportDetails = vulkanPhysicalDevices.GetSwapChainSupport(m_Surface);
    m_Device = _VulkanDevice.GetDevice();

    if (m_SwapChainSupportDetails.formats.empty() || m_SwapChainSupportDetails.presentModes.empty())
    {
        PC_LOGCRITICAL("Unsuported swapChain formats or presents modes");
        return;
    }


    const auto& queueFamilyIndices = vulkanPhysicalDevices.GetQueuesFamilies();
    m_SurfaceFormatKHR = ChooseSwapSurfaceFormat(m_SwapChainSupportDetails.formats);
    m_PresentModeKHR = ChooseSwapPresentMode(m_SwapChainSupportDetails.presentModes);
    m_Extent2D = m_SwapChainSupportDetails.capabilities.currentExtent;
    m_SwapChainImageCount = m_SwapChainSupportDetails.capabilities.minImageCount + 1;
    if (m_SwapChainSupportDetails.capabilities.maxImageCount > 0 && m_SwapChainImageCount > m_SwapChainSupportDetails.capabilities.
        maxImageCount)
    {
        m_SwapChainImageCount = m_SwapChainSupportDetails.capabilities.maxImageCount;
    }

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
    return m_SurfaceFormatKHR;
}


void Vulkan::VulkanSwapChain::GetSwapChainImageIndex(PC_CORE::Window* windowHandle)
{
    VulkanContext& context = GET_VK_CONTEXT;

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(context.rhiDevice);
    const uint32_t frameIndex = m_Rhi.GetFrameIndex();

    std::array<vk::Fence, 1> inflightFence = {context.syncObjects[frameIndex].inFlightFence};

    vk::Semaphore imageAvaibleSemaphore = context.syncObjects[frameIndex].imageAvailableSemaphore;

    VK_CALL(vulkanDevice->GetDevice().waitForFences(inflightFence.size(), inflightFence.data(), VK_TRUE, UINT64_MAX));

    uint32_t nextImageIndex = 0;
    vk::Result result = vulkanDevice->GetDevice().acquireNextImageKHR(m_SwapChain, UINT64_MAX, imageAvaibleSemaphore,
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
    for (const auto& frameBuffer : m_Framebuffers)
        m_Device.destroyFramebuffer(frameBuffer);

    for (const auto& swapChainImageView : m_SwapChainImageViews)
        m_Device.destroyImageView(swapChainImageView);


    m_Device.destroySwapchainKHR(m_SwapChain);
}

void Vulkan::VulkanSwapChain::CreateSwapChain(uint32_t _width, uint32_t _height)
{
    m_SwapChainWidth = _width;
    m_SwapChainHeight = _height;

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

const void* Vulkan::VulkanSwapChain::GetFrameNativeHandle(size_t _frameIndex) const
{
    return &m_SwapChain;
}

void* Vulkan::VulkanSwapChain::GetFrameNativeHandle(size_t _frameIndex)
{
    return &m_SwapChain;
}

bool Vulkan::VulkanSwapChain::Build()
{
    return true; // TODO
}

void Vulkan::VulkanSwapChain::Present(PC_CORE::Window* _window)
{
    const uint32_t frameIndex = m_Rhi.GetFrameIndex();

    VulkanContext& context = GET_VK_CONTEXT;
    vk::Queue mainQueu = context.mainQueue;
    auto& flushedCommands = context.flushedCommands;

    vk::Fence inFlightFence = context.syncObjects[frameIndex].inFlightFence;
    vk::Semaphore imageAvailableSemaphore = context.syncObjects[frameIndex].imageAvailableSemaphore;
    vk::Semaphore renderFinishSemaphoreImage = context.syncObjects[frameIndex].renderFinishedSemaphore;

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;

    // Graphic Work
    {
        for (size_t i = 0; i < flushedCommands.size(); i++)
        {
            vk::Semaphore waitSemaphore;
            vk::PipelineStageFlags waitStage;

            if (i == 0)
            {
                waitSemaphore = imageAvailableSemaphore;
                waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            }
            else
            {
                waitSemaphore = flushedCommands[i - 1].semaphore;
                waitStage = Utils::RhiPipelineStageToVulkan(flushedCommands[i - 1].waitStages);
            }

            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &flushedCommands[i].cmd;

            if (i == (flushedCommands.size() - 1))
            {
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &renderFinishSemaphoreImage;
            }
            else
            {
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &flushedCommands[i].semaphore;
            }

            

            // Only attach the fence to the LAST submission
            vk::Fence fence = (i == flushedCommands.size() - 1) ? inFlightFence : VK_NULL_HANDLE;

            VK_CALL(mainQueu.submit(1, &submitInfo, fence));
        }
    }

    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;

    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(1);
    presentInfo.pWaitSemaphores = &renderFinishSemaphoreImage;

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

    flushedCommands.clear();
}

void Vulkan::VulkanSwapChain::HandleRecreateSwapChain(PC_CORE::Window* windowHandle)
{
    if (!windowHandle->resizeDirty)
        return;

    GET_VK_DEVICE.waitIdle();

    Tbx::Vector2ui size = windowHandle->GetWindowSize();
    CleanUpSwapChain();
    CreateSwapChain(size.x, size.y);
    CreateImageViews();
    CreateFrameBuffers();
    windowHandle->resizeDirty = false;
}

void Vulkan::VulkanSwapChain::BeginSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    auto vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);

    std::shared_ptr<VulkanRenderPass> renderPass = reinterpret_pointer_cast<VulkanRenderPass>(m_SwapChainRenderPass);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = m_Framebuffers.at(m_SwapChainImageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = vk::Extent2D{m_SwapChainWidth, m_SwapChainHeight};

    vk::ClearValue clearColor = {};
    clearColor.color.setFloat32({
        0, 0, 0, 0
    });

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vcommandList->GetVkHandle().beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanSwapChain::EndSwapChainRenderPass(PC_CORE::CommandList* _commandList)
{
    auto vcommandList = reinterpret_cast<VulkanCommandList*>(_commandList);
    vcommandList->GetVkHandle().endRenderPass();
}
