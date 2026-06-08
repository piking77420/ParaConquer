#include "VulkanFrameBuffer.hpp"

#include "VulkanContext.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanTexture.hpp"
#include "LowRenderer/Rhi.hpp"
#include <Utils/RhiToVulkan.hpp>

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(PC_CORE::Rhi& _Rhi)
    : RhiFrameBuffer(_Rhi)
{
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    auto& Context = GET_VK_CONTEXT;
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(Context.rhiDevice);

    if (m_FrameBufferHandle != VK_NULL_HANDLE)
    {
        vulkanDevice->GetDevice().destroyFramebuffer(m_FrameBufferHandle);
        m_FrameBufferHandle = VK_NULL_HANDLE;
    }
    
    for (auto& ImgView : m_ImageViews)
        Context.DefferdDestroy(ImgView, m_Rhi.GetFrameIndex());
}


bool Vulkan::VulkanFrameBuffer::Build()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        GET_VK_CONTEXT.rhiDevice);

    assert(m_RenderPass != nullptr);

    const VulkanRenderPass* renderPass = reinterpret_cast<const VulkanRenderPass*>(m_RenderPass);
    vk::Device Device = vulkanDevice->GetDevice();

    m_ImageViews.clear();
    m_ImageViews.reserve((m_DepthAttachement) ? (m_Attachments.size() + 1) : m_Attachments.size());

    auto ImageViewFromAttachement = [&](const AttachementDescriptor& Attachement, const VulkanTexture& VkTexture) {

        vk::ImageView imageView = VK_NULL_HANDLE;
        vk::ImageViewCreateInfo imageviewInfo = VkTexture.GetImageViewCreateInfo(Utils::RhiImageToVkImageViewType(Attachement.ViewType), Attachement.BaseLayer, Attachement.LayerCount, Attachement.BaseLevel, Attachement.LevelCount);
        VK_CALL(Device.createImageView(&imageviewInfo, nullptr, &imageView));

            vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
            nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
            nameInfoImageView.pNext = nullptr;
            nameInfoImageView.objectType = vk::ObjectType::eImageView;
            nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(imageView));
            std::string Name = std::string(GetName().data()) + "VulkanFrameBuffer ImageView";
            nameInfoImageView.pObjectName = Name.c_str();

            SET_VK_DEBUG_NAME(nameInfoImageView);
            return imageView;
        };

    for (const auto& Attachement : m_Attachments)
    {
        if (Attachement.Texture == nullptr)
        {
            PC_LOGERROR("One of the attachemtn was null")
                return false;
        }

        const VulkanTexture& VkTexture = *reinterpret_cast<const VulkanTexture*>(Attachement.Texture);
        const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(VkTexture.GetTextureAndAlloc());

        m_ImageViews.emplace_back(ImageViewFromAttachement(Attachement, VkTexture));
    }

    if (m_DepthAttachement)
    {
        const VulkanTexture& VkTexture = *reinterpret_cast<const VulkanTexture*>((m_DepthAttachement)->Texture);
        const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(VkTexture.GetTextureAndAlloc());
        m_ImageViews.emplace_back(ImageViewFromAttachement(*m_DepthAttachement, VkTexture));
    }

    vk::FramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
    framebufferCreateInfo.flags = {};
    framebufferCreateInfo.renderPass = renderPass->GetVulkanRenderPass();
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(m_ImageViews.size());
    framebufferCreateInfo.pAttachments = m_ImageViews.data();
    framebufferCreateInfo.width = GetWidth();
    framebufferCreateInfo.height = GetHeight();
    framebufferCreateInfo.layers = GetLayer();
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
