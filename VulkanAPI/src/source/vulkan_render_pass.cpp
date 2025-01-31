#include "vulkan_render_pass.hpp"

#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"

Vulkan::VulkanRenderPass::VulkanRenderPass(vk::SurfaceFormatKHR surfaceFormat)
{
    std::shared_ptr<Vulkan::VulkanDevice> device = std::reinterpret_pointer_cast<Vulkan::VulkanDevice>(VulkanContext::GetContext().rhiDevice);

    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = surfaceFormat.format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
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
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    m_RenderPass = device->GetDevice().createRenderPass(renderPassInfo);
}

Vulkan::VulkanRenderPass::~VulkanRenderPass()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice);
    if (m_RenderPass == VK_NULL_HANDLE)
        return;
    
    vulkanDevice->GetDevice().destroyRenderPass(m_RenderPass);
    m_RenderPass = VK_NULL_HANDLE;
}

vk::RenderPass Vulkan::VulkanRenderPass::GetVulkanRenderPass() const
{
    return m_RenderPass;
}
