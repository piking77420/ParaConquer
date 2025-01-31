#include "vulkan_swap_chain.hpp"

#include "vulkan_command_list.hpp"
#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_physical_devices.hpp"
#include "GLFW/glfw3.h"
#include "low_renderer/rhi.hpp"


Vulkan::VulkanSwapChain::VulkanSwapChain(uint32_t _widht, uint32_t _height): SwapChain(_widht, _height)
{
    CreateSwapChain(m_SwapChainWidth, m_SwapChainHeight);
    CreateImageViews();
    CreateFrameBuffers();
    CreateSyncObjects();
}

Vulkan::VulkanSwapChain::~VulkanSwapChain()
{
    DestroySyncObjects();
    CleanUpSwapChain();
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::GetSurfaceFormat()
{
    return m_SurfaceFormat;
}



bool Vulkan::VulkanSwapChain::GetSwapChainImageIndex(PC_CORE::Window* windowHandle)
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);
    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    // Wait for the fence to signal, ensuring the operation completes
    VK_CALL(vulkanDevice->GetDevice().waitForFences(1, &m_SyncObject[frameIndex].inFlightFence, VK_TRUE, UINT64_MAX));
  
    uint32_t nextImageIndex = 0;
    vk::Result result = vulkanDevice->GetDevice().acquireNextImageKHR(m_SwapChain, UINT64_MAX,
                                                                      m_SyncObject[frameIndex].imageAvailableSemaphore,
                                                                      VK_NULL_HANDLE, &nextImageIndex);
    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        HandleRecreateSwapChain(windowHandle);

        return false;
    }
    else if (vk::Result::eSuccess != result)
    {
        VK_CALL(result);

        return false;
    }

    m_SwapChainImageIndex = nextImageIndex;
    VK_CALL(vulkanDevice->GetDevice().resetFences(1, &m_SyncObject[frameIndex].inFlightFence));
    return true;
}

vk::SurfaceFormatKHR Vulkan::VulkanSwapChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace ==
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
    m_SwapChainFramebuffers.resize(m_SwapChainImage.size());

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
        m_SwapChainFramebuffers[i] = std::make_shared<VulkanFrameBuffer>(framebuffer, true);
    }
}

void Vulkan::VulkanSwapChain::CreateSyncObjects()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    for (size_t i = 0; i < m_SyncObject.size(); i++)
    {
        m_SyncObject[i].imageAvailableSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        m_SyncObject[i].renderFinishedSemaphore = vulkanDevice->GetDevice().createSemaphore(semaphoreInfo);
        m_SyncObject[i].inFlightFence = vulkanDevice->GetDevice().createFence(fenceInfo);
    }
}

void Vulkan::VulkanSwapChain::DestroySyncObjects()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        VulkanContext::GetContext().rhiDevice);
    for (size_t i = 0; i < m_SyncObject.size(); i++)
    {
        vulkanDevice->GetDevice().destroySemaphore(m_SyncObject[i].renderFinishedSemaphore);
        vulkanDevice->GetDevice().destroySemaphore(m_SyncObject[i].imageAvailableSemaphore);
        vulkanDevice->GetDevice().destroyFence(m_SyncObject[i].inFlightFence);
    }
}

void Vulkan::VulkanSwapChain::CleanUpSwapChain()
{
    VulkanContext& vulkanContext = VulkanContext::GetContext();
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice);

    m_SwapChainRenderPass.reset();
    m_SwapChainRenderPass = nullptr;

    for (const auto& frameBuffer : m_SwapChainFramebuffers)
    {
        std::shared_ptr<VulkanFrameBuffer> vframeBuffer = std::reinterpret_pointer_cast<VulkanFrameBuffer>(frameBuffer);
        device->GetDevice().destroyFramebuffer(vframeBuffer->GetFramebuffer());
    }

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
    m_SwapChainRenderPass = std::make_shared<VulkanRenderPass>(m_SurfaceFormat);


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
}

void Vulkan::VulkanSwapChain::Present(const PC_CORE::CommandList* _commandList, PC_CORE::Window* _window)
{
    const Vulkan::VulkanCommandList* vcommandList = reinterpret_cast<const Vulkan::VulkanCommandList*>(_commandList);
    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    vk::CommandBuffer commandBuffer = vcommandList->GetHandle();
    const vk::Queue& queue = *vcommandList->GetQueue();

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;

    vk::Semaphore waitSemaphores[] = { m_SyncObject[frameIndex].imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vk::Semaphore signalSemaphores[] = { m_SyncObject[frameIndex].renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Ensure proper error handling for queue submission
    VK_CALL(queue.submit(1, &submitInfo, m_SyncObject[frameIndex].inFlightFence)); // Changed `queu` to `queue`

    vk::PresentInfoKHR presentInfo{};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { m_SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_SwapChainImageIndex;

    // Check if the presentation queue supports presentation and handle errors
    vk::Result result = VulkanContext::GetContext().presentQueue.presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
    {
        HandleRecreateSwapChain(_window);
    }
    else if (result != vk::Result::eSuccess)
    {
        VK_CALL(result);
    }
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
