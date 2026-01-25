#include "VulkanFrameBuffer.hpp"

#include "VulkanContext.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanTexture.hpp"
#include "LowRenderer/Rhi.hpp"

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(PC_CORE::Rhi& _Rhi)
    : RhiFrameBuffer(_Rhi)
{
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        GET_VK_CONTEXT.rhiDevice);


    if (m_FrameBufferHandle != VK_NULL_HANDLE)
    {
        vulkanDevice->GetDevice().destroyFramebuffer(m_FrameBufferHandle);
        m_FrameBufferHandle = VK_NULL_HANDLE;
    }
    
}


bool Vulkan::VulkanFrameBuffer::Build()
{

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        GET_VK_CONTEXT.rhiDevice);

    assert(m_RenderPass != nullptr);

    auto renderPass = reinterpret_cast<const VulkanRenderPass*>(m_RenderPass);


    std::vector<vk::ImageView> image_views;
    image_views.reserve((m_DepthAttachement != nullptr) ? (m_Attachments.size() + 1) : m_Attachments.size());

    for (const PC_CORE::RhiTexture* attachement : m_Attachments)
    {
        if (attachement == nullptr)
        {
            PC_LOGERROR("One of the attachemtn was null")
                return false;
        }

        const VulkanTexture& VkTexture = *reinterpret_cast<const VulkanTexture*>(attachement);
        const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(VkTexture.GetTextureAndAlloc());
        image_views.emplace_back(textureAndAlloc->ImageView);
    }

    if (m_DepthAttachement != nullptr)
    {
        const VulkanTexture& VkTexture = *reinterpret_cast<const VulkanTexture*>(m_DepthAttachement);
        const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(VkTexture.GetTextureAndAlloc());
        image_views.emplace_back(textureAndAlloc->ImageView);
    }

    vk::FramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
    framebufferCreateInfo.flags = {};
    framebufferCreateInfo.renderPass = renderPass->GetVulkanRenderPass();
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(image_views.size());
    framebufferCreateInfo.pAttachments = image_views.data();
    framebufferCreateInfo.width = m_Width;
    framebufferCreateInfo.height = m_Height;
    framebufferCreateInfo.layers = 1;
    m_FrameBufferHandle = vulkanDevice->GetDevice().createFramebuffer(framebufferCreateInfo);

    vk::DebugUtilsObjectNameInfoEXT frameBufferDebugName;
    frameBufferDebugName.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
    frameBufferDebugName.pNext = nullptr;
    frameBufferDebugName.objectType = vk::ObjectType::eFramebuffer;
    frameBufferDebugName.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkFramebuffer>(m_FrameBufferHandle));
    frameBufferDebugName.pObjectName = GetName().data();
    SET_VK_DEBUG_NAME(frameBufferDebugName);

    return true;
}


vk::Framebuffer Vulkan::VulkanFrameBuffer::GetVkFramebuffer() const
{
    return m_FrameBufferHandle;
}
